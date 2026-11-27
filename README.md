# SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa

Simulador de despacho de emergencias para Arequipa. C++17 + Qt6.  
Curso de Programación Orientada a Objetos — UNSA 2026-B.

---

## Cómo usar

1. Compila el ejecutable `SIDEA` siguiendo las instrucciones al final de este README y ejecútalo con `./build/SIDEA` (Windows: `.\build\SIDEA.exe`; con Visual Studio, usa la carpeta de la configuración elegida).
2. El escenario inicial tiene ocho unidades en sus bases. Selecciona **Más Cercana**, **Prioridad** o **Balance de Carga** y la duración de un tick (100–5000 ms).
3. Pulsa **Iniciar**. Se genera un incidente cada dos segundos. La central despacha unidades compatibles en hilos propios; el mapa se refresca e interpola el movimiento entre nodos.
4. **Pausar** conserva las unidades, la cola y el historial. **Reanudar** continúa el ciclo; puedes cambiar la velocidad durante la pausa. **Detener** conserva también el escenario para poder inspeccionarlo y continuar con **Iniciar**. **Nuevo escenario** detiene los hilos y limpia incidentes, historial y métricas.
5. Activa **Reubicación preventiva** para que las unidades libres recorran la ruta Dijkstra a la zona de mayor riesgo histórico. Sin historial no se mueven. El despacho de un incidente compatible tiene precedencia e interrumpe la reubicación. Desactivar el interruptor detiene el recorrido preventivo en el nodo actual. La base original se conserva y las unidades ocupadas completan su ciclo normal.
6. Cambia la estrategia o el interruptor durante la ejecución. La tabla agrupa los despachos por **estrategia y predicción al momento de asignar**. **Exportar comparación CSV** guarda esos resultados; **Nuevo escenario** los borra.

Los vehículos identifican ambulancias, bomberos y serenazgo; debajo aparece su ID. Los incidentes usan cruz roja (urgencia médica), llama naranja (incendio), triángulo amarillo (accidente) y escudo azul (robo). Se muestran tanto los pendientes en cola como los asignados, hasta completar su atención. Pasa el cursor sobre una zona para ver su nombre.

### Capturas reales

Widgets Qt del ejecutable `SIDEA_capturas`, con un escenario fijo de cuatro tipos de incidente:

![Incidentes pendientes e íconos de las ocho unidades](docs/capturas/incidentes.png)

Después de completar dos ciclos de demostración, uno por modo, se muestran los resultados y las unidades reubicadas:

![Reubicación activada y comparación de resultados](docs/capturas/comparacion.png)

Las capturas se generan avanzando el motor por ticks desde `examples/capturas.cpp`; sus tiempos de ejecución acelerada no son un benchmark. Para regenerarlas:

```bash
QT_QPA_PLATFORM=offscreen ./build/SIDEA_capturas docs/capturas
```

En Windows con Qt offscreen se puede pasar una fuente instalada explícitamente:

```powershell
$env:QT_QPA_PLATFORM = "offscreen"
.\build\SIDEA_capturas.exe docs/capturas C:/Windows/Fonts/arial.ttf
Remove-Item Env:QT_QPA_PLATFORM
```

### Comparación de estrategias y predicción

| Estrategia | Selección entre unidades compatibles libres |
|---|---|
| Más cercana | Menor distancia ponderada al incidente, calculada por Dijkstra. |
| Prioridad | Primera unidad del tipo preferido. La central ya ordena la cola por prioridad descendente y conserva FIFO en empates, para las tres estrategias. |
| Balance de carga | Menor número de incidentes atendidos. |

La tabla de la interfaz cuenta asignaciones, llegadas y atenciones. La media es el tiempo real en segundos desde la creación del incidente hasta que una unidad llega al nodo, incluida la espera en cola. Sin llegadas se muestra `—`; los casos aún en ruta no entran en la media. Las pausas también cuentan en ese reloj. Para una comparación controlada entre modos, usa el escenario reproducible siguiente.

`SIDEA_comparar` reinicia completamente el escenario para cada combinación: ocho unidades en las mismas bases, 24 incidentes, uno cada 12 ticks; 18 en Cerro Colorado (nodo 3) y seis en Sachaca (nodo 10). Tipo y prioridad siguen la misma secuencia en las seis ejecuciones. La predicción aprende durante cada ejecución, sin historial precargado. La respuesta se mide en ticks globales, incluida la espera en cola, sin depender del rendimiento del equipo.

