#!/usr/bin/env python3
"""Test example backfill in a disposable PostgreSQL database."""
import argparse
import json
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--container', required=True)
p.add_argument('--user', default='n8n')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
name = 'mentor_courses_test_examples_' + str(os.getpid())
base = ['docker', 'exec', '-i', a.container, 'psql', '-X', '-v', 'ON_ERROR_STOP=1', '-U', a.user, '-At']
def sql(text, db=name):
    return subprocess.run(base + ['-d', db], input=text, text=True, capture_output=True, check=True).stdout.strip()
def quote(text):
    return "'" + text.replace("'", "''") + "'"
sql('CREATE DATABASE ' + name, 'postgres')
try:
    for migration in sorted((root / 'migrations').glob('*.sql')):
        if migration.name < '005':
            sql(migration.read_text())
    sql('INSERT INTO users(user_id) VALUES(1),(2)')
    for i, course in enumerate(('conversation','medicine','it')):
        w = json.loads((root / 'resources/courses' / (course + '.json')).read_text())['words'][0]
        original = w['lesson'] + '\nПример: ' + w['example']
        for user in (1, 2):
            sql('INSERT INTO words(user_id,english,topic,definition_ru,is_learned,repetition_count,next_review) VALUES (' +
                f'{user},' + ','.join(map(quote, (w['english'],course,original))) + f',{str(user == 2).lower()},7,123456)')
    sql("INSERT INTO words(user_id,english,topic,definition_ru) VALUES(1,'custom','it','My custom example')")
    snapshot = "SELECT json_agg(to_jsonb(w)-'definition_ru' ORDER BY id)::text FROM words w"
    before = sql(snapshot)
    migration = (root / 'migrations/005_example_translations.sql').read_text()
    sql(migration)
    assert sql(snapshot) == before, 'Progress or identity changed'
    assert sql("SELECT count(*) FROM words WHERE definition_ru LIKE '%Перевод: %'") == '6'
    assert sql("SELECT definition_ru FROM words WHERE english='custom'") == 'My custom example'
    first = sql('SELECT json_agg(w ORDER BY id)::text FROM words w')
    sql(migration)
    assert sql('SELECT json_agg(w ORDER BY id)::text FROM words w') == first, 'Not idempotent'
    reading = (root / 'migrations/006_example_pronunciations.sql').read_text()
    sql(reading)
    assert sql(snapshot) == before, 'Pronunciation migration changed progress'
    assert sql("SELECT count(*) FROM words WHERE definition_ru LIKE '%Перевод: % · ≈ %'") == '6'
    assert sql("SELECT count(*) FROM words WHERE definition_ru LIKE '% · ≈ %' AND position(chr(769) in definition_ru)>0") == '6'
    assert sql("SELECT definition_ru FROM words WHERE english='custom'") == 'My custom example'
    updated = sql('SELECT json_agg(w ORDER BY id)::text FROM words w')
    sql(reading)
    assert sql('SELECT json_agg(w ORDER BY id)::text FROM words w') == updated, 'Pronunciation duplicated'
    position = (root / 'migrations/007_example_pronunciation_position.sql').read_text()
    sql(position)
    assert sql(snapshot) == before, 'Position migration changed progress'
    assert sql("SELECT count(*) FROM words WHERE definition_ru ~ E'Пример: [^\\n]+ · ≈ [^\\n]+\\nПеревод: '") == '6'
    assert sql("SELECT definition_ru FROM words WHERE english='custom'") == 'My custom example'
    moved = sql('SELECT json_agg(w ORDER BY id)::text FROM words w')
    sql(position)
    assert sql('SELECT json_agg(w ORDER BY id)::text FROM words w') == moved, 'Position migration not idempotent'
    print('PASS: all courses, multiple users, learned words, progress preservation, custom text, pronunciation position, idempotency')
finally:
    sql('DROP DATABASE ' + name + ' WITH (FORCE)', 'postgres')
