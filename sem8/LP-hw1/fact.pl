fs(1, _, 1).
fs(N, D, R) :- N > 1, NN is N // D, ND is D + 1, fs(NN, ND, _), R is ND.

fact(N, V) :- var(N), var(V), write('Either N (first argument) or V (second argument) should be instantiated').
fact(0, 1).
fact(N, V) :- nonvar(N), N > 0, N1 is N - 1, fact(N1, V1), V is V1 * N.
fact(N, V) :- nonvar(V), V > 1, fs(V, 2, N).
