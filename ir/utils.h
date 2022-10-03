#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#define UNREACHABLE()                                                                                                  \
    do {                                                                                                               \
        std::cerr << "this line should be unreachable" << std::endl;                                                   \
        std::cerr << __FILE__ << " " << __LINE__ << std::endl;                                                         \
    } while (0);

#endif // UTILS_H
