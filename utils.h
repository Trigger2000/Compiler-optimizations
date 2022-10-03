#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#define UNREACHABLE()                                                   \
    do {                                                                \
        std::cerr << "this line should be unreachable" << std::endl;    \
        std::cerr << __FILE__ << " " << __LINE__ << std::endl;          \
    } while(0);


class IdCounter {
public:
    static uint32_t AssignId() {
        id++;
        return id;
    }
private:
    inline static uint32_t id = 0;
};

#endif // UTILS_H
