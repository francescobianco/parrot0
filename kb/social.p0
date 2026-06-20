% parrot0 social register — conversational markers as knowledge.
% Loaded at brain_create so the self-model can query which markers exist;
% the C code reads these facts at runtime instead of hardcoding word lists.
% Predicates: social_marker/2, social_pattern/2, question_word/1, reaction_word/1.

% --- token-level social markers (whole-word match) ---
social_marker(opening, hello).
social_marker(opening, hi).
social_marker(opening, hey).
social_marker(opening, hiya).
social_marker(opening, yo).
social_marker(opening, salve).
social_marker(opening, ehi).
social_marker(opening, buongiorno).
social_marker(opening, buonasera).
social_marker(opening, hello!).
social_marker(opening, sup).
social_marker(opening, howdy).

social_marker(closing, bye).
social_marker(closing, goodbye).
social_marker(closing, farewell).
social_marker(closing, addio).
social_marker(closing, arrivederci).

social_marker(thanks, thanks).
social_marker(thanks, thx).
social_marker(thanks, ty).
social_marker(thanks, grazie).

social_marker(apology, sorry).
social_marker(apology, scusa).
social_marker(apology, scusate).
social_marker(apology, scusi).
social_marker(apology, dispiace).

social_marker(ambiguous, ciao).

% --- cue-based patterns (substring match in normalized text) ---
social_pattern(opening, good morning).
social_pattern(opening, good evening).
social_pattern(opening, good afternoon).

social_pattern(closing, see you).
social_pattern(closing, a presto).

social_pattern(thanks, thank you).
social_pattern(thanks, thank u).

social_pattern(apology, mi dispiace).

social_pattern(wellbeing, how are you).
social_pattern(wellbeing, how r u).
social_pattern(wellbeing, how do you do).
social_pattern(wellbeing, how is it going).
social_pattern(wellbeing, come stai).
social_pattern(wellbeing, come va).
social_pattern(wellbeing, how is your day).
social_pattern(wellbeing, hows your day).
social_pattern(wellbeing, hows it going).
social_pattern(wellbeing, how are things).
social_pattern(wellbeing, hows things).
% gen124: casual small-talk openers the chatsim transcripts kept walling on —
% phatic contact, not information requests. Patterns are matched against the
% CANONICALIZED text, so "whats up" appears here as its expansion "what is up"
% (gen74 contraction map), while the apostrophe form "what's up" is left intact.
social_pattern(wellbeing, what is up).
social_pattern(wellbeing, what's up).
social_pattern(wellbeing, wassup).
social_pattern(wellbeing, you good).
social_pattern(wellbeing, u good).
social_pattern(wellbeing, you doing ok).
social_pattern(opening, you there).

% --- question words (signal an information request) ---
question_word(who).
question_word(what).
question_word(where).
question_word(when).
question_word(why).
question_word(how).
question_word(which).

question_word(chi).
question_word(che).
question_word(cosa).
question_word(dove).
question_word(quando).
question_word(perche).
question_word(perché).
question_word(come).

% --- reaction words (laughter/conversational fillers) ---
reaction_word(haha).
reaction_word(lol).
reaction_word(hehe).
reaction_word(ahaha).
reaction_word(ahah).
reaction_word(lmao).
reaction_word(lolol).
reaction_word(hehehe).
reaction_word(hahaha).
