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
