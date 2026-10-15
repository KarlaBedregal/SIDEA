# SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa

Simulador de despacho de emergencias para la ciudad de Arequipa. Desarrollado en C++17 con interfaz gráfica en Qt6.

## Descripción

El sistema representa Arequipa como un grafo de calles y zonas. Sobre ese mapa se mueven unidades de emergencia (ambulancias, bomberos y serenazgo) que atienden incidentes generados aleatoriamente en tiempo real. El objetivo es comparar cómo distintas estrategias de asignación afectan el tiempo de respuesta.

## Tecnologías

- **C++17** — lógica del sistema
- **Qt6** (Widgets + Charts) — interfaz gráfica
- **GoogleTest** — tests unitarios
- **CMake 3.20+** — sistema de build
- **PlantUML** — diagramas UML

## Compilar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
```

## Ejecutar tests

```bash
cd build
ctest --output-on-failure
```

## Estructura del proyecto

```
SIDEA/
├── src/
│   ├── core/          # Clases del dominio (Unidad, Incidente, CentralDespacho)
│   │   ├── states/    # Patrón State (Libre, EnRuta, Atendiendo, Regresando)
│   ├── graph/         # Grafo<T>, Dijkstra<T>, ColaPrioridad<T>, MapaArequipa
│   ├── strategies/    # Patrón Strategy (EstrategiaDespacho y sus variantes)
│   ├── patterns/      # IObserver / IObservable
│   ├── threading/     # Generador de incidentes y thread por unidad
│   ├── gui/           # Widgets Qt (MainWindow, MapaWidget, PanelControl, Estadísticas)
│   ├── stats/         # Módulo de estadísticas
│   └── prediction/    # Módulo opcional: predicción de zonas de riesgo
├── tests/             # Tests unitarios (GoogleTest)
├── docs/UML/          # Diagramas PlantUML
└── assets/icons/      # Íconos de unidades e incidentes
```

## Equipo

Proyecto SIDEA — Curso de Programación Orientada a Objetos, UNSA 2026-B.
