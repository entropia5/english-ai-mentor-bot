"""Approximate Cyrillic reading hints for the checked-in example sentences.

CMU phonemes supply lexical stress. These hints do not encode sentence intonation.
Context overrides apply only to reviewed sentences, never guessed from spelling.
"""
import re

EXTRA_PHONES = {
    'contraindications': 'K AA2 N T R AH0 IH2 N D AH0 K EY1 SH AH0 N Z',
    'json': 'JH EY1 S AH0 N',
    "object's": 'AA1 B JH EH0 K T S',
    'reproducible': 'R IY2 P R AH0 D UW1 S AH0 B AH0 L',
    'retries': 'R IY0 T R AY1 Z',
    'timestamp': 'T AY1 M S T AE2 M P',
    'typos': 'T AY1 P OW0 Z',
    'untrusted': 'AH0 N T R AH1 S T IH0 D',
    'utf': 'Y UW1 T IY1 EH1 F',
    'vomited': 'V AA1 M AH0 T IH0 D',
    # Verb inflection; CMU's first entry is the plural noun /uses/.
    'uses': 'Y UW1 Z IH0 Z',
    # Articles use their weak forms inside sentences.
    'a': 'AH0',
    'an': 'AH0 N',
}

ABBREVIATIONS = {
    'API': 'эй пи ай', 'HTTP': 'эйч ти ти пи', 'URL': 'ю ар эл',
    'DNS': 'ди эн эс', 'TLS': 'ти эл эс', 'CT': 'си ти',
    'MRI': 'эм ар ай', 'HTML': 'эйч ти эм эл', 'UTF': 'ю ти эф',
    'JSON': 'джэ́йсэн', 'README': 'ри́дми', 'OK': 'оукэ́й',
}

# Distinguish nouns, verbs and past-tense read in these particular examples.
CONTEXT_PHONES = {
    'The war changed many lives.': {'lives': 'L AY1 V Z'},
    'I read an interesting article.': {'read': 'R EH1 D'},
    'Is this medication for oral use?': {'use': 'Y UW1 S'},
    'Release the resource after use.': {'use': 'Y UW1 S'},
    'There is a trade-off between speed and memory use.': {'use': 'Y UW1 S'},
    'Delete only the selected records.': {'records': 'R EH1 K ER0 D Z'},
    'Here is a quick progress update.': {'update': 'AH1 P D EY2 T'},
}

VOWELS = set('AA AE AH AO AW AY EH ER EY IH IY OW OY UH UW'.split())
HINTS = dict(zip(
    'AA AE AH AO AW AY B CH D DH EH ER EY F G HH IH IY JH K L M N NG OW OY P R S SH T TH UH UW V W Y Z ZH'.split(),
    'а э а о ау ай б ч д з э ёр эй ф г х и и дж к л м н нг оу ой п р с ш т с у у в у й з ж'.split()))


def render_hint(phones):
    syllables = sum(p.rstrip('012') in VOWELS for p in phones)
    primary = max((i for i, p in enumerate(phones) if p.endswith('1')), default=-1)
    result = ''
    for index, phone in enumerate(phones):
        base = phone.rstrip('012')
        hint = HINTS[base]
        if phone == 'AH0':
            hint = 'э'
        elif phone == 'ER0':
            hint = 'эр'
        # One-syllable words have no ambiguous lexical stress.
        if syllables > 1 and index == primary:
            hint = re.sub('([аэиоуыёеюя])', r'\1́', hint, count=1)
        result += hint
    return result.replace('йу', 'ю')


def pronounce_example(sentence, phones):
    context = CONTEXT_PHONES.get(sentence, {})

    def replace(match):
        text = match.group()
        if text in ABBREVIATIONS:
            return ABBREVIATIONS[text]
        key = text.lower()
        if key == '8':
            return 'эйт'
        if key in context:
            sounds = context[key].split()
        elif key == 'record' and sentence.startswith('Record '):
            sounds = 'R IH0 K AO1 R D'.split()
        elif key in EXTRA_PHONES:
            sounds = EXTRA_PHONES[key].split()
        else:
            sounds = phones[key]  # Unknown tokens fail the build, never disappear silently.
        return render_hint(sounds)

    hint = re.sub(r"[A-Za-z]+(?:'[A-Za-z]+)?|\d+", replace, sentence)
    if re.search('[A-Za-z0-9]', hint) or '\n' in hint:
        raise ValueError('Incomplete example pronunciation: ' + sentence)
    return hint
