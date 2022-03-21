% Группа 8304, вариант 1.
% Реализуйте, используя рекурсию и отсечение, цикл с постусловием (типа repeat <оператор> until <условие>).

while_do(_).
while_do(OPERATOR, CONDITION) :- while_do(OPERATOR), CONDITION, !; while_do(OPERATOR, CONDITION).
