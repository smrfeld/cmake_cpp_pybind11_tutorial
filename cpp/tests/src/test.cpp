#include <automobile>

#include <iostream>

int main() {

    autos::Motorcycle c("Yamaha");

    std::cout << "Made a car called: " << c.get_name() << std::endl;

    c.ride("mullholland");

    return 0;
}
