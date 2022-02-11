% Группа 8304, вариант 1
% Создайте предикат, вычисляющий неотрицательную степень целого числа.

pow(NUMBER, 1, RESULT) :- nonvar(NUMBER), RESULT is NUMBER.
pow(NUMBER, POWER, RESULT) :- nonvar(NUMBER), nonvar(POWER), POWER > 1, LESS is POWER - 1, pow(NUMBER, LESS, INTERRESULT), RESULT is INTERRESULT * NUMBER.
pow(NUMBER, _, _) :- var(NUMBER), write('NUMBER (first parameter) should be initialized.').
pow(_, POWER, _) :- var(POWER), write('POWER (second parameter) should be initialized.').
