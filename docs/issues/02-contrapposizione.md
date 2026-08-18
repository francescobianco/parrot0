# 02 — La contrapposizione

> **Prompt (#8 dei cento).**
> `What is the contrapositive of if it rains then the ground is wet?`
>
> **Esito, prima:** «Hmm, I don't know about **contrapositive** yet.»
> **Esito, dopo gen415:** «That looks like a **logic** problem, and I cannot solve it yet.»
> **Risposta giusta:** «Se il terreno non è bagnato, allora non è piovuto.»

## La traccia

| pezzo | prova | esito |
|---|---|---|
| il condizionale, forma classe | `if someone is a doctor then they are a scientist` | `Learned rule: scientist($V1) :- doctor($V1)` ✔ |
| il condizionale, forma proposizionale | `if it rains then the ground is wet` | **funziona da gen413** → `holds(ground_is_wet) :- holds(it_rains)` ✔ |
| il fatto | `the ground is wet` | `Learned: holds(ground_is_wet)` ✔ |
| la domanda polare | `is the ground wet` | **Yes.** ✔ |
| la trasformazione | `what is the contrapositive of…` | ✘ |

Anche qui il grosso c'è: la regola **entra** e il ragionatore la usa. Manca
esattamente una cosa.

## Perché non viene processato

1. **La negazione di una proposizione non esiste.** Il termine è `holds(X)`;
   servirebbe il suo contrario. Il motore ha `naf/1` (negazione per fallimento),
   che è **un'altra cosa**: «non risulta» non è «è falso», e una contrapposizione
   costruita su `naf` sarebbe logicamente sbagliata.
2. **La contrapposizione è una trasformazione su una REGOLA, non su un fatto.**
   Tutte le procedure di `procedures.p0` trasformano *valori*; questa prende una
   clausola e ne restituisce un'altra. È il primo caso in cui parrot0 dovrebbe
   ragionare **sulle proprie regole come dati**.
3. **La domanda chiede un ARTEFATTO, non un valore di verità.** «Qual è la
   contrapposta» non è «è vero che»: la risposta è una frase da comporre, e serve
   il percorso inverso del transcoder — da termine a lingua.

## Cosa manca

- un termine per la proposizione negata (`holds_not/1` o `neg(P)`), distinto da
  `naf`, e le due regole che dicono come si comporta;
- la trasformazione come procedura: `contrapositive(rule(P, Q), rule(neg(Q), neg(P)))`;
- **il transcoder inverso** — da `holds(ground_is_wet)` alla frase. Oggi il
  transcoder è a senso unico (gen419), e questo è il primo caso che chiede
  l'altro verso.

Nessuno dei tre è motore: sono un termine, una regola e una tabella di resa.

## Dove sta l'autocorrezione

Il ciclo vede la lacuna e ne conosce il registro (`logic`, gen415). Il rimedio
sarebbe una **procedura**, che non è fra i rimedi dichiarati:

```prolog
remedy_for(reachability, procedure).   % ← manca, e dietro serve saper
                                       %    proporre una regola, non una cue
```

Ed è il caso in cui la diagonale di `autocorrezione.md` §4c morde: una cue è una
sottostringa del turno e si indovina; **una regola no**. Va indotta da esempi, e
qui l'esempio sarebbe la coppia (regola, sua contrapposta) — cioè esattamente il
materiale che un umano userebbe per insegnarla.
