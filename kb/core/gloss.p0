% parrot0 bilingual content lexicon (gen126, L5 grounded translation).
% Loaded as base knowledge. mod_translate composes a clause translation from
% these glosses + a structural article rule in C — it is NOT a phrasebook of
% whole sentences, so any noun/verb here transfers to a held-out clause.
%
% tr(English, Italian) — content words, looked up in BOTH directions.
% gender(ItalianNoun, m|f) — picks the Italian article (il/la, un/una).

% nouns
tr(dog, cane).
tr(cat, gatto).
tr(house, casa).
tr(book, libro).
tr(water, acqua).
tr(child, bambino).
tr(woman, donna).
tr(man, uomo).
tr(door, porta).
tr(friend, amico).

gender(cane, m).
gender(gatto, m).
gender(casa, f).
gender(libro, m).
gender(acqua, f).
gender(bambino, m).
gender(donna, f).
gender(uomo, m).
gender(porta, f).
gender(amico, m).

% verbs (3rd person singular present — keyed on the English surface form)
tr(runs, corre).
tr(sleeps, dorme).
tr(eats, mangia).
tr(drinks, beve).
tr(reads, legge).
tr(sees, vede).
tr(opens, apre).
tr(arrives, arriva).

% adjectives (masculine singular base form; agreement handled in C)
tr(big, grande).
tr(small, piccolo).
tr(white, bianco).
tr(black, nero).