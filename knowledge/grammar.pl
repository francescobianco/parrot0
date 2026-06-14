% parrot0 grammar expertise — v0: parts of speech.
% The first forged expert (DESIGN.md D5). Load it as a knowledge layer:
%   PARROT0_BASE=knowledge/grammar.pl make chat
% Exports the predicates: noun/1, verb/1, adjective/1, word/1.

% --- parts of speech (facts) ---
noun(dog).
noun(cat).
noun(language).
verb(run).
verb(speak).
adjective(red).
adjective(quick).

% --- category rules: every part of speech is a word (disjunction: many
%     clauses for one head) ---
word(X) :- noun(X).
word(X) :- verb(X).
word(X) :- adjective(X).
