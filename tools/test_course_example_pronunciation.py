#!/usr/bin/env python3
"""Regression checks for example reading hints and their lexical stress."""
import unittest
from build_course_catalogs import PHONES
from course_example_pronunciation import pronounce_example


class ExamplePronunciationTest(unittest.TestCase):
    def test_stress_and_punctuation(self):
        self.assertEqual(pronounce_example('I am ready.', PHONES), 'ай эм рэ́ди.')
        self.assertEqual(pronounce_example('Can you help me?', PHONES), 'кэн ю хэлп ми?')

    def test_context_changes_reading(self):
        self.assertIn('лайвз', pronounce_example('The war changed many lives.', PHONES))
        self.assertIn('ливз', pronounce_example('My family lives here.', PHONES))
        self.assertIn('рид', pronounce_example('Can you read this?', PHONES))
        self.assertIn('рэд', pronounce_example('I read an interesting article.', PHONES))
        self.assertIn('рико́рд', pronounce_example('Record the heart rate.', PHONES))
        self.assertIn('рэ́кэрд', pronounce_example('The record already exists.', PHONES))
        self.assertIn('юс', pronounce_example('Is this medication for oral use?', PHONES))
        self.assertIn('юз', pronounce_example('Can I use your phone?', PHONES))

    def test_abbreviations_numbers_and_contractions(self):
        self.assertEqual(pronounce_example('Use UTF-8 encoding.', PHONES),
                         'юз ю ти эф-эйт энко́удинг.')
        self.assertTrue(pronounce_example("Let's take a break.", PHONES).startswith('лэтс'))
        self.assertIn('эм ар ай', pronounce_example('MRI', PHONES))

    def test_primary_stress_is_unambiguous(self):
        hint = pronounce_example('engineer', PHONES)
        self.assertEqual(hint.count('\u0301'), 1)
        self.assertTrue(hint.endswith('ни́р'))

    def test_unknown_words_do_not_silently_disappear(self):
        with self.assertRaises(KeyError):
            pronounce_example('Unlistedmadeupword.', PHONES)


if __name__ == '__main__':
    unittest.main()
