//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_COORDS_H
#define LOGIC_COORDS_H


class coords {
private:
    int x, y;
public:
    coords(int x, int y);

    int get_x();

    void set_x(int x);

    int get_y();

    void set_y(int y);

    bool operator==(const coords &rhs);

    bool operator!=(coords &rhs);
};


#endif //LOGIC_COORDS_H
