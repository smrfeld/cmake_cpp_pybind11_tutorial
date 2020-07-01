#include <car>

#include <iostream>

int main() {

    carn::Car c("Mazda");

    std::cout << "Made a car called: " << c.get_name() << std::endl;

    c.drive();

    return 0;
}
