#ifndef LOGIC_CONTAINER_H
#define LOGIC_CONTAINER_H

#include <memory>
#include "../../other/log.h"

template <typename E>
class container {
protected:
    int length = 0;
public:
    virtual E& operator[](int pos) = 0;
    virtual void clear() = 0;

    int size();
    bool empty();
};



template<typename E>
int container<E>::size() {
    return length;
}

template<typename E>
bool container<E>::empty() {
    return length == 0;
}


#endif //LOGIC_CONTAINER_H
