% Группа 8304, вариант 1.
% Реализуйте, используя рекурсию и отсечение, цикл с постусловием (типа repeat <оператор> until <условие>).

repeat_until(OPERATOR) :- OPERATOR, !; true.
repeat_until(OPERATOR, CONDITION) :- repeat_until(OPERATOR), CONDITION, !; repeat_until(OPERATOR, CONDITION).

% Для тестирования рекомендуется использовать предикаты:

test_operator :- random(COUNTER), tell('condition.txt'), write(COUNTER), write('.\n'), told.
test_condition(MATCH) :- see('condition.txt'), read(COUNTER), seen, write('counter = '), write(COUNTER), write('\n'), COUNTER > MATCH.

% При их использовании, вызов цикла repeat until будет выглядеть следующим образом:
% ?- repeat_until(test_operator, test_condition(0.5)).
