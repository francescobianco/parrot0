"""prose-mkq.py — genera la parte META e STRUTTURA di un file `.q` della scala
della prosa, con le risposte calcolate dal TESTO, non da parrot0.

⛔ E' questo il punto: se le attese le producesse parrot0, il banco misurerebbe
la propria eco. Il conto delle frasi e delle parole, la prima frase, l'ultima e
il tema si ricavano qui, dal file; se parrot0 risponde un altro numero e' lui a
sbagliare, e il banco lo deve dire. E' successo due volte nel giro 2 — «311
parole» su un testo di 299, e il tema che murava sui testi senza determinante —
ed e' cosi' che le due cose sono state trovate.

Le domande NEL MERITO si scrivono a mano leggendo il testo: la loro risposta
deve stare scritta nella prosa, altrimenti il piolo misura l'indovinare.

Il conto delle frasi e delle parole viene da qui, con la convenzione del
tokenizzatore dichiarata in count.py; se parrot0 risponde un altro numero e'
lui a sbagliare, e il banco lo deve dire. Le domande nel MERITO le scrivo a
mano leggendo il testo: la loro risposta deve stare scritta nella prosa.

Uso:  python3 mkq.py tests/fixtures/prose/ladder/r300.txt
"""
import re, sys

# Le parole sono quelle che conterebbe una persona: corse separate da spazio.
# (Il flusso di TOKEN e' un'altra cosa e spezza «0.1%» — vedi word_separator/1.)


def sentences(t):
    return [s.strip() for s in re.split(r'(?<=[.;]) ', t) if s.strip()]


def ere(s):
    """Cita per grep -E: il testo di Wikipedia ha (), %, $, -, |, ."""
    return re.sub(r'([.^$*+?()\[\]{}|\\])', r'\\\1', s)


def slice_words(s, a, b):
    return ' '.join(s.split()[a:b])


def main(path):
    t = open(path).read().strip()
    ss = sentences(t)
    nw = len(t.split())
    rows = []
    rows.append(("how many sentences does the text have?",
                 "has %d sentences" % len(ss), "struttura"))
    rows.append(("how many words does the text have?",
                 "has %d words" % nw, "struttura"))
    # una fetta centrale della prima frase: non la prima parola (che comparirebbe
    # per caso in mezza risposta) e non tutta la frase (che il lettore puo'
    # troncare a fine riga)
    rows.append(("what is the first sentence?",
                 ere(slice_words(ss[0], 1, 7)), "struttura"))
    rows.append(("how does the text begin?",
                 ere(slice_words(ss[0], 1, 7)), "struttura"))
    rows.append(("what is the last sentence?",
                 ere(slice_words(ss[-1], 0, 6)), "struttura"))
    head = ss[0].split()
    topic = head[1] if head[0].lower() in ("a", "an", "the", "in") else head[0]
    topic = topic.strip(',').lower()
    rows.append(("what is this text about?", ere(topic), "meta"))
    rows.append(("what is the topic?", ere(topic), "meta"))
    for q, a, k in rows:
        print("%s\t%s\t\t%s" % (q, a, k))


if __name__ == '__main__':
    main(sys.argv[1])
