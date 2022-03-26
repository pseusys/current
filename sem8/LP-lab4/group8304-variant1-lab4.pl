% Группа 8304, вариант 1
% Миссионеры и каннибалы

% К берегу реки подошли 3 миссионера и 3 каннибала.
% Как им всем безопасно переправиться на другой берег, используя двухместную лодку, если каннибалы могут съесть миссионеров, оказавшихся в меньшинстве?
% Вывести порядок перемещений.

% Перевозить каннибалов и миссионеров следует по очереди.
% Если на одном берегу окажется хотя бы на два каннибала больше, чем миссионера, кого-то точно съедят.

% Ограничение:
constartint(M, C) :- M > 0, C > M.

% Проверка состояния:
check(LM, LC, RM, RC) :- \+ constartint(LM, LC), \+ constartint(RM, RC).

% Решение (рассчитывается для определенного количества шагов N):
go(N) :- move(3, 3, 0, 0, [], l, N).

write_passengers([F | S]) :-
  F = m, S = [], write('миссионер');
  F = c, S = [], write('каннибал');
  F = m, S = [m], write('два миссионера');
  F = c, S = [c], write('два каннибала');
  F = m, S = [c], write('миссионер и каннибал').

% Вывод одного сообщения из истории перемещений.
write_movement([D, O]) :- D = l, write('на левый берег плывет: '), write_passengers(O), !; D = r, write('на правый берег плывет: '), write_passengers(O).

% Вывод элементов из истории перемещений
write_queue([P | I]) :- I = [], write_movement(P); write_queue(I), write(' -> '), write_movement(P).

% Печать истории перемещений P:
move(0, 0, 3, 3, P, r, _) :- write('Начало на левом берегу; '), write_queue(P), write('; лодка осталась на правом берегу.').

% Функтор move[7] имеет следующую сигнатуру:
% [0] - Количество миссионеров слева.
% [1] - Количество каннибалов слева.
% [2] - Количество миссионеров справа.
% [3] - Количество каннибалов справа.
% [4] - История перемещений в обратном порядке (в формате [[куда плывет лодка], [пассажир, пассажир?]])).
% [5] - Сторона реки, на которой находится лодка (l или r).
% [6] - Количество шагов.
move(LM, LC, RM, RC, P, A, N) :- N > 0, M is N - 1, step(LM, LC, RM, RC, P, A, M).

% Вспомогательный функтор, перемещающий человека, не уменьшая количество шагов.
step(LM, LC, RM, RC, P, A, N) :- A = l, step_left(LM, LC, RM, RC, P, N); A = r, step_right(LM, LC, RM, RC, P, N).

% Всего доступно пять вариантов перемещения с левого берега на правый:
% 1. Едет один миссионер.
% 2. Едет один каннибал.
% 3. Едут два миссионера.
% 4. Едут два каннибала.
% 5. Едет один миссионер и один каннибал.
step_left(LM, LC, RM, RC, P, N) :-
  LM >= 1, NLM is LM - 1, NRM is RM + 1, check(NLM, LC, NRM, RC), move(NLM, LC, NRM, RC, [[r, [m]] | P], r, N);
  LC >= 1, NLC is LC - 1, NRC is RC + 1, check(LM, NLC, RM, NRC), move(LM, NLC, RM, NRC, [[r, [c]] | P], r, N);
  LM >= 2, NLM is LM - 2, NRM is RM + 2, check(NLM, LC, NRM, RC), move(NLM, LC, NRM, RC, [[r, [m, m]] | P], r, N);
  LC >= 2, NLC is LC - 2, NRC is RC + 2, check(LM, NLC, RM, NRC), move(LM, NLC, RM, NRC, [[r, [c, c]] | P], r, N);
  LM >= 1, LC >= 1, NLM is LM - 1, NRM is RM + 1, NLC is LC - 1, NRC is RC + 1, check(NLM, NLC, NRM, NRC), move(NLM, NLC, NRM, NRC, [[r, [m, c]] | P], r, N).

% То же самое - с правого беега на левый:
step_right(LM, LC, RM, RC, P, N) :-
  RM >= 1, NRM is RM - 1, NLM is LM + 1, check(NLM, LC, NRM, RC), move(NLM, LC, NRM, RC, [[l, [m]] | P], l, N);
  RC >= 1, NRC is RC - 1, NLC is LC + 1, check(LM, NLC, RM, NRC), move(LM, NLC, RM, NRC, [[l, [c]] | P], l, N);
  RM >= 2, NRM is RM - 2, NLM is LM + 2, check(NLM, LC, NRM, RC), move(NLM, LC, NRM, RC, [[l, [m, m]] | P], l, N);
  RC >= 2, NRC is RC - 2, NLC is LC + 2, check(LM, NLC, RM, NRC), move(LM, NLC, RM, NRC, [[l, [c, c]] | P], l, N);
  RM >= 1, RC >= 1, NRM is RM - 1, NLM is LM + 1, NRC is RC - 1, NLC is LC + 1, check(NLM, NLC, NRM, NRC), move(NLM, NLC, NRM, NRC, [[l, [m, c]] | P], l, N).