| Estrategia | Predicción | Incidentes atendidos | Respuesta media (ticks) |
|---|---|---:|---:|
| Más cercana | No | 24/24 | 2.250 |
| Más cercana | Sí | 24/24 | 2.000 |
| Prioridad | No | 24/24 | 3.250 |
| Prioridad | Sí | 24/24 | 2.042 |
| Balance de carga | No | 24/24 | 3.083 |
| Balance de carga | Sí | 24/24 | 2.042 |

Resultados generados por `examples/comparar_prediccion.cpp`, disponibles en [CSV](docs/comparacion_prediccion.csv). Este escenario concentra la demanda y favorece la prevención; los resultados no garantizan una mejora para otras distribuciones. En la simulación existente cada arista se recorre en un tick; Dijkstra elige la ruta usando sus pesos.

```bash
./build/SIDEA_comparar > docs/comparacion_prediccion.csv
```

El modelo asigna un puntaje `0.6 × frecuenciaZona / total + 0.4 × frecuenciaZonaHora / total`, usando la hora UTC actual. Solo considera nodos válidos con puntaje positivo; los empates favorecen el menor ID de nodo. El historial pertenece a la central y se consulta bajo su mutex. La prevención envía las unidades libres a una misma zona de mayor riesgo, según el requisito del issue #18.

### Diagramas UML

Fuentes y versiones SVG generadas del sistema implementado:

- [Clases](docs/UML/clases.puml) · [SVG](docs/UML/SIDEA_Clases.svg)
- [Estados](docs/UML/estado_unidad.puml) · [SVG](docs/UML/Estado_Unidad.svg)
- [Secuencia de despacho](docs/UML/secuencia_despacho.puml) · [SVG](docs/UML/Secuencia_Despacho.svg)

Con Java y un JAR de PlantUML local:

```bash
java -jar plantuml.jar -charset UTF-8 -checkonly docs/UML/*.puml
java -jar plantuml.jar -charset UTF-8 -tsvg docs/UML/*.puml
```

Los diagramas de clases y estados usan Smetana para evitar la dependencia externa de Graphviz. Los submodos de espera y reubicación pertenecen a `EstadoLibre`; no añaden un quinto estado al ciclo de atención.

---

## Tareas por persona

Cada integrante tiene issues asignados por sprint (sorteo). Búscalos en la pestaña [Issues](../../issues) filtrados por tu usuario.

