#ifndef LABA2_OOP3_HPP
#define LABA2_OOP3_HPP

#include <cmath>

// Remove for no drawing:
#include "CImg.h"

// Вариант №3: квадрат, параллелограмм, ромб.
// Для рисования применялась библиотека: https://github.com/dtschump/CImg

namespace stepik {
    typedef long double number;
    typedef unsigned char light;

    class Coords {
    public:
        Coords(number x, number y) : x(x), y(y) {}

        Coords() : Coords(0, 0) {}

        number& X() {
            return x;
        }

        number& Y() {
            return y;
        }

        number X() const {
            return x;
        }

        number Y() const {
            return y;
        }

        Coords abs() {
            this->x = std::abs(this->x);
            this->y = std::abs(this->y);
            return *this;
        }

        number dist(Coords other) const {
            return std::sqrt(std::pow((other.x - this->x), 2) + std::pow((other.y - this->y), 2));
        }

        Coords operator+(const Coords &other) const {
            return Coords{this->x + other.x, this->y + other.y};
        }

        Coords operator-(const Coords &other) const {
            return Coords{this->x - other.x, this->y - other.y};
        }

        Coords operator*(const long double number) const {
            return Coords{this->x * number, this->y * number};
        }

        Coords operator/(const long double number) const {
            return Coords{this->x / number, this->y / number};
        }

        Coords &operator+=(const Coords &other) {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Coords &operator-=(const Coords &other) {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        }

        Coords &operator*=(const number number) {
            this->x *= number;
            this->y *= number;
            return *this;
        }

        Coords &operator/=(const number number) {
            this->x /= number;
            this->y /= number;
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &out, const Coords &c) {
            out << "{" << c.x << ", " << c.y << "}";
            return out;
        }

    private:
        number x;
        number y;
    };



    class Color {
    public:
        Color(light r, light g, light b) : red(r), green(g), blue(b) {}

        Color() : Color(255, 255, 255) {}

        light& Red() {
            return red;
        }
        light& Green() {
            return green;
        }
        light& Blue() {
            return blue;
        }

        light Red() const {
            return red;
        }
        light Green() const {
            return green;
        }
        light Blue() const {
            return blue;
        }

        friend std::ostream &operator<<(std::ostream &out, const Color &c) {
            return out << "{R: " << (int) c.red << ", G: " << (int) c.green << ", B: " << (int) c.blue << "}";
        }

    private:
        light red;
        light green;
        light blue;
    };





    class Shape {
    public:
        // CONSTRUCTOR:
        explicit Shape(Color new_filling = Color()) : filling(new_filling) {}


        // SET METHODS:
        // Movement:
        virtual void move_by(Coords coord_delta) = 0;

        virtual void move_to(Coords new_center) {
            move_by(new_center - get_center());
        };

        // Scaling:
        virtual void scale_by(number modifier) = 0;

        void scale_by_percent(number percent) {
            scale_by(percent / 100);
        };

        // Rotation:
        void rotate_by_deg(number rotation_delta) {
            rotate_by_rad(deg_to_rad(rotation_delta));
        }

        void rotate_by_rad(number rotation_delta) {
            correct_rotation(fmod(rotation_delta, 2 * M_PI));
        }

        // Color:
        void set_color(Color new_filling) {
            this->filling = new_filling;
        }


        // GET METHODS:
        // Position:
        virtual Coords get_center() const = 0;

        //Color:
        Color get_color() const {
            return this->filling;
        }


        // STREAM OUTPUT:
        friend std::ostream &operator<<(std::ostream &out, const Shape &c) {
            return c.print(out);
        }

        // DRAW METHODS:
        virtual void draw(std::string file_name) const = 0;


    private:
        Color filling;

        virtual void correct_rotation(number rotation_delta) = 0;

    protected:
        virtual std::ostream& print(std::ostream &out) const {
            return out << "Shape: {\n     Filled with color: " << this->filling << "\n}";
        }

        static number rad_to_deg(number radians) {
            return radians * (180.0 / M_PI);
        }

        static number deg_to_rad(number degrees) {
            return degrees * (M_PI / 180.0);
        }
    };



    class FourAngle : public Shape {
    public:
        // CONSTRUCTORS:
        FourAngle(Coords uL, Coords uR, Coords lR, Coords lL, Color col = Color()) : Shape(col), upperLeft(uL), upperRight(uR), lowerRight(lR), lowerLeft(lL) {}
        FourAngle() : FourAngle(Coords(-1, 1), Coords(1, 1), Coords(1, -1), Coords(-1, -1)) {}


        // SET METHODS:
        // Movement:
        void move_by(Coords rotation_delta) override {
            upperLeft += rotation_delta;
            upperRight += rotation_delta;
            lowerRight += rotation_delta;
            lowerLeft += rotation_delta;
        }

