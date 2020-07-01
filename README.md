# Setting up a C++/Python project with pybind11 and CMake

This tutorial shows how to set up a `pybind11` project with `CMake` for wrapping a `C++` library into `Python`.

## Requirements

Obviously, get `pybind11`:
```
conda install -c conda-forge pybind11
```

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
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

## Setting up the Python wrapper

Finally, let's get to wrapping the library into a Python. We're moving up a directory! In the main directory, let's make a new directory called `python`. It will hold all the glue code:
```
mkdir python
```
We also need a `CMakeLists.txt` file, with contents:
```
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(car)

include_directories(/home/oernst/local/include)
include_directories("${CMAKE_SOURCE_DIR}/cpp/include/car_bits")
include_directories("${CMAKE_SOURCE_DIR}/python")

file (GLOB SOURCE_FILES "cpp/src/*.cpp")
file (GLOB HEADER_FILES "cpp/include/car_bits/*.hpp")
file (GLOB PYTHON_FILES "python/*.cpp" "python/*.hpp")

# Set up such that XCode organizes the files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES} )

find_package(pybind11 REQUIRED)
pybind11_add_module(car 
	${SOURCE_FILES}
	${HEADER_FILES}
	${PYTHON_FILES}
)

target_link_libraries(car PUBLIC)

install(TARGETS car
  COMPONENT python
  LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
  )
```

You should be ready to build your `Python` library! Try:
```
mkdir build
cd build
cmake .. -DPYTHON_LIBRARY_DIR="/path/to/site-packages" -DPYTHON_EXECUTABLE="/path/to/executable/python3"
make
make install
```
As usual, you could also generate code using a generator for your favorite IDE, e.g. by adding `-GXcode` to the `cmake` command. My paths were:
```
DPYTHON_LIBRARY_DIR="/Users/USERNAME/opt/anaconda3/lib/python3.7/site-packages"
DPYTHON_EXECUTABLE="/Users/USERNAME/opt/anaconda3/bin/python3"
```
Note that if you are lazy like me, you can try to add for **testing**:
```
set(PYTHON_LIBRARY_DIR "/Users/oernst/opt/anaconda3/lib/python3.7/site-packages")
set(PYTHON_EXECUTABLE "/Users/oernst/opt/anaconda3/bin/python3")
```
in your `CMakeLists.txt` - obviously not a good trick for production!

Fire up `python` (make sure it's the same as you specified in `PYTHON_EXECUTABLE` above) and try:
```
>>> import car
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ImportError: dynamic module does not define module export function (PyInit_car)
```
You got a nice fat error, but that's OK! We didn't write the glue code yet, but at least your `CMake` is working and `Python` can find your library.