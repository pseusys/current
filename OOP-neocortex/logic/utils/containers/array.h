#ifndef LOGIC_ARRAY_H
#define LOGIC_ARRAY_H

#include "container.h"

template <typename E>
class array : public container<E> {
private:
    std::shared_ptr<std::shared_ptr<E> []> arr = nullptr;

public:
    explicit array(int capacity);

    std::shared_ptr<E> get(int pos) override;
    void set(std::shared_ptr<E> item, int pos) override;

    void clear() override;
};



template<typename E>
array<E>::array(int capacity) {
    this->length = capacity;
    arr = std::shared_ptr<std::shared_ptr<E> []>(new std::shared_ptr<E> [capacity]);
}



template<typename E>
std::shared_ptr<E> array<E>::get(int pos) {
    if ((pos >= 0) && (pos < this->size())) {
        return arr[pos];
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}

template<typename E>
void array<E>::set(std::shared_ptr<E> item, int pos) {
    if ((pos >= 0) && (pos < this->size())) {
        arr[pos] = std::shared_ptr<E>(item);
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}



template<typename E>
void array<E>::clear() {
    for (int i = 0; i < this->length; ++i) {
        arr[i] = nullptr;
    }
}


#endif //LOGIC_ARRAY_H
