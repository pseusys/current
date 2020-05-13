#include <iostream>
#include "oop3.hpp"

using namespace stepik;

int main() {
    Square sq(Coords(100, 180), Coords(180, 180), Color(122, 34, 96));
    std::cout << sq << std::endl << std::endl;
    sq.draw("Square.bmp");

    Parallelogram par(Coords(100, 180), Coords (120, 160), Coords(180, 180), Color(43, 194, 12));
    std::cout << par << std::endl << std::endl;
    par.draw("Parallelogram.bmp");

    Rhombus rho(Coords(100, 180), Coords(180, 180), 30 * M_PI / 180, Color(198, 216, 157));
    std::cout << rho << std::endl;
    rho.draw("Rhombus.bmp");
}
