#include <string>

#ifndef MOTORCYCLE_H
#define MOTORCYCLE_H

namespace vehicles {

class Motorcycle {

private:

    /// Name
    std::string _name;

public:

    /// Constructor
    Motorcycle(std::string name);

    /// Get name
    /// @return Name
    std::string get_name() const;

    /// Ride the bike
    /// @param road Name of the road
    void ride(std::string road) const;
};

}

#endif
