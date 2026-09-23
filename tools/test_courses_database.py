#!/usr/bin/env python3
"""Run integration checks in a disposable database on the configured local PostgreSQL container."""
import argparse, os, subprocess, tempfile, time
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--container',required=True);p.add_argument('--config',default='.env');p.add_argument('--binary',default='build/courses/english_mentor_database_tests');a=p.parse_args()
config={}
for line in Path(a.config).read_text().splitlines():
 if line and not line.startswith('#') and '=' in line:
  k,v=line.split('=',1);config[k.strip()]=v.strip().strip('"')
name='mentor_courses_test_'+str(os.getpid())+'_'+str(int(time.time()))
user=config.get('DB_USER','n8n')
cmd=['docker','exec','-i',a.container,'psql','-X','-v','ON_ERROR_STOP=1','-U',user,'-d','postgres']
subprocess.run(cmd,input=f'CREATE DATABASE {name};',text=True,check=True,capture_output=True)
try:
 with tempfile.TemporaryDirectory(prefix='mentor-courses-test-') as directory:
  cfg=Path(directory)/'.env';cfg.write_text('\n'.join(f'{k}={v}' for k,v in config.items() if k.startswith('DB_') and k!='DB_NAME')+'\nDB_NAME='+name+'\n');cfg.chmod(0o600)
  env=os.environ.copy();env['MENTOR_TEST_CONFIG']=str(cfg)
  result=subprocess.run([str(Path(a.binary).resolve())],env=env)
  if result.returncode:raise SystemExit(result.returncode)
finally:
 subprocess.run(cmd,input=f'DROP DATABASE {name} WITH (FORCE);',text=True,check=True,capture_output=True)
