% Группа 8304, вариант 1
% Создайте правило с предикатом, имеющим пять аргументов и проверяющим, попадает ли точка, чьи координаты заданы первыми двумя параметрами, в круг, центр которого определяют третий и четвертый параметр, а радиус - пятый.

circle_checker(POINT_X, POINT_Y, CIRCLE_X, CIRCLE_Y, RADIUS) :-
    X is CIRCLE_X - POINT_X,
    ( X < 0 -> X_DIST is X ; X_DIST is -X ),
    Y is CIRCLE_Y - POINT_Y,
    ( Y < 0 -> Y_DIST is Y ; Y_DIST is -Y ),
    RADIUS * RADIUS > X_DIST * X_DIST + Y_DIST * Y_DIST.
