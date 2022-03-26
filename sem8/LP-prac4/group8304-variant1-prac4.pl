:- initialization(clear).
:- initialization(init).

% Группа 8304, вариант 1.
% Электронный терапевт.
% Имеется список болезней и характерных для них симптомов (в виде фактов базы знаний) (например, "грипп": кашель, высокая температура, головная боль).
% Требуется по введенным пользователем симптомам определить, чем он заболел (м.б. варианты).

memb(ELEM, [ELEM | _]) :- !.
memb(ELEM, [_ | TAIL]) :- TAIL \= [], member(ELEM, TAIL).

all_in([HEAD | TAIL], OUTER) :- memb(HEAD, OUTER), (TAIL = []; all_in(TAIL, OUTER)).

repeat_until(OPERATOR) :- OPERATOR, !; true.
repeat_until(OPERATOR, CONDITION) :- repeat_until(OPERATOR), CONDITION, !; repeat_until(OPERATOR, CONDITION).


init :- assertz(diseases([])), assertz(symptoms([])), assertz(doctor([])).
clear :- retract(diseases(_)), retract(symptoms(_)), retract(doctor(_)); true.


print_diseases :- diseases(D), write('Болезни в базе знаний: '), write(D).
print_symptoms :- symptoms(S), write('Симптомы в базе знаний: '), write(S).


process_line([[DISEASE, SYMPTOMS] | LINES], REQUESTED) :- all_in(REQUESTED, SYMPTOMS), write('Это может быть: '), write(DISEASE), write('\n'); (LINES = [], fail; process_line(LINES, REQUESTED)).
predict(SYMPTOMS) :- doctor(LINES), process_line(LINES, SYMPTOMS).


save_symptoms([HEAD | TAIL]) :- symptoms(SYMPTOMS), (memb(HEAD, SYMPTOMS); retract(symptoms(SYMPTOMS)), assertz(symptoms([HEAD | SYMPTOMS]))), (TAIL = []; save_symptoms(TAIL)).
save_disease([DISEASE, SYMPTOMS]) :- retract(diseases(DISEASES)), assertz(diseases([DISEASE | DISEASES])), save_symptoms(SYMPTOMS).
save_line(LINE) :- retract(doctor(LINES)), assertz(doctor([LINE | LINES])), save_disease(LINE).

read_line(LINE) :- read(LINE), LINE \= end_of_file, save_line(LINE).
read_db(FILE) :- see(FILE), repeat_until(read_line(LINE), LINE = end_of_file), seen.


% При тестировании на устройстве, поддерживающем кириллический ввод, можно использовать:
% ?- read_db('database.txt').
% При тестировании на устройстве, не поддерживающем кириллический ввод, следует использовать:
% ?- read_db('database-en.txt').

% Далее можно посмотреть список болезней:
% ?- print_diseases.
% или список симптомов:
% ?- print_symptoms.

% Для предсказания болезни по симптомам следует использовать:
% ?- predict(['symptom 1', 'symptom 2', 'symptom 3']).

% При чтении из 'database-en.txt' также можно получить боступ к списку болезней, симптомов или к базе знаний напрямую.
% Для этого следует использовать предикаты diseases/1, symptoms/1 и doctor/1 соответственно.
% При чтении из 'database.txt' вывод этих предикатов напрямую нечитабелен (из-за ограничений GNU Prolog, связанных с кодировкой внутренней DB).
