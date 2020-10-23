#include "../include/automobile_bits/motorcycle.hpp"

#include <iostream>

namespace vehicles {

Motorcycle::Motorcycle(std::string name) {
    _name = name;
}

std::string Motorcycle::get_name() const {
    return _name;
}

void Motorcycle::ride(std::string road) const {
    std::cout << "Zoom Zoom on road: " << road << std::endl;
}

}
