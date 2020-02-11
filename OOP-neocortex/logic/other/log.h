//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_LOG_H
#define LOGIC_LOG_H

#include <iostream>

class log {
public:
    template<class... Args>
    static void say(Args... args);

    template<class... Args>
    static void report();
};


template<class... Args>
void log::say(Args... args) {
    (std::cout << ... << args) << std::endl;
}

template<class... Args>
void log::report() {
    say("Some error happens! See message for details.");
}


#endif //LOGIC_LOG_H
