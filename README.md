# SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa

Simulador de despacho de emergencias para Arequipa. C++17 + Qt6.  
Curso de Programación Orientada a Objetos — UNSA 2026-B.

---

## Tareas por persona

Cada integrante tiene issues asignados por sprint (sorteo). Búscalos en la pestaña [Issues](../../issues) filtrados por tu usuario.

| Integrante | Sprint 1 | Sprint 2 | Sprint 3 |
|------------|----------|----------|----------|
| **KarlaBedregal** | [#3 Dijkstra + ColaPrioridad](../../issues/3) ✅ | [#10 Threads](../../issues/10) · [#12 Observer Qt](../../issues/12) | — |
| **tniia** | [#2 Grafo + Unidad](../../issues/2) ✅ | [#7 Patrón State](../../issues/7) · [#11 Productor-consumidor](../../issues/11) | — |
| **ISacck144** | [#4 Mapa + CentralDespacho](../../issues/4) ✅ | [#8 Estrategia más cercana](../../issues/8) | [#15 Panel de control](../../issues/15) |
| **Daniel-IS04** | [#1 Setup + Incidente](../../issues/1) ✅ | [#9 Estrategias prioridad/balance](../../issues/9) | [#16 Estadísticas](../../issues/16) |
| **SharonCondoriUNSA** | [#5 Subclases Unidad](../../issues/5) ✅ | — | [#13 Mapa Qt animado](../../issues/13) · [#17 Historial + predicción](../../issues/17) |
| **FernandoBarra05** | [#6 Factories + tests](../../issues/6) | — | [#14 Íconos + movimiento](../../issues/14) · [#18 UML + docs](../../issues/18) |

> ✅ = mergeado. Abre tu issue antes de empezar y muévelo a "In Progress" en el Kanban.

---

## Sprints

| Sprint | Fechas | Enfoque |
|--------|--------|---------|
| **Sprint 1** | 7 nov | Compilación, estructura base y tests | ✅ Completo |
| **Sprint 2** | 8 – 24 nov | Lógica de despacho + concurrencia | En curso |
| **Sprint 3** | 25 nov – 26 nov | GUI animada + estadísticas + predicción | Pendiente |

---

## Sprint 2 — detalle de tareas

### #7 — Patrón State en Unidad · tniia

**Archivos:** `src/core/states/EstadoLibre.h/cpp`, `EstadoEnRuta.h/cpp`, `EstadoAtendiendo.h/cpp`, `EstadoRegresando.h/cpp`, `src/core/states/EstadoUnidad.h`

**Tareas:**
- Implementar `EstadoUnidad::manejar(Unidad&)` en cada subclase:
  - `EstadoLibre::manejar` — no hace nada (unidad en espera)
  - `EstadoEnRuta::manejar` — avanza un nodo en `m_ruta`; al llegar, cambia a `EstadoAtendiendo`
  - `EstadoAtendiendo::manejar` — llama a `atenderIncidente` y cambia a `EstadoRegresando`
  - `EstadoRegresando::manejar` — avanza de regreso a `m_nodoBase`; al llegar, cambia a `EstadoLibre`
- Cada estado implementa `nombre()` retornando su string ("Libre", "EnRuta", "Atendiendo", "Regresando")

**Criterio de aceptación:** `unidad.tick()` avanza el estado correctamente en cada ciclo. `estaLibre()` retorna `true` solo en `EstadoLibre`.

---

### #8 — EstrategiaDespacho y estrategia más cercana · ISacck144

**Archivos:** `src/strategies/EstrategiaDespacho.h`, `src/strategies/EstrategiaMasCercana.h/cpp`

**Tareas:**
- Verificar que `EstrategiaDespacho` es una clase abstracta con `seleccionar(unidades, incidente)` puro
- Implementar `EstrategiaMasCercana::seleccionar`:
  - Para cada unidad libre, correr `dijkstra(mapa, unidad.getNodoActual())`
  - Calcular `distanciaA(incidente.getNodoUbicacion())`
  - Retornar la unidad libre con menor distancia
- Usar `MapaArequipa::instancia()` para acceder al grafo

**Criterio de aceptación:** Dado un conjunto de unidades libres en nodos conocidos y un incidente en un nodo, `seleccionar` retorna la unidad más cercana según Dijkstra.

---

### #9 — Estrategias prioridad y balance de carga · Daniel-IS04

**Archivos:** `src/strategies/EstrategiaPrioridad.h/cpp`, `src/strategies/EstrategiaBalanceCarga.h/cpp`

**Tareas:**
- `EstrategiaPrioridad::seleccionar`: selecciona la primera unidad libre cuyo tipo coincide con el tipo del incidente (Ambulancia→URGENCIA_MEDICA, Bombero→INCENDIO/ACCIDENTE, Serenazgo→ROBO); si no hay coincidencia de tipo, retorna cualquier unidad libre
- `EstrategiaBalanceCarga::seleccionar`: ya implementada en Sprint 1 (retorna la unidad libre con menos `m_incidentesAtendidos`); verificar que pasa el test existente
- Agregar al menos 1 test por estrategia en `tests/test_estrategias.cpp`

**Criterio de aceptación:** Los tests de `test_estrategias.cpp` pasan. Cada estrategia retorna `nullptr` si no hay unidades libres.

---

### #10 — Thread por unidad · KarlaBedregal

**Archivos:** `src/core/Unidad.h/cpp`, `src/threading/SimuladorThread.h/cpp`

**Tareas:**
- Agregar `std::thread m_hilo` y `std::atomic<bool> m_activo` a `Unidad`
- Implementar `Unidad::iniciarHilo()`: lanza un thread que llama `tick()` en loop cada ~200ms mientras `m_activo`
- Implementar `Unidad::detenerHilo()`: setea `m_activo = false` y hace `join()`
- Implementar `SimuladorThread::iniciar()` y `SimuladorThread::detener()`: gestiona los hilos de todas las unidades registradas con `CentralDespacho`
- Proteger con `std::mutex` el acceso a `m_nodoActual`, `m_ruta` y `m_estado`

**Criterio de aceptación:** Se pueden crear 3 unidades, iniciar sus hilos, y detenerlos sin deadlock ni crash. `valgrind` no reporta race conditions.

---

### #11 — Productor-consumidor con condition_variable · tniia

**Archivos:** `src/threading/GeneradorIncidentes.h/cpp`, `src/core/CentralDespacho.h/cpp`

**Tareas:**
- Implementar `GeneradorIncidentes::iniciar()`: thread que cada N segundos llama `IncidenteFactory::crearAleatorio()` y lo pasa a `CentralDespacho::recibirIncidente()`
- Implementar `GeneradorIncidentes::detener()`: detiene el thread productor
- Completar `CentralDespacho::recibirIncidente(inc)`: encola el incidente y hace `m_cv.notify_one()`
- Completar `CentralDespacho::iniciar()`: loop consumidor que espera en `m_cv.wait()`, llama `procesarCola()` y repite mientras `m_activa`
- El acceso a `m_cola` debe estar protegido por `m_mutex`

**Criterio de aceptación:** El sistema genera incidentes automáticamente y los procesa sin race conditions. `m_cola` nunca crece indefinidamente si hay unidades libres.

---

### #12 — Patrón Observer Qt · KarlaBedregal

**Archivos:** `src/patterns/IObserver.h`, `src/patterns/IObservable.cpp`, `src/gui/MainWindow.h/cpp`, `src/gui/MapaWidget.h/cpp`

**Tareas:**
- Completar `IObservable::suscribir(observer)` y `IObservable::notificar(evento)`
- Hacer que `CentralDespacho` herede de `IObservable` y llame `notificar("incidente_asignado")` al despachar
- Hacer que `MainWindow` implemente `IObserver` y actualice la GUI al recibir una notificación
- La notificación debe cruzar el hilo de trabajo al hilo Qt usando `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`

**Criterio de aceptación:** Al asignar un incidente desde un hilo de fondo, la GUI se actualiza sin crash. No se llama directamente a widgets Qt desde hilos que no son el hilo principal.

---

## Sprint 3 — detalle de tareas

### #13 — Mapa animado en Qt · SharonCondoriUNSA

**Archivos:** `src/gui/MapaWidget.h/cpp`

**Tareas:**
- Dibujar los 20 nodos del `MapaArequipa` como círculos sobre un `QWidget` con `paintEvent`
- Dibujar las aristas como líneas entre nodos
- Mostrar nombre de zona al hacer hover sobre un nodo
- Animar las unidades moviéndose entre nodos (un punto coloreado que sigue la ruta)
- Actualizar con `QTimer` cada 200ms

**Criterio de aceptación:** Se ven los 20 nodos, las aristas y al menos 1 unidad moviéndose sobre el mapa.

---

### #14 — Íconos de incidentes y unidades · FernandoBarra05

**Archivos:** `assets/icons/`, `src/gui/MapaWidget.cpp`, `resources.qrc`

**Tareas:**
- Crear o añadir íconos SVG/PNG para: ambulancia, bombero, serenazgo, incendio, accidente, robo, urgencia médica
- Registrar todos en `resources.qrc`
- Renderizar el ícono correspondiente en el nodo del mapa cuando hay una unidad o incidente activo

**Criterio de aceptación:** El proyecto compila con `resources.qrc`. Los íconos se ven en el mapa.

---

### #15 — Panel de control y selector de estrategia · ISacck144

**Archivos:** `src/gui/PanelControl.h/cpp`

**Tareas:**
- `QComboBox` para seleccionar estrategia de despacho (Más cercana / Por prioridad / Balance de carga)
- Botón "Iniciar simulación" / "Detener"
- Contador en tiempo real: incidentes generados, atendidos, en cola
- Al cambiar la estrategia, llama `CentralDespacho::instancia().setEstrategia(...)`

**Criterio de aceptación:** Se puede cambiar la estrategia en caliente sin reiniciar. Los contadores se actualizan cada segundo.

---

### #16 — Estadísticas y comparación de estrategias · Daniel-IS04

**Archivos:** `src/stats/Estadisticas.h/cpp`, `src/gui/EstadisticasWidget.h/cpp`

**Tareas:**
- Registrar: tiempo promedio de respuesta por tipo de incidente, unidad más activa, estrategia con mejor tiempo medio
- Mostrar en `EstadisticasWidget` con `QChart` (gráfico de barras)
- Exportar resumen a texto plano

**Criterio de aceptación:** El gráfico muestra datos reales de la simulación. Se actualiza al final de cada ciclo.

---

### #17 — Historial y modelo de predicción · SharonCondoriUNSA

**Archivos:** `src/prediction/HistorialIncidentes.h/cpp`, `src/prediction/ModeloPrediccion.h/cpp`

**Tareas:**
- `HistorialIncidentes`: guarda los últimos N incidentes con timestamp, tipo y nodo
- `ModeloPrediccion::zonasDeMayorRiesgo()`: retorna los 3 nodos con más incidentes históricos
- Mostrar zonas de riesgo resaltadas en `MapaWidget`

**Criterio de aceptación:** Tras 20+ incidentes, el modelo identifica correctamente las zonas más frecuentes.

---

### #18 — Reubicación preventiva, diagramas UML y README · FernandoBarra05

**Archivos:** `src/core/CentralDespacho.cpp`, `docs/UML/`, `README.md`

**Tareas:**
- Implementar `CentralDespacho::reubicarUnidades()`: mueve unidades libres a nodos de alta predicción
- Actualizar los 3 diagramas PlantUML (clases, estados, secuencia) para reflejar el sistema final
- Actualizar el README con instrucciones de uso del simulador y capturas de pantalla

**Criterio de aceptación:** UML refleja el sistema real. README tiene sección "Cómo usar" con capturas.

---

## Flujo de trabajo

```
main  ←  develop  ←  feature/S2-B1-descripcion
```

```bash
# 1. Siempre partir de develop actualizado
git checkout develop
git pull origin develop

# 2. Crear rama con número de issue
git checkout -b feature/S2-B7-state    # cambia el número por el tuyo

# 3. Implementar, commitear
git commit -m "feat: implementar EstadoEnRuta con avance por ruta"

# 4. PR siempre hacia develop
git push origin feature/S2-B7-state
```

> PR hacia `develop` siempre. Con 1 aprobación se puede mergear.  
> **Nunca pushear directo a `main`.**

---

## Cómo compilar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target SIDEA_tests -j$(nproc)
```

## Correr tests

```bash
ctest --test-dir build --output-on-failure
```

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
│   └── prediction/    # HistorialIncidentes, ModeloPrediccion
├── tests/             # Tests unitarios (GoogleTest vía FetchContent)
├── docs/UML/          # Diagramas PlantUML
└── assets/icons/      # Íconos de unidades e incidentes
```

---

## Tablero Kanban

[github.com/users/KarlaBedregal/projects/4](https://github.com/users/KarlaBedregal/projects/4)

Mueve tu issue a **In Progress** cuando arranques y a **Done** cuando tu PR se mergee.
