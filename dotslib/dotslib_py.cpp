#define VERSION_INFO 1.0.0
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;
#include "DotsGame.hpp"
#include "dotslib_py.hpp"
#include <functional>

PYBIND11_MODULE(dotslib_py, m) 
{
    m.doc() = R"pbdoc(DotsGame module.)pbdoc";
    py::class_<DotsGame>(m, "DotsGame")
        .def(py::init<int,int>())
        .def("reset", &DotsGame::reset)
        .def("moveIsValid", &DotsGame::moveIsValid)
        .def("addPoint", &DotsGame::addPoint)
        .def("doMove", &DotsGame::doMove)
        .def("doMovePlayer", &DotsGame::doMovePlayer)
        .def("doRandomMove", &DotsGame::doRandomMove)
        .def("IsGameOver", &DotsGame::IsGameOver)
        .def("getGameState", &DotsGame::getGameState)
        .def("setGameState", &DotsGame::setGameState)
        .def("flipPlayers", &DotsGame::flipPlayers)        
        .def("renderToFile", &DotsGame::renderToFile)
        .def("getScore", &DotsGame::getScore);
        
#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}


