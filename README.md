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

1. Using `VS Code`. Install the `CMake Tools` extension. Then, bring up the command pallette and select `CMake: Quick start`. Follow the prompts and enter a name - I chose `automobile`. When prompted for library or executable, choose `library`. Your directory should now look like this:
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
    In the include directory, we would like to have a single header to import. This way, we could later simply `#include <automobile>`. We can organize it as follows:
    ```
    cd cpp/include
    mkdir automobile_bits
    touch automobile
    ```
    Finally, let us create a header file in the `cpp/include/automobile_bits` directory:
    ```
    cd cpp/include/automobile_bits
    touch car.hpp
    ```
    The final directory structure should now look like this:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/automobile
    cpp/include/automobile_bits/car.hpp
    cpp/src/car.cpp
    ```

2. Manually create the files and directories, such that the final structure is:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/automobile
    cpp/include/automobile_bits/car.hpp
    cpp/src/car.cpp
    ```

We will need to edit the current `CMakeLists.txt` such that it can find the header and source files. I edited mine to read as follows:
```
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(automobile VERSION 0.1.0)

# Include dir
include_directories(/usr/local/include)

# Src
AUX_SOURCE_DIRECTORY(src SRC_FILES)

# Headers
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include/automobile_bits")

# Source files
set(SOURCE_FILES
    ${PROJECT_INCLUDE_DIR}/car.hpp
    ${PROJECT_SOURCE_DIR}/car.cpp
)

# Set up such that XCode organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Add library
add_library(automobile SHARED ${SOURCE_FILES})

# Include directories
target_include_directories(automobile PRIVATE include/automobile_bits)

# Install
install(TARGETS automobile DESTINATION lib)

# Install the headers
install(FILES include/automobile DESTINATION include)

# Create base directory
install(DIRECTORY include/automobile_bits DESTINATION include)
```

Let's also give the `car.hpp` and `car.cpp` files some reasonable content. For the header:
```
#include <string>

#ifndef CAR_H
#define CAR_H

namespace autos {

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
#include "../include/automobile_bits/car.hpp"

#include <iostream>

namespace autos {

Car::Car(std::string name) {
    _name = name;
}

std::string Car::get_name() const {
    return _name;
}

void Car::drive() const {
    std::cout << "Zoom Zoom" << std::endl;
}

}
```
Yes! I know they're dumb. Note that we introduced a namespace `autos` - this is always a good idea.

We also need to have the header file find the actual library. Edit the `include/automobile` file to read:
```
#ifndef AUTOMOBILE_LIBRARY_H
#define AUTOMOBILE_LIBRARY_H

#include "automobile_bits/car.hpp"

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
    should generate `automobile.xcodeproject` in the `build` directory.

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
#include <automobile>

#include <iostream>

int main() {

    autos::Car c("Mazda");

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

find_library(AUTOMOBILE_LIB automobile HINTS /usr/local/lib/)

add_executable(test src/test.cpp)

target_link_libraries(test PUBLIC ${AUTOMOBILE_LIB})
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
Zoom Zoom on road: mullholland
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

project(automobile)

include_directories("${CMAKE_SOURCE_DIR}/cpp/include/automobile_bits")
include_directories("${CMAKE_SOURCE_DIR}/python")

file (GLOB SOURCE_FILES "cpp/src/*.cpp")
file (GLOB HEADER_FILES "cpp/include/automobile_bits/*.hpp")
file (GLOB PYTHON_FILES "python/*.cpp" "python/*.hpp")

# Set up such that XCode organizes the files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES} )

find_package(pybind11 REQUIRED)
pybind11_add_module(automobile 
	${SOURCE_FILES}
	${HEADER_FILES}
	${PYTHON_FILES}
)

target_link_libraries(automobile PUBLIC)

install(TARGETS automobile
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
>>> import automobile
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ImportError: dynamic module does not define module export function (PyInit_automobile)
```
You got a nice fat error, but that's OK! We didn't write the glue code yet, but at least your `CMake` is working and `Python` can find your library.

## Wrapping the library into Python

Now for the actual logic of wrapping the `C++` code into `Python`. It will take place in the `python` directory. First create a file which will define the "module export function" that `python` was complaning about in the last part:
```
touch python/automobile.cpp
```
Give it the following content:
```
#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_car(py::module &);

namespace mcl {

PYBIND11_MODULE(automobile, m) {
    // Optional docstring
    m.doc() = "Automobile library";
    
    init_car(m);
}
}
```

Next, we will define the `init_car` method that was declared. We will do this in a separate file:
```
touch python/car.cpp
```
Edit it to read:
```
#include "../cpp/include/automobile_bits/car.hpp"

#include <pybind11/stl.h>

#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_car(py::module &m) {
    
    py::class_<autos::Car>(m, "Car")
    .def(py::init<std::string>(), py::arg("name"))
    .def("get_name",
         py::overload_cast<>( &autos::Car::get_name, py::const_))
    .def("drive",
         py::overload_cast<std::string>( &autos::Car::drive, py::const_),
         py::arg("road"));
}
```

I always find the code itself to be the best explanation, but some pointers:
- `py::class_<autos::Car>(m, "Car")` defines the class. The `"Car"` defines the name of the class in `Python` - you could change it if you want! Notice also the appearance of the namespace.
- `.def(py::init<std::string>(), py::arg("name"))` defines the constructor. The `py::arg("name")` allows you to use named arguments in `Python`.
- `.def("get_name", py::overload_cast<>( &autos::Car::get_name, py::const_))` wraps the `get_name` method. Note how the `const` declaration is wrapped.
- `.def("drive", py::overload_cast<std::string>( &autos::Car::drive, py::const_), py::arg("road"));` wraps the `drive` method. The arguments to the method are declared in `py::overload_cast<std::string>` (separated by commas if multiple), and can again be named using `py::arg("road")`. Also note the semicolon at the end - often forgotten, but this should be proper `C++` code.

You can now test your library. Run `make` and `make install` again to rebuild and install the library.

Fire up `python` and try it:
```
import automobile
c = automobile.Car("Mazda")
print("Made a car called: %s" % c.get_name())
c.drive("mullholland")
```
should give the same output as before:
```
Made a car called: Mazda
Zoom Zoom on road: mullholland
```

You could make another test script with those contents, located in a directory `tests/test.py`.

## Conclusion

That's it for this tutorial. There are obviously much more advanced features of `pybind11` not covered here - maybe for another day.