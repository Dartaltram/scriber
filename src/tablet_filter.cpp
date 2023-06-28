#include "../headers/tablet_filter.h"


// --- ACCESSORS --- //

// returns the raw value inside the filter
uint8_t* scriber::filter::raw() { return &m_filter; }

// returns the value of the filter at the specified bit
bool scriber::filter::get_bit(uint8_t bitIndex) { return (bitIndex <= sizeof(m_filter) * 8 -1 && (m_filter >> bitIndex) % 2 == 1); }
// returns the value of the filter at the specified level
bool scriber::filter::get_level(LogLevel level) { return get_bit(uint8_t(level)); }

//returns all values as bool array
std::vector<bool> scriber::filter::get_all() { 
    
    std::vector<bool> b_filterValue(sizeof(m_filter) * 8, false); //construct vector with appropriate size and initialise it with false
    for (size_t i = b_filterValue.size()-1; i >= 0; i--) { b_filterValue[i] = get_bit(i); } //set correct values
    return b_filterValue;
}


// --- SETTERS --- //

// sets the raw value inside the filter
void scriber::filter::set_raw(uint8_t newFilterValue) { m_filter = newFilterValue;}

// sets the value of the filter at the specified bit
void scriber::filter::set_bit(uint8_t bitIndex, bool value) {

    if (bitIndex > sizeof(m_filter) * 8 - 1)
        return;
    
    if (value)
        m_filter |= 1 << bitIndex;
    else
        m_filter &= ~(1 << bitIndex);
}
// returns the value of the filter at the specified level
void scriber::filter::set_level(LogLevel level, bool value) { set_bit(uint8_t(level), value); }

// sets all the filter at the given value
void scriber::filter::set_all(bool value) { for (size_t i = 0; i < sizeof(m_filter)*8 -1; i++) { set_bit(i, value); } }