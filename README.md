# SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa

Simulador de despacho de emergencias para Arequipa. C++17 + Qt6.  
Curso de Programación Orientada a Objetos — UNSA 2026-B.

---

## Tareas por persona

Cada integrante tiene 3 issues asignados, uno por sprint. Búscalos en la pestaña [Issues](../../issues) filtrados por tu usuario, o directo acá:

| Integrante | Sprint 1 (hoy) | Sprint 2 (hasta 21 nov) | Sprint 3 (hasta 5 dic) |
|------------|---------------|------------------------|------------------------|
| **KarlaBedregal** | [#3 Dijkstra + ColaPrioridad](../../issues/3) | [#10 Threads por unidad](../../issues/10) | [#12 Observer Qt](../../issues/12) |
| **tniia** | [#2 Grafo\<T\> + Unidad](../../issues/2) | [#7 Patrón State](../../issues/7) | [#11 Productor-consumidor](../../issues/11) |
| **ISacck144** | [#4 MapaArequipa + CentralDespacho](../../issues/4) | [#8 Estrategia más cercana](../../issues/8) | [#15 Panel de control](../../issues/15) |
| **Daniel-IS04** | [#1 Setup + Qt + Incidente](../../issues/1) | [#9 Estrategias prioridad/balance](../../issues/9) | [#16 Estadísticas](../../issues/16) |
| **SharonCondoriUNSA** | [#5 Subclases Unidad](../../issues/5) | [#13 Mapa Qt animado](../../issues/13) | [#17 Historial + predicción](../../issues/17) |
| **FernandoBarra05** | [#6 Factories + tests](../../issues/6) | [#14 Íconos + movimiento](../../issues/14) | [#18 Reubicación + UML + docs](../../issues/18) |

> Abre tu issue, lee las tareas y el criterio de aceptación antes de empezar.

---

## Sprints

| Sprint | Fechas | Enfoque |
|--------|--------|---------|
| **Sprint 1** | 7 nov (hoy) | Compilación, tests base y estructura |
| **Sprint 2** | 8 – 21 nov | Lógica de despacho + concurrencia |
| **Sprint 3** | 22 nov – 5 dic | GUI + estadísticas + predicción |

### Sprint 1 — foco de hoy

La estructura de archivos ya existe. Hoy cada quien solo necesita:

- **KarlaBedregal (#3):** correr `ctest` y verificar que `test_dijkstra` y `test_cola_prioridad` pasan en verde.
- **tniia (#2):** revisar `Grafo.h` y `Unidad.h/cpp`, asegurarse que `Grafo<T>` compila y `Unidad` no es instanciable directamente.
- **ISacck144 (#4):** implementar `MapaArequipa::construir()` con al menos 20 nodos (zonas de Arequipa con coordenadas x/y).
- **Daniel-IS04 (#1):** verificar que el proyecto compila con CMake y la ventana Qt abre.
- **SharonCondoriUNSA (#5):** implementar `atenderIncidente()` en `Ambulancia`, `Bombero` y `Serenazgo`.
- **FernandoBarra05 (#6):** verificar que `test_incidente` y `test_unidad` pasan; agregar 2 tests propios.

---

## Cómo compilar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
```

## Correr tests

```bash
cd build
ctest --output-on-failure
```

---

## Flujo de trabajo (leer antes de tocar código)

```
main  ←  develop  ←  feature/S1-B1-nombre-corto
```

**Pasos para cada tarea:**

```bash
# 1. Siempre partir de develop actualizado
git checkout develop
git pull origin develop

# 2. Crear tu rama con el número de tu issue
git checkout -b feature/S1-B1-setup    # cambia S1-B1 por el tuyo

# 3. Implementar, commitear con mensaje claro
git add src/core/Incidente.cpp
git commit -m "feat: implementar Incidente con getters y marcarAtendido"

# 4. Subir y abrir PR → siempre hacia develop, nunca a main
git push origin feature/S1-B1-setup
```

> En GitHub te va a aparecer el botón "Compare & pull request" — abrelo hacia `develop`.  
> Pide revisión a cualquier compañero. Con 1 aprobación ya se puede mergear.

**Nunca pushear directo a `main`.** Está protegida y va a rechazar el push.

---

## Estructura del proyecto

```
SIDEA/
├── src/
│   ├── core/          # Unidad, Incidente, CentralDespacho, Factories
│   │   └── states/    # Patrón State: Libre, EnRuta, Atendiendo, Regresando
│   ├── graph/         # Grafo<T>, Dijkstra<T>, ColaPrioridad<T>, MapaArequipa
│   ├── strategies/    # Patrón Strategy: 3 estrategias de despacho
│   ├── patterns/      # IObserver / IObservable
│   ├── threading/     # GeneradorIncidentes, SimuladorThread
│   ├── gui/           # MainWindow, MapaWidget, PanelControl, EstadisticasWidget
│   ├── stats/         # Estadisticas
│   └── prediction/    # HistorialIncidentes, ModeloPrediccion (Sprint 3)
├── tests/             # Tests unitarios (GoogleTest vía FetchContent)
├── docs/UML/          # Diagramas PlantUML
└── assets/icons/      # Íconos de unidades e incidentes
```

---

## Tablero Kanban

Seguimiento en tiempo real: [github.com/users/KarlaBedregal/projects/4](https://github.com/users/KarlaBedregal/projects/4)

Mueve tu issue a **In Progress** cuando arranques y a **Done** cuando tu PR se mergee.
