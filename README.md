# Setting up a C++/Python project with pybind11 and CMake

This tutorial shows how to set up a `pybind11` project with `CMake` for wrapping a `C++` library into `Python`.

## Create a C++ project

We will use the outer (current) working directory to build python, and an inner directory called `cpp` to build the `C++` code. First make a C++ directory.
```
mkdir cpp
cd cpp
```
Next, we will initialize a C++ project. Two ways (of many more) are:

1. Using `VS Code`. Install the `CMake Tools` extension. Then, bring up the command pallette and select `CMake: Quick start`. Follow the prompts and enter a name - I chose `car`. When prompted for library or executable, choose `library`. Your directory should now look like this:
    ```
    cpp/build/
    cpp/car.cpp
    cpp/CMakeLists.txt
    ```
    We will separate the source and header files - this is always good practice. In the `cpp` directory, make two new directories:
    ```
    cd cpp
    mkdir include
    mkdir src
    ```
    and move the source file:
    ```
    mv car.cpp src/
    ```
    In the include directory, we would like to have a single header to import. This way, we could later simply `#include <car>`. We can organize it as follows:
    ```
    cd cpp/include
    mkdir car_bits
    touch car
    ```
    Finally, let us create a header file in the `cpp/include/car_bits` directory:
    ```
    cd cpp/include/car_bits
    touch car.hpp
    ```
    The final directory structure should now look like this:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/car
    cpp/include/car_bits/car.hpp
    cpp/src/car.cpp
    ```

2. Manually create the files and directories, such that the final structure is:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/car
    cpp/include/car_bits/car.hpp
    cpp/src/car.cpp
    ```

We will need to edit the current `CMakeLists.txt` such that it can find the header and source files. I edited mine to read as follows:
```
cmake_minimum_required(VERSION 3.0.0)
project(car VERSION 0.1.0)

# Include dir
include_directories(/usr/local/include)

# Src
AUX_SOURCE_DIRECTORY(src SRC_FILES)

# Headers
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include/car_bits")

# Source files
set(SOURCE_FILES
    ${PROJECT_INCLUDE_DIR}/car.hpp
    ${PROJECT_SOURCE_DIR}/car.cpp
)

# Set up such that XCode organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Add library
add_library(car SHARED ${SOURCE_FILES})

# Include directories
target_include_directories(car PRIVATE include/car_bits)

# Install
install(TARGETS car DESTINATION lib)

# Install the headers
install(FILES include/car DESTINATION include)

# Create base directory
install(DIRECTORY include/car_bits DESTINATION include)
```

Let's also give the `car.hpp` and `car.cpp` files some reasonable content. For the header:
```
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
```
and the source:
```
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
```
Yes! I know they're dumb. Note that we introduced a namespace `carn` - this is always a good idea.

We also need to have the header file find the actual library. Edit the `include/car` file to read:
```
#ifndef CAR_LIBRARY_H
#define CAR_LIBRARY_H

#include "car_bits/car.hpp"

#endif
```

We can now already build the library:
1. Using the command line:
    ```
    cd cpp/build
    cmake ..
    make
    make install
    ```

2. Using your favorite IDE, e.g. `XCode`:
    ```
    cd cpp/build
    cmake .. -GXcode
    ```
    should generate `car.xcodeproject` in the `build` directory.

Either way, you should get the library to build and install.

## Testing the C++ library

Before we go on to wrapping the library into `Python`, let's create a test for the `C++` library (not a real test, just somewhere for us to mess around!).

Create a new directory in `cpp`:
```
cd cpp
mkdir tests
```

Here we will again set up a `CMake` project for our test. Make the directory structure look as follows:
```
cpp/tests/CMakeLists.txt
cpp/tests/src/test.cpp
```

Edit the `test.cpp` file to read:
```
#include <car>

#include <iostream>

int main() {

    carn::Car c("Mazda");

    std::cout << "Made a car called: " << c.get_name() << std::endl;

    c.drive();

    return 0;
}
```
and edit the `CMakeLists.txt` file:
```
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(test)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(CAR_LIB car HINTS /usr/local/lib/)

add_executable(test src/test.cpp)

target_link_libraries(test PUBLIC ${CAR_LIB})
```

Make and run that bad boy using `XCode` as before, or from the command line:
```
mkdir build
cd build
cmake ..
make
cd ../bin
./test
```
Note that the binary will be in the `bin` directory. The output should be:
```
Made a car called: Mazda
Vroom Vroom
```