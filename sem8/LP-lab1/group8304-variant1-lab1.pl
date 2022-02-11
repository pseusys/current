% Группа 8304, вариант 1
% Дядя, невестка (жена сына или жена брата).

% Family information:
% sofi         tom   ann
%    \         / \   /
%     liza,jake   bob        angela
%                   \        /
%                    mary,luk    judith
%                           \    /
%                            kate
male(tom).
male(bob).
male(luk).
male(jake).
female(kate).
female(liza).
female(mary).
female(angela).
female(sofi).
female(judith).
parent(tom, bob).
parent(tom, jake).
parent(tom, liza).
parent(ann, bob).
parent(sofi, jake).
parent(sofi, liza).
parent(bob, mary).
parent(angela, mary).
parent(bob, luk).
parent(luk, kate).
parent(judith, kate).

% Brother or sister (has a parent in common)
geschwister(X, Y) :- parent(F, X), parent(F, Y), X \= Y.

% Partner (has children with)
partner(X, Y) :- parent(X, F), parent(Y, F), X \= Y.


% Uncle (is a brother of mother or father)
uncle(NAME, TARGET) :- male(NAME), parent(PARENT, TARGET), geschwister(NAME, PARENT).

% Daughter in law (is a wife of son or brother)
daughterinlaw(NAME, TARGET) :- parent(TARGET, SON), male(SON), partner(NAME, SON), female(NAME).
daughterinlaw(NAME, TARGET) :- geschwister(TARGET, BRO), male(BRO), partner(NAME, BRO), female(NAME).


% Tests:

% ?- uncle(jake, mary). -> true
% ?- uncle(X, luk). -> X=jake
% ?- uncle(jake, Y). -> Y=mary
% ?- uncle(X, Y). -> X=jake,Y=mary; X=jake,Y=luk

% ?- daughterinlaw(angela, tom). -> true
% ?- daughterinlaw(X, liza). -> X=angela
% ?- daughterinlaw(judith, Y). -> Y=bob; Y=mary
% ?- daughterinlaw(X, Y). -> X=angela,Y=tom; X=angela,Y=ann; X=angela,Y=liza; X=angela,Y=jake; X=judith,Y=bob; X=judith,Y=mary

