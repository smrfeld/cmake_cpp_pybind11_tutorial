#include "../include/automobile_bits/car.hpp"

#include <iostream>

namespace autos {

Car::Car(std::string name) {
    _name = name;
}

std::string Car::get_name() const {
    return _name;
}

void Car::drive(std::string road) const {
    std::cout << "Zoom Zoom on road: " << road << std::endl;
}

}