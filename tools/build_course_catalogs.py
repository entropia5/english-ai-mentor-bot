#!/usr/bin/env python3
"""Build deterministic course JSON from reviewed TSV and checked-in phoneme data.

No API calls. CMUdict source and license are recorded in resources/courses.
Russian hints are approximate sound guides, never a substitute for audio/IPA.
"""
import json
import re
from pathlib import Path

BASE = Path(__file__).resolve().parents[1] / 'resources' / 'courses'
PHONES = json.loads((BASE / 'pronunciations.json').read_text())['phones']
# Additional words and context-sensitive readings, authored for these courses.
EXTRA = {
 'antenatal':'AE2 N T IY0 N EY1 T AH0 L',
 'anticoagulant':'AE2 N T IY0 K OW0 AE1 G Y AH0 L AH0 N T',
 'aseptic':'EY0 S EH1 P T IH0 K',
 'auscultation':'AO2 S K AH0 L T EY1 SH AH0 N',
 'breakpoint':'B R EY1 K P OY2 N T', 'changelog':'CH EY1 N JH L AO2 G',
 'concurrency':'K AH0 N K ER1 AH0 N S IY0',
 'contraindication':'K AA2 N T R AH0 IH2 N D AH0 K EY1 SH AH0 N',
 'creatinine':'K R IY0 AE1 T IH0 N IY2 N', 'deprecation':'D EH2 P R AH0 K EY1 SH AH0 N',
 'dysphagia':'D IH0 S F EY1 JH AH0', 'dysuria':'D IH0 S Y UH1 R IY0 AH0',
 'executable':'EH0 G Z EH1 K Y AH0 T AH0 B AH0 L',
 'hematemesis':'HH IY2 M AH0 T EH1 M AH0 S IH0 S',
 'hematuria':'HH IY2 M AH0 T UH1 R IY0 AH0',
 'hemoptysis':'HH IY0 M AA1 P T AH0 S IH0 S',
 'hyperglycemia':'HH AY2 P ER0 G L AY0 S IY1 M IY0 AH0',
 'idempotent':'AY2 D AH0 M P OW1 T AH0 N T',
 'intramuscular':'IH2 N T R AH0 M AH1 S K Y AH0 L ER0',
 'maintainability':'M EY0 N T EY2 N AH0 B IH1 L AH0 T IY0',
 'maintainer':'M EY0 N T EY1 N ER0', 'melena':'M AH0 L IY1 N AH0',
 'motorway':'M OW1 T ER0 W EY2', 'mutex':'M Y UW1 T EH2 K S',
 'orthopnea':'AO2 R TH AA0 P N IY1 AH0', 'palpation':'P AE0 L P EY1 SH AH0 N',
 'physiotherapy':'F IH2 Z IY0 OW0 TH EH1 R AH0 P IY0',
 'readme':'R IY1 D M IY2', 'rebase':'R IY0 B EY1 S', 'refactor':'R IY0 F AE1 K T ER0',
 'runtime':'R AH1 N T AY2 M', 'serialization':'S IH2 R IY0 AH0 L AH0 Z EY1 SH AH0 N',
 'stridor':'S T R AY1 D ER0', 'tls':'T IY1 EH1 L EH1 S',
 'toothache':'T UW1 TH EY2 K', 'uptime':'AH1 P T AY2 M',
 'urination':'Y UH2 R AH0 N EY1 SH AH0 N', 'webhook':'W EH1 B HH UH2 K',
 'read':'R IY1 D', 'live':'L IH1 V', 'use':'Y UW1 Z', 'close':'K L OW1 Z',
 'wind':'W IH1 N D', 'permit':'P ER1 M IH2 T', 'lead':'L IY1 D', 'resume':'R EH1 Z AH0 M EY2', 'record':'R EH1 K ER0 D',
 'refuse':'R IH0 F Y UW1 Z', 'minute':'M IH1 N AH0 T',
 'wound':'W UW1 N D', 'bow':'B OW1', 'does':'D AH1 Z',
 'api':'EY1 P IY1 AY1', 'http':'EY1 CH T IY1 T IY1 P IY1',
 'url':'Y UW1 AA1 R EH1 L', 'dns':'D IY1 EH1 N EH1 S',
 'ct':'S IY1 T IY1', 'mri':'EH1 M AA1 R AY1', 'x':'EH1 K S',
 't':'T IY1', 'a':'EY1', 'an':'AE1 N', 'the':'DH AH0', 'of':'AH0 V',
}
PHONES.update({k:v.split() for k,v in EXTRA.items()})
IPA = dict(zip(
 'AA AE AH AO AW AY B CH D DH EH ER EY F G HH IH IY JH K L M N NG OW OY P R S SH T TH UH UW V W Y Z ZH'.split(),
 'ɑ æ ʌ ɔ aʊ aɪ b tʃ d ð ɛ ɝ eɪ f ɡ h ɪ i dʒ k l m n ŋ oʊ ɔɪ p ɹ s ʃ t θ ʊ u v w j z ʒ'.split()))
