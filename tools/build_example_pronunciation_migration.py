#!/usr/bin/env python3
"""Generate the one-time backfill from catalog data, without changing learning progress.

Regenerate before releasing 006. After deployment, use a new migration for corrections.
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def quote(value):
    return "'" + value.replace("'", "''") + "'"

rows = []
for course in ('conversation', 'medicine', 'it'):
    for word in json.loads((ROOT / 'resources/courses' / (course + '.json')).read_text())['words']:
        original = (word['lesson'] + '\nПример: ' + word['example'] +
                    '\nПеревод: ' + word['example_translation'])
        rows.append('(' + ','.join(map(quote, (course, word['english'].lower(), original,
                                              ' · ≈ ' + word['example_pronunciation']))) + ')')

sql = '''-- Add pronunciations only to unchanged catalog examples, including learned words.
-- IDs, original examples, user data and all progress fields remain intact.
UPDATE words AS w
SET definition_ru = w.definition_ru || v.suffix
FROM (VALUES
''' + ',\n'.join(rows) + '''
) AS v(course, english, original, suffix)
WHERE w.topic = v.course AND lower(trim(w.english)) = v.english
  AND w.definition_ru = v.original;
'''
(ROOT / 'migrations/006_example_pronunciations.sql').write_text(sql)
