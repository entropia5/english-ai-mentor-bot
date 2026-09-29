-- Переносим подсказку к английскому примеру, сохраняя перевод и прогресс.
UPDATE words
SET definition_ru = regexp_replace(
    definition_ru,
    E'(Пример: [^\n]+)\n(Перевод: [^\n]+) · ≈ ([^\n]+)$',
    E'\\1 · ≈ \\3\n\\2'
)
WHERE topic IN ('conversation', 'medicine', 'it')
  AND definition_ru ~ E'Пример: [^\n]+\nПеревод: [^\n]+ · ≈ [^\n]+$';
