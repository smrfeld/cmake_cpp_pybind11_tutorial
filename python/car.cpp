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
