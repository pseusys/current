#ifndef LOGIC_ARRAY_H
#define LOGIC_ARRAY_H

#include <memory>
#include "../other/log.h"
#include "serializable.h"

template <class T>
class array : public serializable {
private:
    int length;
    std::shared_ptr<std::shared_ptr<T> []> arr = nullptr;

public:
    array(int capacity);
    explicit array(json& package);
    std::shared_ptr<json> pack(int serializer) override;

    T& operator[](int pos);
    void clear();

    int size();
    bool empty();
};



template <class T>
array<T>::array(int capacity) {
    static_assert(std::is_convertible<T*, serializable*>::value, "array class may only contain serializable objects");
    length = capacity;
    arr = std::shared_ptr<std::shared_ptr<T> []>(new std::shared_ptr<T> [capacity]);
}

template <class T>
array<T>::array(json& package) : serializable(package) {
    static_assert(std::is_convertible<T*, serializable*>::value, "array class may only contain serializable objects");
    this->length = package["length"];
    arr = std::shared_ptr<std::shared_ptr<T> []>(new std::shared_ptr<T> [this->length]);
    for (int i = 0; i < this->length; ++i) {
        arr[i] = std::make_shared<T>(package["data"][i]);
    }
}

template <class T>
std::shared_ptr<json> array<T>::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["length"] = this->length;
    (*package)["data"] = json::object();
    for (int i = 0; i < this->length; ++i) {
        (*package)["data"][i] = *(arr[i]->pack(serializer));
    }
    return package;
}



template <class T>
T& array<T>::operator[](int pos) {
    if ((pos >= 0) && (pos < this->length)) {
        return *(arr[pos]);
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}

template <class T>
void array<T>::clear() {
    for (int i = 0; i < this->length; ++i) {
        arr[i] = nullptr;
    }
}



template <class T>
int array<T>::size() {
    return length;
}

template <class T>
bool array<T>::empty() {
    return length == 0;
}


#endif //LOGIC_ARRAY_H
