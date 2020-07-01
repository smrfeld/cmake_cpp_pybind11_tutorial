#include <string>

#ifndef CAR_H
#define CAR_H

namespace carn {

class Car {

private:

    /// Name
    std::string _name;

public:

    /// Constructor
    Car(std::string name);

    /// Get car name
    /// @return Car name
    std::string get_name() const;

    /// Drive the car
    void drive() const;
};

}

#endif