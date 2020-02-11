//
// Created by miles on 2/10/2020.
//

#include "utils/containers/list.h"

void out(list<int> arr) {
    for (int i = 0; i < arr.size(); ++i) {
        std::cout << *(arr.get(i)) << " ";
    }
    std::cout << std::endl;
}

int main() {
    list<int> arr = list<int>(5);

    int k = 5;
    for (int i = 0; i < arr.size(); ++i) {
        arr.set(std::make_shared<int>(k), i);
        k++;
    }

    out(arr);

    arr.push_back(std::make_shared<int>(100));
    out(arr);
    arr.push_back(std::make_shared<int>(101));
    out(arr);
    arr.push_front(std::make_shared<int>(200));
    out(arr);
    arr.push_front(std::make_shared<int>(201));

    out(arr);

    std::cout << *(arr.pop_back()) << " " << *(arr.peek_back()) << std::endl;
    std::cout << *(arr.pop_front()) << " " << *(arr.peek_front()) << std::endl;

    out(arr);

    arr.remove(5);
    std::cout << arr.size() << std::endl;

    out(arr);

    arr.clear();
    std::cout << arr.empty() << std::endl;

    out(arr);
}