| Integrante | Sprint 1 | Sprint 2 | Sprint 3 |
|------------|----------|----------|----------|
| **KarlaBedregal** | [#3 Dijkstra + ColaPrioridad](../../issues/3) ✅ | [#10 Threads](../../issues/10) ✅ · [#12 Observer Qt](../../issues/12) ✅ | — |
| **tniia** | [#2 Grafo + Unidad](../../issues/2) ✅ | [#7 Patrón State](../../issues/7) ✅ · [#11 Productor-consumidor](../../issues/11) ✅ | — |
| **ISacck144** | [#4 Mapa + CentralDespacho](../../issues/4) ✅ | [#8 Estrategia más cercana](../../issues/8) ✅ | [#15 Panel de control](../../issues/15) |
| **Daniel-IS04** | [#1 Setup + Incidente](../../issues/1) ✅ | [#9 Estrategias prioridad/balance](../../issues/9) ✅ | [#16 Estadísticas](../../issues/16) |
| **SharonCondoriUNSA** | [#5 Subclases Unidad](../../issues/5) ✅ | — | [#13 Mapa Qt animado](../../issues/13) ✅ · [#17 Historial + predicción](../../issues/17) |
| **FernandoBarra05** | [#6 Factories + tests](../../issues/6) ✅ | — | [#14 Íconos + movimiento](../../issues/14) · [#18 UML + docs](../../issues/18) |

> ✅ = mergeado. Abre tu issue antes de empezar y muévelo a "In Progress" en el Kanban.

---

## Sprints

| Sprint | Fechas | Enfoque | Estado |
|--------|--------|---------|--------|
| **Sprint 1** | 7 nov | Compilación, estructura base y tests | ✅ Completo |
| **Sprint 2** | 8 – 25 nov | Lógica de despacho + concurrencia | ✅ Completo |
| **Sprint 3** | 25 nov – 5 dic | GUI animada + estadísticas + predicción | En curso |

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

**Criterio de aceptación:** Se pueden crear 3 unidades, iniciar sus hilos, y detenerlos sin deadlock ni crash. ThreadSanitizer reporta 0 data races.

---

### #11 — Productor-consumidor con condition_variable · tniia

**Archivos:** `src/threading/GeneradorIncidentes.h/cpp`, `src/core/CentralDespacho.h/cpp`

**Tareas:**
- Implementar `GeneradorIncidentes::iniciar()`: thread que cada N ms llama `IncidenteFactory::crearAleatorio()` y lo pasa a `CentralDespacho::recibirIncidente()`
- Implementar `GeneradorIncidentes::detener()`: detiene el thread productor con parada inmediata via `condition_variable`
- `CentralDespacho::recibirIncidente(inc)`: encola el incidente y hace `m_cv.notify_one()`
- Hilo consumidor interno (`bucleConsumo`) que espera en `m_cv.wait()`, despacha y repite mientras `m_activa`
- El acceso a `m_cola` debe estar protegido por `m_mutex`

**Criterio de aceptación:** Productor y consumidor en paralelo sin pérdida de incidentes y sin busy-wait. `detener()` limpio en menos de 250ms.

---

### #12 — Patrón Observer Qt · KarlaBedregal

**Archivos:** `src/patterns/IObserver.h`, `src/patterns/IObservable.h/cpp`, `src/gui/MainWindow.h/cpp`

**Tareas:**
- `IObservable` thread-safe: mutex propio, copia local en `notificar()` para evitar deadlock
- `CentralDespacho` hereda de `IObservable` y emite `incidente_recibido`, `unidad_asignada`, `unidad_libre`
- `MainWindow` escucha via `MainWindowObserver` (adapter) con `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`
- Las notificaciones desde hilos secundarios nunca tocan widgets directamente

**Criterio de aceptación:** Al asignar un incidente desde un hilo de fondo, la GUI se actualiza sin crash. Sin segfault al detener.

---

## Sprint 3 — detalle de tareas

### #13 — Mapa animado en Qt · SharonCondoriUNSA ✅

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

**Archivos:** `assets/icons/`, `src/gui/MapaWidget.cpp`, `src/gui/MainWindow.cpp`, `resources.qrc`, `CMakeLists.txt`

**Tareas:**
- Implementado: siete PNG RGBA de 32×32 y recursos Qt compilados con AUTORCC.
- Implementado: incidentes pendientes/asignados visibles hasta completar atención y unidades con interpolación cada 16 ms.
- Verificación: carga de los siete recursos, render de los cuatro tipos de incidente, ocultación al atender y movimiento interpolado.

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

**Archivos:** `src/core/`, `src/prediction/ModeloPrediccion.*`, `src/gui/`, `docs/UML/`, `README.md`

**Tareas:**
- Implementado: `CentralDespacho::reubicarUnidades()`, toggle en UI, recorrido preventivo interrumpible y comparación por estrategia/modo con exportación CSV.
- Implementado: los tres diagramas PlantUML, sus SVG, instrucciones de uso, capturas reales y comparación reproducible de seis configuraciones.
- Verificación: ruta por aristas, prioridad de atención, apagado del toggle, interrupción por despacho, reubicación al liberarse, concurrencia, limpieza y métricas por modo.

**Criterio de aceptación:** UML refleja el sistema real. README tiene sección "Cómo usar" con capturas.

---

## Flujo de trabajo

```
main  ←  develop  ←  feature/SX-BX-descripcion
```

```bash
# 1. Siempre partir de develop actualizado
git checkout develop
git pull origin develop

# 2. Crear rama con número de issue
git checkout -b feature/S3-B2-iconos    # cambia el número por el tuyo

# 3. Implementar, commitear
git commit -m "feat: agregar íconos de unidades al mapa"

# 4. PR siempre hacia develop
git push origin feature/S3-B2-iconos
```

> PR hacia `develop` siempre. Con 1 aprobación se puede mergear.  
> **Nunca pushear directo a `main`.**

---

## Cómo compilar

Requiere CMake ≥ 3.20, compilador C++17 y Qt6 con Core, Widgets y Charts. La primera configuración descarga GoogleTest 1.14.0 y necesita acceso a GitHub.

En Ubuntu (las mismas dependencias utilizadas en CI):

```bash
sudo apt-get install cmake build-essential qt6-base-dev libqt6charts6-dev libgl1-mesa-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/SIDEA
```

En Windows usa un compilador compatible con tu instalación de Qt. Ejemplo con Qt MinGW; adapta las rutas y versiones a tu equipo:

```powershell
$env:Path = "C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.12.0\mingw_64\bin;" + $env:Path
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64
cmake --build build --parallel 4
.\build\SIDEA.exe
```

## Correr tests

```bash
ctest --test-dir build --output-on-failure
```

`SIDEA_tests` cubre el núcleo; `SIDEA_gui_tests` carga los recursos y verifica el render, la interpolación y los controles con `QT_QPA_PLATFORM=offscreen`. CI compila además la aplicación y los ejecutables de capturas y comparación. Para regenerar los siete íconos propios: `python tools/generar_iconos.py` (requiere Pillow; no hace falta para compilar los PNG ya incluidos).

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
