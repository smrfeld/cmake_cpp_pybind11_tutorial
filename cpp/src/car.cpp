#include "../include/car_bits/car.hpp"

#include <iostream>

namespace carn {

Car::Car(std::string name) {
    _name = name;
}

std::string Car::get_name() const {
    return _name;
}

void Car::drive() const {
    std::cout << "Vroom Vroom" << std::endl;
}

}