        // Scaling:
        void scale_by(number modifier) override {
            upperLeft *= modifier;
            upperRight *= modifier;
            lowerRight *= modifier;
            lowerLeft *= modifier;
        }


        // GET METHODS:
        // Position:
        Coords get_center() const override {
            Coords upperCenter(upperLeft - upperRight);
            Coords lowerCenter(lowerLeft - lowerRight);
            return upperCenter.abs() - lowerCenter.abs();
        }

        // DRAW METHODS:
        void draw(std::string file_name) const override {
            cimg_library::CImg<float> img(800, 600, 1, 3);
            light color[] = {this->get_color().Red(), this->get_color().Green(), this->get_color().Blue()};
            cimg_library::CImg<int> points(4, 2);
            int thePoints[] = {static_cast<int>(upperLeft.X()), static_cast<int>(upperLeft.Y()),
                               static_cast<int>(upperRight.X()), static_cast<int>(upperRight.Y()),
                               static_cast<int>(lowerRight.X()), static_cast<int>(lowerRight.Y()),
                               static_cast<int>(lowerLeft.X()), static_cast<int>(lowerLeft.Y())};
            int *iterator = thePoints;
            cimg_forX(points, i) {
                points(i, 0) = *(iterator++);
                points(i, 1) = *(iterator++);
            }
            img.draw_polygon(points, color).save(file_name.c_str());
        }


    private:
        Coords upperLeft, upperRight, lowerRight, lowerLeft;

        void correct_rotation(number rotation_delta) override {
            Coords center = get_center();
            upperLeft = rotate_point(upperLeft, rotation_delta, center);
            upperRight = rotate_point(upperRight, rotation_delta, center);
            lowerRight = rotate_point(lowerRight, rotation_delta, center);
            lowerLeft = rotate_point(lowerLeft, rotation_delta, center);
        }

        static Coords rotate_point(Coords point, number angle, Coords center) {
            number s = std::sin(angle);
            number c = std::cos(angle);

            center -= point;

            number new_x = center.X() * c - center.Y() * s;
            number new_y = center.X() * s + center.Y() * c;

            center.X() = new_x + point.X();
            center.Y() = new_y + point.Y();
            return center;
        }

    protected:
        std::ostream &print(std::ostream &out) const override {
            return Shape::print(out) << " <- FourAngle: {\n     Angles: " << upperLeft << ", " << upperRight << ", "
                                     << lowerRight << ", " << lowerLeft << "\n}";
        }
    };





    class Parallelogram : public FourAngle {
    public:
        // CONSTRUCTORS:
        /**
         *    U--------------#
         *   /              /
         *  /              /
         * A--------------L
         */
        Parallelogram(Coords angler, Coords upper, Coords lower, Color new_filling = Color()) : FourAngle(upper, upper + (angler - lower).abs(), lower, angler, new_filling) {}

        /**
         *    #---------------#
         *   /               / <- L
         *  / A             /
         * L---------------R
         */
        Parallelogram(Coords left, Coords right, number left_angle, number length, Color new_filling = Color()) :
                Parallelogram(left, Coords(left.X() + std::cos(left_angle) * length, left.Y() + std::sin(left_angle) * length), right, new_filling) {}
        Parallelogram() : Parallelogram(Coords(-1, -1), Coords(1, -1), Shape::deg_to_rad(90), 1) {}

    protected:
        std::ostream &print(std::ostream &out) const override {
            return FourAngle::print(out) << " <- Parallelogram";
        }
    };



    class Rhombus : public Parallelogram {
    public:
        // CONSTRUCTORS:
        /**
         *    #-------#
         *   /       /
         *  / A     /
         * O-------T
         */
        Rhombus(Coords one, Coords two, number angle, Color new_filling = Color()) :
                Parallelogram(one, Coords(one.X() + one.dist(two) * std::cos(angle), one.Y() + one.dist(two) * std::sin(angle)), two,
                              new_filling) {}
        Rhombus() : Rhombus(Coords(-1, -1), Coords(1, -1), Shape::deg_to_rad(90)) {}

    protected:
        std::ostream &print(std::ostream &out) const override {
            return Parallelogram::print(out) << " <- Rhombus";
        }
    };



    class Square : public Rhombus {
    public:
        // CONSTRUCTORS:
        /**
         * #-------#
         * |       |
         * |       |
         * O-------T
         */
        Square(Coords one, Coords two, Color new_filling = Color()) :
                Rhombus(one, two, Shape::deg_to_rad(90), new_filling) {}
        Square() : Square(Coords(-1, -1), Coords(1, -1)) {}

    protected:
        std::ostream &print(std::ostream &out) const override {
            return Rhombus::print(out) << " <- Square";
        }
    };

}

#endif //LABA2_OOP3_HPP
