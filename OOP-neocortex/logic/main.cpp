//
// Created by miles on 2/10/2020.
//

#include "utils/containers/list.h"
#include "utils/containers/array.h"
#include "utils/containers/container.h"

void out(container<int>* arr) {
    for (int i = 0; i < arr->size(); ++i) {
        std::cout << (*arr)[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    list<int>* arr = new list<int>(5);

    int k = 5;
    for (int i = 0; i < arr->size(); ++i) {
        (*arr)[i] = k;
        k++;
    }

    out(arr);

    /*arr->push_back(100);
    out(arr);
    arr->push_back(101);
    out(arr);
    arr->push_front(200);
    out(arr);
    arr->push_front(201);

    out(arr);*/

    std::cout << arr->peek_back() << std::endl;
    arr->pop_back();
    std::cout << arr->peek_front() << std::endl;
    arr->pop_front();

    out(arr);

    arr->remove(2);
    std::cout << arr->size() << std::endl;

    out(arr);

    arr->clear();
    std::cout << arr->empty() << std::endl;

    out(arr);
    out(arr);

    array<int>* trt = new array<int>(5);

    int art = 0;
    for (int j = 0; j < 5; ++j) {
        (*trt)[j] = art;
        art++;
    }

    out(trt);

    int g = 100;
    int u = (*trt)[3];
    std::cout << u << std::endl;

    out(trt);

    trt->clear();

    out(trt);
}