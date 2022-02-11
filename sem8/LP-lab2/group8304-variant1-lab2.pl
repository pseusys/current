% Группа 8304, вариант 1
% Найти минимальный элемент в заданном списке.


min_elem([ITEM|ARRAY], MIN) :-
    nonvar(ARRAY), % if ARRAY is not initialized we can not perform minimum search in it.
    ARRAY = [], % execute if ARRAY length is equal 1 only.
    MIN is ITEM. % set minimum value to the only array element.

min_elem([ITEM|ARRAY], MIN) :- % separate first ITEM from other ARRAY.
    nonvar(ARRAY), % if ARRAY is not initialized we can not perform minimum search in it.
    ARRAY \= [], % execute if ARRAY length is more than 1 only.
    min_elem(ARRAY, SUBMIN), % search for minimum in the tail ARRAY.
    ( ITEM < SUBMIN -> MIN is ITEM ; MIN is SUBMIN ). % if SUBMIN (minimum of the tail ARRAY) is less than ITEM, ITEM is array minimum at this point (SUBMIN otherwise).

min_elem(ARRAY, _) :-
    var(ARRAY), % if ARRAY is not initialized print a warning message to avoid exception.
    write('ARRAY (first parameter) should be initialized in order to perform search.').


% Tests:
% ?- min_elem([0, 9, 1, 8, 2, 7, 3, 6, 4, 5], 0). -> true
% ?- min_elem([4], 5). -> no
% ?- min_elem([7], 7). -> true
% ?- min_elem([], 7). -> no
% ?- min_elem([0, 9, 1, 8, 2, 7, 3, 6, 4, 5], X). -> X=0
% ?- min_elem([4], X). -> X=4
% ?- min_elem([], X). -> no
% ?- min_elem(A, 3). -> ARRAY (first parameter) should be initialized in order to perform search.
% ?- min_elem(A, X). -> ARRAY (first parameter) should be initialized in order to perform search.
