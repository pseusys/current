//
// Created by alex on 10/21/19.
//

#ifndef ALG_LAB1_VISIBLE_STACK_H
#define ALG_LAB1_VISIBLE_STACK_H

#include <bits/stdc++.h>
#include "universal_exception.h"

#define EMPTY_STACK "Stack is empty!"

using namespace std;


template <typename T>
class visible_stack {
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

        stack_element *getNext();

        stack_element *getPrevious();
    };

    int size{};  // номер текущего элемента стека
    stack_element* first;
    stack_element* last;

public:
    visible_stack();                  // по умолчанию размер стека равен 10 элементам
    ~visible_stack();                         // деструктор

    void push(T& element);     // поместить элемент в вершину стека
    T pop();                   // удалить элемент из вершины стека и вернуть его

    string toString();

    bool isEmpty();
    int getStackSize();        // получить размер стека
};



template<typename T>
visible_stack<T>::stack_element::stack_element(T& value, visible_stack::stack_element* previous) {
    this->value = value;
    this->previous = previous;
    this->next = nullptr;
}

template<typename T>
visible_stack<T>::stack_element::~stack_element() {
    free(this->next);
    free(this->previous);
}

template<typename T>
T& visible_stack<T>::stack_element::getValue() {
    return value;
}

template<typename T>
void visible_stack<T>::stack_element::setNext(visible_stack::stack_element *next) {
    stack_element::next = next;
}

template <typename T>
typename visible_stack<T>::stack_element *visible_stack<T>::stack_element::getNext() {
    return this->next;
}

template <typename T>
typename visible_stack<T>::stack_element *visible_stack<T>::stack_element::getPrevious() {
    return this->previous;
}



template<typename T>
int visible_stack<T>::getStackSize() {
    return size;
}

template<typename T>
visible_stack<T>::visible_stack() {
    this->size = 0;
    this->first = nullptr;
    this->last = nullptr;
}

template<typename T>
visible_stack<T>::~visible_stack() {
    free(this->first);
    free(this->last);
}

template<typename T>
void visible_stack<T>::push(T& element) {
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
T visible_stack<T>::pop() {
    if (size == 0)
        throw universal_exception(1);

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
string visible_stack<T>::toString() {
    string out;

    if (size > 0) {
        for (int i = 0; i < (size * 2 + 1); i++) {
            out += "_";
        }

        out += "\n|";

        stack_element *current = first;
        for (int j = 0; j < size; j++) {
            out += string(1, (T) current->getValue()) + "|";
            current = current->getNext();
        }

        out += "\n";

        for (int i = 0; i < (size * 2 + 1); i++) {
            out += "‾";
        }
    } else {
        out = EMPTY_STACK;
    }

    return out;
}

template<typename T>
bool visible_stack<T>::isEmpty() {
    return size == 0;
}

#endif //ALG_LAB1_VISIBLE_STACK_H
