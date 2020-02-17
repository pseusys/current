#ifndef LOGIC_ARRAY_HPP
#define LOGIC_ARRAY_HPP

#include <memory>
#include "../other/log.hpp"
#include "serializable.hpp"
#include "coords.hpp"

template <class T>
class double_array : public serializable {
private:
    int width, height;
    std::shared_ptr<std::shared_ptr<T> []> arr = nullptr;

public:
    explicit double_array(coords& capacity);
    explicit double_array(json& package);
    std::shared_ptr<json> pack(int serializer) override;

    T& operator[](coords& index);
    T& operator[](std::shared_ptr<coords> index);
    void clear();

    int size();
    bool empty();
};



template <class T>
double_array<T>::double_array(coords& capacity) {
    static_assert(std::is_convertible<T*, serializable*>::value, "double_array class may only contain serializable objects");
    width = capacity.get_x();
    height = capacity.get_y();
    arr = std::shared_ptr<std::shared_ptr<T> []>(new std::shared_ptr<T> [width * height]);
    for (int i = 0; i < width * height; ++i) {
        arr[i] = std::shared_ptr<T>(new T());
    }
}

template <class T>
double_array<T>::double_array(json& package) : serializable(package) {
    static_assert(std::is_convertible<T*, serializable*>::value, "double_array class may only contain serializable objects");
    width = package["width"];
    height = package["height"];
    arr = std::shared_ptr<std::shared_ptr<T> []>(new std::shared_ptr<T> [width * height]);
    for (int i = 0; i < height; ++i) {
        json array = package["data"].back();
        for (int j = 0; j < width; ++j) {
            arr[i * width + j] = std::make_shared<T>(array.back());
        }
    }
}

template <class T>
std::shared_ptr<json> double_array<T>::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["width"] = width;
    (*package)["height"] = height;
    (*package)["data"] = json::array();
    for (int i = 0; i < height; ++i) {
        json array = json::array();
        for (int j = 0; j < width; ++j) {
            array.push_back(*(arr[i * width + j]->pack(serializer)));
        }
        (*package)["data"].push_back(array);
    }
    return package;
}



template <class T>
T& double_array<T>::operator[](coords& index) {
    if (((index.get_x() >= 0) && (index.get_x() < width)) || ((index.get_y() >= 0) && (index.get_y() < height))) {
        return *(arr[index.get_y() * height + index.get_x()]);
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}

template <class T>
T& double_array<T>::operator[](std::shared_ptr<coords> index) {
    if (((index->get_x() >= 0) && (index->get_x() < width)) || ((index->get_y() >= 0) && (index->get_y() < height))) {
        return *(arr[index->get_y() * height + index->get_x()]);
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}

template <class T>
void double_array<T>::clear() {
    for (int i = 0; i < this->length; ++i) {
        arr[i] = nullptr;
    }
}



template <class T>
int double_array<T>::size() {
    return width * height;
}

template <class T>
bool double_array<T>::empty() {
    return ((width == 0) && (height == 0));
}


#endif //LOGIC_ARRAY_HPP
