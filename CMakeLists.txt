cmake_minimum_required(VERSION 3.20)
project(SIDEA VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)

set(SIDEA_LIB_SOURCES
    src/core/Incidente.cpp
    src/core/Unidad.cpp
    src/core/Ambulancia.cpp
    src/core/Bombero.cpp
    src/core/Serenazgo.cpp
    src/core/CentralDespacho.cpp
    src/core/IncidenteFactory.cpp
    src/core/UnidadFactory.cpp
    src/core/states/EstadoLibre.cpp
    src/core/states/EstadoEnRuta.cpp
    src/core/states/EstadoAtendiendo.cpp
    src/core/states/EstadoRegresando.cpp
    src/graph/MapaArequipa.cpp
    src/strategies/EstrategiaMasCercana.cpp
    src/strategies/EstrategiaPrioridad.cpp
    src/strategies/EstrategiaBalanceCarga.cpp
    src/patterns/IObservable.cpp
    src/threading/GeneradorIncidentes.cpp
    src/threading/SimuladorThread.cpp
    src/gui/MainWindow.cpp
    src/gui/MapaWidget.cpp
    src/gui/PanelControl.cpp
    src/gui/EstadisticasWidget.cpp
    src/stats/Estadisticas.cpp
    src/prediction/HistorialIncidentes.cpp
    src/prediction/ModeloPrediccion.cpp
)

add_library(sidea_core OBJECT ${SIDEA_LIB_SOURCES})
target_include_directories(sidea_core PUBLIC src)
target_link_libraries(sidea_core PUBLIC Qt6::Core Qt6::Widgets Qt6::Charts)

add_executable(SIDEA src/main.cpp resources.qrc)
target_link_libraries(SIDEA PRIVATE sidea_core)

enable_testing()
add_subdirectory(tests)
