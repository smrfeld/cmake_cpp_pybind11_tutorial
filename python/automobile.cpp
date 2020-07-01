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
