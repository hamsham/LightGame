
/* 
 * File:   main.h
 * Author: miles
 *
 * Created on February 9, 2016, 12:45 AM
 */

#ifndef MAIN_H
#define MAIN_H

class Display;

namespace global {
    extern Display* pDisplay;
}


template <typename data_t = unsigned>
constexpr data_t get_test_window_width() {
    return data_t{800};
}


template <typename data_t = unsigned>
constexpr data_t get_test_window_height() {
    return data_t{600};
}



#endif /* MAIN_H */

