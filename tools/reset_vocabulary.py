#!/usr/bin/env python3
"""Explicit operator-only reset. Stop all bot writers before invoking this command.

Makes a private full database backup, verifies it by restoring a disposable database,
then applies the new schema and clears only vocabulary and vocabulary-related progress.
It never sends Telegram messages or deletes users/conversations. No reset on startup.
"""
import argparse
import datetime
import json
import os
import re
import subprocess
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--container', required=True)
parser.add_argument('--config', default='.env')
parser.add_argument('--binary', required=True, help='New bot executable; used only for migrations')
parser.add_argument('--backup-dir', default='data/backups')
parser.add_argument('--confirm-all-users', action='store_true')
args = parser.parse_args()
if not args.confirm_all_users:
    parser.error('Explicit --confirm-all-users is required. This clears learned words too.')
config_path = Path(args.config).resolve()
config = {}
for line in config_path.read_text().splitlines():
    if line and not line.startswith('#') and '=' in line:
        key, value = line.split('=', 1)
        config[key.strip()] = value.strip().strip('"')
name = config.get('DB_NAME', 'english_mentor')
user = config.get('DB_USER', 'n8n')
if name in {'postgres', 'template0', 'template1'} or not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name):
    raise SystemExit('Refusing an unsafe database name')
binary = Path(args.binary).resolve(strict=True)
base = ['docker', 'exec', '-i', args.container]

def sql(query, database=name):
    result = subprocess.run(base + ['psql', '-X', '-v', 'ON_ERROR_STOP=1', '-U', user,
                                   '-d', database, '-At'], input=query, text=True,
                            check=True, capture_output=True)
    return result.stdout.strip()

count_query = '''SELECT json_build_object('users',(SELECT count(*) FROM users),
    'words',(SELECT count(*) FROM words),'learned',(SELECT count(*) FROM words WHERE is_learned),
    'conversations',(SELECT count(*) FROM conversations));'''
before = json.loads(sql(count_query))
stamp = datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ')
directory = Path(args.backup_dir).resolve() / ('courses-reset-' + stamp)
directory.mkdir(parents=True, mode=0o700)
directory.chmod(0o700)
backup = directory / (name + '.dump')
fd = os.open(backup, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
with os.fdopen(fd, 'wb') as output:
    subprocess.run(base + ['pg_dump', '-U', user, '-d', name, '-Fc', '--no-owner', '--no-acl'],
                   stdout=output, check=True)
verify_name = 'mentor_courses_test_restore_' + str(os.getpid())
sql('CREATE DATABASE ' + verify_name + ';', 'postgres')
try:
    with backup.open('rb') as source:
        subprocess.run(base + ['pg_restore', '-U', user, '-d', verify_name,
                               '--no-owner', '--no-acl', '--exit-on-error'], stdin=source, check=True)
    restored = json.loads(sql(count_query, verify_name))
    if restored != before:
        raise RuntimeError('Backup count mismatch; reset refused. Stop all writers first.')
finally:
    sql('DROP DATABASE ' + verify_name + ' WITH (FORCE);', 'postgres')
print('Backup restored and verified:', before, flush=True)
# The bot reads .env in its working directory; require the configured file to match that contract.
if config_path.name != '.env':
    raise RuntimeError('For migrations the config filename must be .env')
subprocess.run([str(binary), '--cleanup-db'], cwd=config_path.parent, check=True)
# Refuse the reset if the new schema has not been applied to this database.
if sql("SELECT count(*) FROM schema_migrations WHERE version='003_courses.sql';") != '1':
    raise RuntimeError('Course migration missing in the target database')
sql('''BEGIN;
SET LOCAL lock_timeout = '5s';
LOCK TABLE words, users IN ACCESS EXCLUSIVE MODE;
DELETE FROM evening_word_batches;
DELETE FROM words;
UPDATE users SET level=1, streak_days=0, last_daily_sent=0,
    new_words_page=0, learned_words_page=0, last_viewed_dict='new', active_course='conversation';
COMMIT;''')
after = json.loads(sql(count_query))
if after['words'] or after['learned'] or after['users'] != before['users'] or after['conversations'] != before['conversations']:
    raise RuntimeError('Unexpected post-reset counts; inspect the saved backup before continuing')
manifest = directory / 'manifest.json'
manifest.write_text(json.dumps({'backup': str(backup), 'before': before, 'after': after,
                                'restore_verified': True}, indent=2) + '\n')
manifest.chmod(0o600)
print('Reset verified:', after, flush=True)
print('Backup:', backup, flush=True)
