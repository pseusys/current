#include <iostream>

void prefix(std::string& mark, int** prefix) {
    int k = 0;
    (*prefix)[0] = 0;
    for (int i = 0; i < mark.size(); ++i) {
        if (k > 0 && mark[k] != mark[i]) k = (*prefix)[k];
        if (mark[k] == mark[i]) k += 1;
        (*prefix)[i] = k;
    }
}

int isCyclic(std::string& a, std::string& b) {
    int* prf = ((int*) (calloc(b.size(), sizeof(unsigned int*))));
    prefix(b, &prf);
    int counter = 0;
    for (int i = 0; i < a.size() * 2; ++i) {
        if (counter > 0 && a[i % a.size()] != b[counter]) counter = prf[counter - 1];
        if (a[i % a.size()] == b[counter]) counter++;
        if (counter == b.size()) {
            return i - a.size() + 1;
        }
    }
    return -1;
}

int main() {
    std::string a = std::string(), b = std::string();
    std::cin >> a;
    std::cin >> b;
    if (a.size() != b.size()) {
        std::cout << -1;
    } else {
        std::cout << isCyclic(a, b);
    }
}
