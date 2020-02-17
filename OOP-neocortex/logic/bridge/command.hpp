//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_COMMAND_HPP
#define LOGIC_COMMAND_HPP


class command {
    enum signals {common, selection, location};

private:
    int signal;
    int arguments_count;
    int arguments [];

public:
    int get_signal();
    int get_arguments_count();
    int get_argument_at(int position);
};


#endif //LOGIC_COMMAND_HPP
