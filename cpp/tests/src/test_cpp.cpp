#include <automobile>

#include <iostream>

int main() {

    vehicles::Motorcycle c("Yamaha");

    std::cout << "Made a car called: " << c.get_name() << std::endl;

    c.ride("mullholland");

    return 0;
}
