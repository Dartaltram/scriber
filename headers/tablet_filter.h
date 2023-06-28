#ifndef TABLET_FILTER_H
#define TABLET_FILTER_H

#include <iostream>
#include <vector>

#include "tablet_base.h"

namespace scriber {

    class filter
    {
        private:
            uint8_t m_filter;


        // --- ACCESSORS --- //
        public:
            // returns the raw value inside the filter
            uint8_t* raw();

            // returns the value of the filter at the specified bit
            bool get_bit(uint8_t bitIndex);
            // returns the value of the filter at the specified level
            bool get_level(LogLevel level);

            //returns all values as bool array
            std::vector<bool> get_all();


        // --- SETTERS --- //
        public:
            // sets the raw value inside the filter
            void set_raw(uint8_t newFilterValue);

            // sets the value of the filter at the specified bit
            void set_bit(uint8_t bitIndex, bool value);
            // returns the value of the filter at the specified level
            void set_level(LogLevel level, bool value);

            // sets all the filter at the given value
            void set_all(bool value);

        public:
            filter() : m_filter(255) {}
            ~filter() {}
    };
}

#endif // TABLET_FILTER_H