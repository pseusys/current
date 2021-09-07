import sys
sys.path.append('../ML-lib/')

import datalib as data
import drawlib as draw
import matlib as mat
import supplib as supp


if __name__ == '__main__':
    print("Пункт 1:\n")
    frame = data.collect_json_file("data1")
    print(f"Среднее значение x: { mat.mean(frame, 'x') }")
    print(f"Медиана x: { mat.median(frame, 'x') }")
    print(f"Мода x: { mat.mode(frame, 'x') }\n")
    print(f"Дисперсия y: { mat.var(frame, 'x') }\n")
    draw.n_dist(frame, 'x')
    integral = supp.integr(mat.norm_dist(frame, 'x'), 80)
    print(f"Вероятность того, что возраст больше 80, составит: {integral[0]} +/- {integral[1]}\n")
    print(f"Двумерное математическое ожидание: { mat.mean_2d(frame, 'x', 'y') }")
    print(f"Ковариационная матрица:\n{ mat.cov(frame) }\n")
    print(f"Корреляция: { mat.corr(frame) }\n\n")
    draw.scatter(frame, 'x', 'y')

    print("Пункт 2:\n")
    frame = data.collect_json_file("data2")
    print(f"Ковариационная матрица:\n{ mat.cov(frame) }")
    print(f"Обобщенная дисперсия: { supp.det(mat.cov(frame)) }\n\n")

    print("Пункт 3:\n")
    Na = mat.n_dist(1, 4)
    Nb = mat.n_dist(2, 8)
    draw.draw(Na, 0, 15, 0.1)
    draw.draw(Nb, 0, 15, 0.1, True)
    for i in range(5, 8):
        print(f"Значение { i } с большей вероятностью было сгенерировано распределением { 'Na' if max(Na(i), Nb(i)) == Na(i) else 'Nb' }")
    print(f"Значение, которое могло быть сгенерировано обеими распределениями с равное вероятностью: { supp.intersection(Na, Nb, 5.0)[0] }")

    draw.flush()