RU = dict(zip(
 'AA AE AH AO AW AY B CH D DH EH ER EY F G HH IH IY JH K L M N NG OW OY P R S SH T TH UH UW V W Y Z ZH'.split(),
 'а э а о ау ай б ч д з э ёр эй ф г х и и дж к л м н нг оу ой п р с ш т с у у в у й з ж'.split()))
VOWELS = set('AA AE AH AO AW AY EH ER EY IH IY OW OY UH UW'.split())
ONSETS = {tuple(x.split()) for x in [
 'P R','B R','T R','D R','K R','G R','F R','TH R','SH R',
 'P L','B L','K L','G L','F L','S L','K W','G W','S W','T W','D W',
 'S P','S T','S K','S M','S N','S F','S P R','S T R','S K R','S P L','S K W',
 'P Y','B Y','F Y','V Y','K Y','G Y','HH Y','M Y','N Y','S K Y',
]}

def render_phonemes(phones):
    bases = [re.sub(r'\d','',p) for p in phones]
    vowels = [i for i,b in enumerate(bases) if b in VOWELS]
    marks = {}
    if len(vowels) > 1:
        for j, i in enumerate(vowels):
            stress = phones[i][-1]
            if stress not in '12': continue
            start = 0 if j == 0 else i
            if j:
                for k in range(vowels[j-1]+1, i):
                    cluster = tuple(bases[k:i])
                    if len(cluster)==1 and cluster[0]!='NG' or cluster in ONSETS:
                        start = k; break
            marks[start] = 'ˈ' if stress == '1' else 'ˌ'
    ipa = ''; ru = ''
    for i,(base,phone) in enumerate(zip(bases,phones)):
        sound = IPA[base]
        if phone == 'AH0': sound = 'ə'
        if phone == 'ER0': sound = 'ɚ'
        ipa += marks.get(i,'') + sound
        hint = RU[base]
        if phone == 'AH0': hint = 'э'
        if phone.endswith('1'):
            hint = re.sub('([аэиоуыёеюя])', r'\1́', hint, count=1)
        ru += hint
    return ipa, ru

def pronounce(english):
    key = english.lower()
    tokens = [key] if key in PHONES else re.findall(r"[a-z]+(?:'[a-z]+)?", key)
    rendered = [render_phonemes(PHONES[token]) for token in tokens]
    overrides = {
        'api': ('/ˌeɪ pi ˈaɪ/', 'эй пи ай'),
        'http': ('/ˌeɪtʃ ti ti ˈpi/', 'эйч ти ти пи'),
        'url': ('/ˌju ɑɹ ˈɛl/', 'ю ар эл'),
        'dns': ('/ˌdi ɛn ˈɛs/', 'ди эн эс'),
        'tls': ('/ˌti ɛl ˈɛs/', 'ти эл эс'),
    }
    if key in overrides: return overrides[key]
    return '/'+' '.join(p[0] for p in rendered)+'/', ' '.join(p[1] for p in rendered)

def build(course):
    words=[]; seen=set();lesson=''
    for n,line in enumerate((BASE/(course+'.tsv')).read_text().splitlines(),1):
        if line.startswith('# '): lesson=line[2:];continue
        if not line.strip():continue
        fields=line.split('|')
        if len(fields)!=3 or any(not x.strip() for x in fields):
            raise ValueError(f'{course}:{n}: invalid record')
        english,translation,example=fields
        key=english.casefold()
        if key in seen:raise ValueError(f'{course}:{n}: duplicate {english}')
        if not lesson:raise ValueError('Missing lesson')
        seen.add(key)
        ipa,ru=pronounce(english)
        words.append(dict(english=english,translation=translation,example=example,lesson=lesson,
                          transcription=ipa,pronunciation='≈ '+ru))
    if course=='conversation' and len(words)!=2000:raise ValueError('Conversation must contain 2000 entries')
    output=dict(version=1,course=course,pronunciation_note='US broad transcription from CMUdict plus authored additions; Russian hints are approximate.',words=words)
    path=BASE/(course+'.json'); path.write_text(json.dumps(output,ensure_ascii=False,indent=2)+'\n')
    print(course,len(words))

if __name__=='__main__':
    for course in ['conversation','medicine','it']:build(course)
