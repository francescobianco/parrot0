● Ecco la tabella dei tempi di make test (binario già compilato, misure reali su una passata):

┌────────────────┬─────────┬──────────────┐
│      Test      │ Secondi │ % del totale │
├────────────────┼─────────┼──────────────┤
│ run            │   34.65 │        67.4% │
├────────────────┼─────────┼──────────────┤
│ knowledge      │    2.80 │         5.4% │
├────────────────┼─────────┼──────────────┤
│ llmscore_world │    2.40 │         4.7% │
├────────────────┼─────────┼──────────────┤
│ profiles       │    2.18 │         4.2% │
├────────────────┼─────────┼──────────────┤
│ check_sort     │    1.51 │         2.9% │
├────────────────┼─────────┼──────────────┤
│ archetype      │    1.27 │         2.5% │
├────────────────┼─────────┼──────────────┤
│ grammar        │    1.10 │         2.1% │
├────────────────┼─────────┼──────────────┤
│ posix          │    0.92 │         1.8% │
├────────────────┼─────────┼──────────────┤
│ experts        │    0.80 │         1.6% │
├────────────────┼─────────┼──────────────┤
│ synth          │    0.69 │         1.3% │
├────────────────┼─────────┼──────────────┤
│ persist        │    0.54 │         1.1% │
├────────────────┼─────────┼──────────────┤
│ skills         │    0.52 │         1.0% │
├────────────────┼─────────┼──────────────┤
│ explain        │    0.41 │         0.8% │
├────────────────┼─────────┼──────────────┤
│ howknow        │    0.35 │         0.7% │
├────────────────┼─────────┼──────────────┤
│ posix_oracle   │    0.34 │         0.7% │
├────────────────┼─────────┼──────────────┤
│ booklearn      │    0.25 │         0.5% │
├────────────────┼─────────┼──────────────┤
│ multigoal      │    0.25 │         0.5% │
├────────────────┼─────────┼──────────────┤
│ anon           │    0.16 │         0.3% │
├────────────────┼─────────┼──────────────┤
│ research_learn │    0.15 │         0.3% │
├────────────────┼─────────┼──────────────┤
│ wiki_learning  │    0.12 │         0.2% │
├────────────────┼─────────┼──────────────┤
│ TOTALE         │ ~51.4 s │         100% │
└────────────────┴─────────┴──────────────┘

Il collo di bottiglia è run.sh

Da solo pesa 34.6 s ≈ 67% di tutto make test. Il motivo è strutturale, non è lento il singolo turno:

- run.sh riproduce 209 file .chat (1320 turni totali);
- per ogni file fa un fork separato del binario (printf ... | bin/parrot0) → 209 avvii di processo;
- ad ogni avvio parrot0 ricarica la KB (kb/core/*.p0 + base) da zero.

Quindi ~166 ms a file, quasi tutti spesi nello startup/caricamento KB ripetuto, non nell'inferenza. I 1320 turni veri e propri sono rapidissimi (vedi archetype: 16 turni in 1.27 s, gran parte dei quali è comunque startup).

Gli altri test sono tutti sotto i 3 s; wiki_learning/research_learn sono velocissimi perché di default sono offline (fetch gated).

Se vuoi ridurre il tempo, le leve sono in ordine di efficacia:
1. Far girare un solo processo per più conversazioni in run.sh (servirebbe un separatore di sessione nel protocollo .chat) — toglierebbe gran parte dei 34 s.
2. Eseguire i .chat in parallelo (es. xargs -P), dato che sono indipendenti — quasi lineare col numero di core.
3. Lasciare tutto com'è: 51 s è il costo di una rete di sicurezza da 1320 turni.

Dimmi se vuoi che implementi la 1 o la 2.
  
