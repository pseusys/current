#ifndef LOGIC_LIST_H
#define LOGIC_LIST_H

#include "container.h"

template <typename E>
class list : public container<E> {
private:
    class element {
    public:
        E data;
        std::shared_ptr<element> next = nullptr;
        std::shared_ptr<element> previous = nullptr;
    };

    std::shared_ptr<element> head = nullptr;
    std::shared_ptr<element> tail = nullptr;

public:
    list();
    explicit list(int capacity);

    E& operator[](int pos) override;

    void push_back(E& item);
    void pop_back();
    E& peek_back();

    void push_front(E& item);
    void pop_front();
    E& peek_front();

    void remove(int pos);
    void clear() override;
};



template<typename E>
list<E>::list() = default;

template<typename E>
list<E>::list(int capacity) {
    this->length = capacity;
    std::shared_ptr<element> first;
    first = head = std::shared_ptr<element>(new element());
    for (int i = 0; i < capacity - 1; ++i) {
        std::shared_ptr<element> next = std::shared_ptr<element>(new element());
        first.get()->next = next;
        next.get()->previous = first;
        first = next;
    }
    tail = first;
}



template<typename E>
E& list<E>::operator[](int pos) {
    if ((pos >= 0) && (pos < this->length)) {
        std::shared_ptr<element> first = head;
        for (int i = 0; i < pos; ++i) {
            first = first.get()->next;
        }
        return first.get()->data;
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}



template<typename E>
void list<E>::push_back(E& item) {
    std::shared_ptr<element> elem = std::shared_ptr<element>(new element());
    elem.get()->data = item;
    if (this->empty()) {
        head = tail = elem;
    } else {
        elem.get()->previous = tail;
        tail.get()->next = elem;
        tail = elem;
    }
    this->length++;
}

template<typename E>
void list<E>::pop_back() {
    remove(this->length - 1);
}

template<typename E>
E& list<E>::peek_back() {
    if (!this->empty()) {
        return tail.get()->data;
    } else {
        log::report();
        throw std::runtime_error("Array is empty!");
    }
}



template<typename E>
void list<E>::push_front(E& item) {
    std::shared_ptr<element> elem = std::shared_ptr<element>(new element());
    elem.get()->data = item;
    if (this->empty()) {
        head = tail = elem;
    } else {
        elem.get()->next = head;
        head.get()->previous = elem;
        head = elem;
    }
    this->length++;
}

template<typename E>
void list<E>::pop_front() {
    remove(0);
}

template<typename E>
E& list<E>::peek_front() {
    if (!this->empty()) {
        return head.get()->data;
    } else {
        log::report();
        throw std::runtime_error("Array is empty!");
    }
}



template<typename E>
void list<E>::remove(int pos) {
    if ((pos >= 0) && (pos < this->length)) {
        this->length--;
        std::shared_ptr<element> first = head;
        for (int i = 0; i < pos; ++i) {
            first = first.get()->next;
        }
        if (first.get()->previous != nullptr) first.get()->previous.get()->next = first.get()->next;
        else head = first.get()->next;
        if (first.get()->next != nullptr) first.get()->next.get()->previous = first.get()->previous;
        else tail = first.get()->previous;
        first.reset();
    } else {
        log::report();
        throw std::runtime_error("Index out of bounds!");
    }
}

template<typename E>
void list<E>::clear() {
    head.reset();
    tail.reset();
    this->length = 0;
}


#endif //LOGIC_LIST_H
