REFERENCE PROBE — la MOSSA sul riferimento, non la correttezza

### ordine_introduzione
  osservo: L'ordine e' quello di INTRODUZIONE o quello di menzione nella frase?
  > Ho messo il libro rosso sul tavolo.
    parrot0 < Non capisco ancora.
    oracolo < Ok, e il libro era già rosso prima o l'hai dipinto? 📚
  > Il quaderno blu invece è nello zaino.
    parrot0 < Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?
    oracolo < Capito, hai un libro rosso sul tavolo e un quaderno blu nello zaino. Tutto ordinato! 😄
  > Dov'è il primo?
    parrot0 < Non capisco ancora.
    oracolo < Il libro rosso è sul tavolo, come hai detto prima. 📖

### ellissi
  osservo: Il turno ellittico: riprende il verbo dal turno prima?
  > Il libro rosso è sul tavolo.
    parrot0 < Learned: located_in(book_red, tavolo).
    oracolo < Sì, il libro rosso è sul tavolo. Vuoi che ti aiuti con qualcosa riguardo a questa frase?
  > Il quaderno blu è sulla mensola.
    parrot0 < Learned: located_in(quaderno_blue, mensola).
    oracolo < Sì, e il quaderno blu è sulla mensola. Stai descrivendo degli oggetti? 😊
  > Dov'è il primo?
    parrot0 < tavolo.
    oracolo < È sul tavolo, come hai detto prima! 😊
  > E il secondo?
    parrot0 < Non capisco ancora.
    oracolo < È sulla mensola! 📚

### ambiguita_referenziale
  osservo: LA SONDA: due candidati. Sceglie? chiede? elenca? con quale frase?
  > Il libro rosso è sul tavolo.
    parrot0 < Learned: located_in(book_red, tavolo).
    oracolo < Bello! Vuoi parlare del libro rosso o posso aiutarti con qualcos'altro?
  > Il libro blu è sulla mensola.
    parrot0 < Learned: located_in(book_blue, mensola).
    oracolo < Interessante! Hai molti libri. Il rosso è sul tavolo, il blu sulla mensola. C'è qualcosa che vuoi sapere?
  > Dove si trova il libro?
    parrot0 < Quale? book red, book blue
    oracolo < Ci sono due libri: - Il libro **rosso** è sul tavolo - Il libro **blu** è sulla mensola Quale intendi?

### proprieta_basta
  osservo: La proprieta' da sola individua? e la forma ellittica passa?
  > Il libro rosso è sul tavolo.
    parrot0 < Learned: located_in(book_red, tavolo).
    oracolo < Sì, lo vedo! È un bel libro rosso. Ti piace leggere?
  > Il libro blu è sulla mensola.
    parrot0 < Learned: located_in(book_blue, mensola).
    oracolo < Bello! Hai una bella collezione di libri.rosse e blu. Ce ne sono altri?
  > E quello rosso?
    parrot0 < Non capisco ancora.
    oracolo < Il libro rosso è sul tavolo, come hai detto prima. È ancora lì!

### correzione_aggiorna
  osservo: La correzione SOSTITUISCE il fatto o ne aggiunge un secondo?
  > Il libro rosso è sul tavolo.
    parrot0 < Learned: located_in(book_red, tavolo).
    oracolo < Sì, il libro rosso è sul tavolo. C'è qualcos'altro che vorresti sapere? 😊
  > Correzione: quello rosso l'ho spostato sulla mensola.
    parrot0 < Non capisco ancora.
    oracolo < Ah, capito! Ora è sulla mensola. Grazie per la correzione! 📚
  > Dove si trova adesso?
    parrot0 < Non sono sicuro di aver seguito. Puoi dirlo in un altro modo?
    oracolo < È sulla mensola, come hai detto tu! 🙂

### riferimento_vuoto
  osservo: CONTROLLO NEGATIVO: nessun referente introdotto. Dichiara il vuoto?
  > Dov'è il primo?
    parrot0 < Non capisco ancora.
    oracolo < Ciao! Potresti darmi un po' più di contesto? Non sono sicuro a cosa ti riferisci con "il primo". 😊

