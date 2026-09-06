# CEFR-SP — attribuzione, licenze e citazione

I dati in `data/` **non sono nostri**. Vengono da **CEFR-SP**, un corpus di
frasi inglesi annotate con il livello CEFR da professionisti dell'insegnamento
dell'inglese.

## Autori e opera

**Yuki Arase, Satoru Uchida, Tomoyuki Kajiwara.**
*CEFR-Based Sentence Difficulty Annotation and Assessment.*
Proceedings of EMNLP 2022, pp. 6206–6219.
<https://aclanthology.org/2022.emnlp-main.416> · doi:10.18653/v1/2022.emnlp-main.416

**Satoru Uchida, Yuki Arase, Tomoyuki Kajiwara.**
*Profiling English sentences based on CEFR levels.*
ITL — International Journal of Applied Linguistics, 175(1), 103–126, 2024.
doi:10.1075/itl.22018.uch

Repository originale: <https://github.com/yukiar/CEFR-SP>

```bibtex
@inproceedings{arase-etal-2022-cefr,
    title = "{CEFR}-Based Sentence Difficulty Annotation and Assessment",
    author = "Arase, Yuki and Uchida, Satoru and Kajiwara, Tomoyuki",
    booktitle = "Proceedings of the 2022 Conference on Empirical Methods in
                 Natural Language Processing",
    month = dec, year = "2022", address = "Abu Dhabi, United Arab Emirates",
    publisher = "Association for Computational Linguistics",
    url = "https://aclanthology.org/2022.emnlp-main.416",
    doi = "10.18653/v1/2022.emnlp-main.416",
    pages = "6206--6219",
}
```

## Che cosa è incluso qui, e che cosa NON lo è

| porzione | licenza | qui |
|---|---|---|
| **Wiki-Auto** (7.453 frasi) | **CC BY-SA 3.0** | ✅ inclusa in `data/` |
| **SCoRE** (2.551 frasi) | **CC BY-NC-SA 4.0** | ⛔ **non inclusa** — vedi sotto |
| **Newsela-Auto** | licenza Newsela | ⛔ non distribuita dagli autori stessi |

### Perché SCoRE non è nel repository

La clausola **NonCommercial** si trasmette a chi riceve il repository: chiunque
usasse parrot0 in un contesto commerciale dovrebbe prima **rimuovere** quei file.
Imporre quel vincolo in silenzio a chi clona non ci sembra corretto, e il bench
funziona senza. Chi vuole quella porzione la prende da sé, e resta sua la
responsabilità di rispettarne i termini:

```bash
make cefr-fetch-score      # scarica in tests/cefr/data/ , non versionato
```

### Newsela-Auto

Gli autori non la distribuiscono: richiede l'accesso al dataset Newsela
(<https://newsela.com/data/>) e poi un contatto diretto con loro. Le «17k frasi»
del paper includono quella porzione; senza, le frasi disponibili sono **10.004**.

## Obblighi che questa inclusione comporta

`Wiki-Auto` è **CC BY-SA 3.0**: chi ridistribuisce quei dati, o un'opera da essi
derivata, deve **attribuire** gli autori e mantenere una licenza compatibile.
Questo file è l'attribuzione; i dati stanno in `data/` **immutati**, così che
resti evidente che cosa è loro e che cosa è nostro. Il codice di parrot0 non è
un'opera derivata dal corpus: lo legge, non lo incorpora.

## Formato

TSV, tre colonne, senza intestazione:

```
frase <TAB> voto annotatore A <TAB> voto annotatore B
```

I livelli sono numerici: `1`=A1, `2`=A2, `3`=B1, `4`=B2, `5`=C1, `6`=C2.

## Contatto degli autori

Yuki Arase — `arase [at] c.titech.ac.jp`
