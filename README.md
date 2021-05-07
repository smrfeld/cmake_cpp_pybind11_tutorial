# Setting up a C++/Python project with pybind11 and CMake

* [Read this in a nicer format on Medium.](https://medium.com/practical-coding/setting-up-a-c-python-project-with-pybind11-and-cmake-8de391494fca)
* [Follow my publication 'practical coding' on Medium for more articles like this.](https://medium.com/practical-coding)
* [Tutorial on more advanced pybind11 features here.](https://github.com/smrfeld/advanced_pybind11_features)

This tutorial shows how to set up a `pybind11` project with `CMake` for wrapping a `C++` library into `Python`.

The final result will be:
* A `C++` project you can build independent of `pybind11`.
* A `Python` library generated from wrapping the `C++` code.
* Both using `CMake`.

<img src="cover.jpg" alt="drawing" width="400"/>

[Image source.](https://www.flickr.com/photos/akira_1972/15689067279/)

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
    cpp/motorcycle.cpp
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
    mv motorcycle.cpp src/
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
    touch motorcycle.hpp
    ```
    The final directory structure should now look like this:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/automobile
    cpp/include/automobile_bits/motorcycle.hpp
    cpp/src/motorcycle.cpp
    ```

2. Manually create the files and directories, such that the final structure is:
    ```
    cpp/build
    cpp/CMakeLists.txt
    cpp/include/automobile
    cpp/include/automobile_bits/motorcycle.hpp
    cpp/src/motorcycle.cpp
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
    ${PROJECT_INCLUDE_DIR}/motorcycle.hpp
    ${PROJECT_SOURCE_DIR}/motorcycle.cpp
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

Let's also give the `motorcycle.hpp` and `motorcycle.cpp` files some reasonable content. For the header:
```
#include <string>

#ifndef CAR_H
#define CAR_H

namespace vehicles {

class Motorcycle {

private:

    /// Name
    std::string _name;

public:

    /// Constructor
    Motorcycle(std::string name);

    /// Get motorcycle name
    /// @return Motorcycle name
    std::string get_name() const;

    /// Drive the motorcycle
    void ride() const;
};

}

#endif
```
and the source:
```
#include "../include/automobile_bits/motorcycle.hpp"

#include <iostream>

namespace vehicles {

Motorcycle::Motorcycle(std::string name) {
    _name = name;
}

std::string Motorcycle::get_name() const {
    return _name;
}

void Motorcycle::ride() const {
    std::cout << "Zoom Zoom" << std::endl;
}

}
```
Yes! I know they're dumb. Note that we introduced a namespace `vehicles` - this is always a good idea.

We also need to have the header file find the actual library. Edit the `include/automobile` file to read:
```
#ifndef AUTOMOBILE_LIBRARY_H
#define AUTOMOBILE_LIBRARY_H

#include "automobile_bits/motorcycle.hpp"

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
cpp/tests/src/test_cpp.cpp
```

Edit the `test_cpp.cpp` file to read:
```
#include <automobile>

#include <iostream>

int main() {

    vehicles::Motorcycle c("Yamaha");

    std::cout << "Made a motorcycle called: " << c.get_name() << std::endl;

    c.ride();

    return 0;
}
```
and edit the `CMakeLists.txt` file:
```
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(test_cpp)

include_directories(/usr/local/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin)

find_library(AUTOMOBILE_LIB automobile HINTS /usr/local/lib/)

add_executable(test_cpp src/test_cpp.cpp)

target_link_libraries(test_cpp PUBLIC ${AUTOMOBILE_LIB})
```

Make and run that bad boy using `XCode` as before, or from the command line:
```
mkdir build
cd build
cmake ..
make
cd ../bin
./test_cpp
```
Note that the binary will be in the `bin` directory. The output should be:
```
Made a motorcycle called: Yamaha
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
```

If pybind11 hasn't been installed in the system (e.g.: `conda install -c conda-forge pybind11`), you can use the `pybind11.cmake` file to fetch the package (see `CMakeLists.txt` as an example).

```
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
set(PYTHON_LIBRARY_DIR "/Users/USERNAME/opt/anaconda3/lib/python3.7/site-packages")
set(PYTHON_EXECUTABLE "/Users/USERNAME/opt/anaconda3/bin/python3")
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

void init_motorcycle(py::module &);

namespace mcl {

PYBIND11_MODULE(automobile, m) {
    // Optional docstring
    m.doc() = "Automobile library";
    
    init_motorcycle(m);
}
}
```

Next, we will define the `init_motorcycle` method that was declared. We will do this in a separate file:
```
touch python/motorcycle.cpp
```
Edit it to read:
```
#include "../cpp/include/automobile_bits/motorcycle.hpp"

#include <pybind11/stl.h>

#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_motorcycle(py::module &m) {
    
    py::class_<vehicles::Motorcycle>(m, "Motorcycle")
    .def(py::init<std::string>(), py::arg("name"))
    .def("get_name",
         py::overload_cast<>( &vehicles::Motorcycle::get_name, py::const_))
    .def("ride",
         py::overload_cast<std::string>( &vehicles::Motorcycle::ride, py::const_),
         py::arg("road"));
}
```

I always find the code itself to be the best explanation, but some pointers:
- `py::class_<vehicles::Motorcycle>(m, "Motorcycle")` defines the class. The `"Motorcycle"` defines the name of the class in `Python` - you could change it if you want! Notice also the appearance of the namespace.
- `.def(py::init<std::string>(), py::arg("name"))` defines the constructor. The `py::arg("name")` allows you to use named arguments in `Python`.
- `.def("get_name", py::overload_cast<>( &vehicles::Motorcycle::get_name, py::const_))` wraps the `get_name` method. Note how the `const` declaration is wrapped.
- `.def("ride", py::overload_cast<std::string>( &vehicles::Motorcycle::ride, py::const_), py::arg("road"));` wraps the `ride` method. The arguments to the method are declared in `py::overload_cast<std::string>` (separated by commas if multiple), and can again be named using `py::arg("road")`. Also note the semicolon at the end - often forgotten, but this should be proper `C++` code.

You can now test your library. Run `make` and `make install` again to rebuild and install the library.

Fire up `python` and try it:
```
import automobile
c = automobile.Motorcycle("Yamaha")
print("Made a motorcycle called: %s" % c.get_name())
c.ride("mullholland")
```
should give the same output as before:
```
Made a motorcycle called: Yamaha
Zoom Zoom on road: mullholland
```
You could make another test script with those contents, located in a directory `tests/test.py`.

## Conclusion

That's it for this tutorial. The nice part about this setup is that you can build your `C++` project in peace from the `cpp` directory, and then at the end in the outer layer worry about wrapping it into `Python`.

[You can read about more advanced pybindy11 features in another tutorial here](https://github.com/smrfeld/advanced_pybind11_features).
