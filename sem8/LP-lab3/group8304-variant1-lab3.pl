% Группа 8304, вариант 1
% Задание 1. Определить, является ли заданный список упорядоченным по возрастанию или убыванию.


is_ordered_list([]) :- 
    write('The list is not ordered in ascending/descending order.'), !. % Если список пуст, то не считаем его упорядоченным

is_ordered_list([_]) :- 
    write('The list is not ordered in ascending/descending order.'), !. % Если список состоит из одного элемента, то не считаем его упорядоченным

is_ordered_list([FIRST, SECOND|TAIL]) :- % Если первый элемент списка меньше второго
    FIRST < SECOND, is_ordered_ascending([SECOND|TAIL]), write('The list is ordered in ascending order.'), ! % И все последующие элементы расположены по возрастанию, то список упорядочен по возрастанию
    ; FIRST < SECOND, write('The list is not ordered in ascending/descending order.'), !. % Иначе список никак не упорядочен

is_ordered_list([FIRST, SECOND|TAIL]) :- % Если первый элемент списка больше второго
    FIRST > SECOND, is_ordered_descending([SECOND|TAIL]), write('The list is ordered in descending order..'), ! % И все последующие элементы расположены по убыванию, то список упорядочен по убыванию
    ;FIRST > SECOND, write('The list is not ordered in ascending/descending order.'), !. % Иначе список никак не упорядочен

is_ordered_list([FIRST, SECOND|TAIL]) :- % Если первый элемент списка равен второму
    FIRST = SECOND, is_ordered_list([SECOND|TAIL]), !. % То рассматриваем список дальше

is_ordered_ascending([_]). % Если при проверке на возрастание в списке остался один элемент, то он больше либо равен предыдущему исходя из предыдущего сравнения.

is_ordered_ascending([FIRST, SECOND|TAIL]) :- % Для успешной проверки на возрастание необходимо, чтобы каждый элемент был больше либо равен предыдущему.
    FIRST =< SECOND, is_ordered_ascending([SECOND|TAIL]), !.

is_ordered_descending([_]). % Если при проверке на убывание в списке остался один элемент, то он меньше либо равен предыдущему исходя из предыдущего сравнения.

is_ordered_descending([FIRST, SECOND|TAIL]) :- % Для успешной проверки на убывание необходимо, чтобы каждый элемент был меньше либо равен предыдущему.
    FIRST >= SECOND, is_ordered_descending([SECOND|TAIL]), !.

% Задание 2. Создайте предикат, проверяющий, что дерево является двоичным справочником.

is_ordered_tree(X) :- % Если задан только корень в виде числа, то дерево является двоичным справочником
    number(X), !.

is_ordered_tree(tr(_, nil, nil)) :- % Если нет поддеревьев, то дерево является двоичным справочником
    !.

is_ordered_tree(tr(ROOT, LEFT, nil)):- % Если есть только левое поддерево, то рассматриваем его
    LEFT\=nil, compare_roots_L(ROOT, LEFT), is_ordered_tree(LEFT), !.

is_ordered_tree(tr(ROOT, nil, RIGHT)):-
    RIGHT\=nil, compare_roots_R(ROOT, RIGHT), is_ordered_tree(RIGHT), !.

is_ordered_tree(tr(ROOT, LEFT, RIGHT)) :-
    RIGHT\=nil, LEFT\=nil, compare_roots_L(ROOT, LEFT), is_ordered_tree(LEFT), compare_roots_R(ROOT, RIGHT), is_ordered_tree(RIGHT), !.


compare_roots_L(_, nil) :- !. % Если слева nil, то проверка пройдена.

compare_roots_L(X, Y) :- % Если слева число Y, то оно должно быть меньше корня дерева X.
    number(Y), X >= Y, !.

compare_roots_L(X, tr(ROOT, LEFT, RIGHT)) :- % Все значения левого поддерева должны быть меньше корня, чтобы признать дерево двоичным справочником.
    X >= ROOT, compare_roots_L(X, LEFT), compare_roots_L(X, RIGHT), !.


compare_roots_R(_, nil) :- !. % Если справа nil, то проверка пройдена.

compare_roots_R(X, Y) :- % Если справа число Y, то оно должно быть больше корня дерева X.
    number(Y), X < Y, !. 

compare_roots_R(X, tr(ROOT, LEFT, RIGHT)) :- % Все значения правого поддерева должны быть больше корня, чтобы признать дерево двоичным справочником.
    X<ROOT, compare_roots_R(X, LEFT), compare_roots_R(X, RIGHT), !.	 
