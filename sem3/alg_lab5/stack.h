#ifndef STACK_H
#define STACK_H


#include <cstdlib>
#include <stdexcept>

template <typename T>
class stack {
private:
    class stack_element {
    private:
        T value;
        stack_element* next;
        stack_element* previous;

    public:
        stack_element(T& value, stack_element *previous);

        virtual ~stack_element();

        T& getValue();

        void setNext(stack_element *next);

        void setPrevious(stack_element *prev);

        stack_element *getNext();

        stack_element *getPrevious();
    };

    int size;
    stack_element* first;
    stack_element* last;

public:
    stack();
    ~stack();

    void push_stack(T element);
    T pop_stack();

    void push_queue(T element);
    T pop_queue();

    bool isEmpty();
    const int* getStackSize() const;
};



template<typename T>
stack<T>::stack_element::stack_element(T& value, stack::stack_element* previous) {
    this->value = value;
    this->previous = previous;
    this->next = nullptr;
}

template<typename T>
stack<T>::stack_element::~stack_element() {
    free(this->next);
    free(this->previous);
}

template<typename T>
T& stack<T>::stack_element::getValue() {
    return value;
}

template<typename T>
void stack<T>::stack_element::setNext(stack::stack_element *next) {
    stack_element::next = next;
}

template<typename T>
void stack<T>::stack_element::setPrevious(stack::stack_element *prev) {
    stack_element::previous = prev;
}

template <typename T>
typename stack<T>::stack_element *stack<T>::stack_element::getNext() {
    return this->next;
}

template <typename T>
typename stack<T>::stack_element *stack<T>::stack_element::getPrevious() {
    return this->previous;
}



template<typename T>
const int* stack<T>::getStackSize() const {
    return &size;
}

template<typename T>
stack<T>::stack() {
    this->size = 0;
    this->first = nullptr;
    this->last = nullptr;
}

template<typename T>
stack<T>::~stack() {
    free(this->first);
    free(this->last);
}

template<typename T>
T stack<T>::pop_stack() {
    if (size == 0)
        throw std::runtime_error("Stack is empty!");

    stack_element *decapitation = last;
    T value = decapitation->getValue();

    last = decapitation->getPrevious();
    if (size > 1) {
        last->setNext(nullptr);
    } else {
        first = nullptr;
    }
    free(decapitation);

    size--;

    return value;
}

template<typename T>
void stack<T>::push_stack(T element) {
    stack_element* SE = new stack_element(element, last);

    if (size == 0) {
        first = SE;
    } else {
        last->setNext(SE);
    }

    size++;

    last = SE;
}

template<typename T>
T stack<T>::pop_queue() {
    if (size == 0)
        throw std::runtime_error("Stack is empty!");

    stack_element *decapitation = first;
    T value = decapitation->getValue();

    first = decapitation->getNext();
    if (size > 1) {
        first->setPrevious(nullptr);
    } else {
        last = nullptr;
    }
    free(decapitation);

    size--;

    return value;
}

template<typename T>
void stack<T>::push_queue(T element) {
    stack_element* SE = new stack_element(element, nullptr);
    SE->setNext(first);

    if (size == 0) {
        last = SE;
    } else {
        first->setPrevious(SE);
    }

    size++;

    first = SE;
}

template<typename T>
bool stack<T>::isEmpty() {
    return size == 0;
}

#endif // STACK_H
