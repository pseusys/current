#ifndef LOGIC_COORDS_HPP
#define LOGIC_COORDS_HPP

#include "serializable.hpp"


class coords : public serializable {
private:
    int x, y;
public:
    coords() = default;
    coords(int x, int y);
    explicit coords(json &package);
    std::shared_ptr<json> pack(int serializer) override;


    int get_x();
    void set_x(int x);

    int get_y();
    void set_y(int y);

    bool operator==(const coords &rhs);
    bool operator!=(coords &rhs);

    void reset();
    bool is_null();
};


#endif //LOGIC_COORDS_HPP
