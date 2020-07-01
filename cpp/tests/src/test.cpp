#include <automobile>

#include <iostream>

int main() {

    autos::Car c("Mazda");

    std::cout << "Made a car called: " << c.get_name() << std::endl;

    c.drive("mullholland");

    return 0;
}
