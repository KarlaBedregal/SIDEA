<!-- PÁGINA 1: CARÁTULA — completar por el equipo antes de exportar a PDF -->
<!-- Incluir: nombre del proyecto, integrantes, curso, docente, fecha -->

---

<!-- Salto de página: en LibreOffice Writer insertar "Salto de página" antes del título siguiente -->

# SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa
## Informe Técnico de Cierre de Proyecto

**Asignatura:** Tecnología de Objetos  
**Repositorio:** https://github.com/KarlaBedregal/SIDEA  
**Fecha:** Noviembre 2026

---

## 1. Introducción

SIDEA (Sistema Inteligente de Despacho de Emergencias de Arequipa) es un simulador de despacho de emergencias que modela la coordinación de unidades de respuesta —ambulancias, bomberos y serenazgo— sobre un grafo de 20 zonas de la ciudad de Arequipa. El sistema replica el flujo operativo de una central de despacho real: recibe incidentes de forma asíncrona, selecciona la unidad más adecuada según el criterio activo, la envía por la ruta óptima calculada con el algoritmo de Dijkstra, y la registra como libre al completar su ciclo de atención.

El proyecto nació de la necesidad de disponer de una herramienta que permita comparar distintas estrategias de asignación de recursos de emergencia de forma objetiva y reproducible, sin depender de datos confidenciales del sistema real. SIDEA permite cambiar la estrategia de despacho en tiempo de ejecución, activar o desactivar la predicción de zonas de riesgo, y observar el comportamiento del sistema mediante una interfaz gráfica animada con actualización en tiempo real.

Desde el punto de vista académico, el proyecto integra los contenidos del curso: programación orientada a objetos, patrones de diseño (Observer, Strategy, Factory Method, State, Singleton), concurrencia con `std::thread`, comunicación entre hilos mediante `condition_variable`, plantillas (templates) de C++17, y desarrollo de interfaces gráficas con Qt6.

---

## 2. Análisis del Problema

### 2.1 Problema concreto

La gestión manual de emergencias en ciudades medianas como Arequipa enfrenta dos dificultades operativas principales. La primera es la **asignación ineficiente de unidades**: elegir qué recurso enviar a cada incidente según distancia, tipo de unidad o carga de trabajo es una decisión que debe tomarse en segundos y varía según el contexto. La segunda es la **falta de previsión geográfica**: sin un registro histórico de dónde ocurren los incidentes, las unidades libres permanecen en sus bases incluso si existe una zona con alta frecuencia de llamadas.

### 2.2 Por qué es difícil de resolver

El problema no es solo algorítmico. El sistema tiene que manejar incidentes que llegan de forma asíncrona (en cualquier momento y desde cualquier zona), unidades que se desplazan físicamente por el grafo en su propio hilo de ejecución, y una interfaz gráfica que debe reflejar todos esos cambios en tiempo real sin bloquearse. Coordinar estos tres flujos concurrentes —generador de incidentes, motor de despacho, y GUI de Qt— sin condiciones de carrera ni interbloqueos es el desafío central de ingeniería del proyecto.

### 2.3 Solución adoptada

Se modeló el sistema como un grafo ponderado de 20 nodos (zonas de Arequipa) con aristas bidireccionales. Cada unidad corre en su propio hilo y avanza nodo a nodo siguiendo la ruta calculada por Dijkstra. La `CentralDespacho` actúa como coordinador central con una cola de incidentes pendientes: un hilo consumidor espera mediante `condition_variable` hasta que llega un incidente, selecciona la mejor unidad disponible aplicando la estrategia activa, y la asigna. La GUI de Qt observa los eventos de despacho mediante el patrón Observer y actualiza el mapa animado y las estadísticas en el hilo principal de Qt, usando `QMetaObject::invokeMethod` con `Qt::QueuedConnection` para cruzar el límite entre hilos de forma segura.

La predicción de zonas de riesgo se implementó con `HistorialIncidentes`, que acumula los últimos 1000 registros con timestamp y frecuencia por zona. Cuando la reubicación preventiva está activa, las unidades libres se desplazan hacia la zona de mayor riesgo histórico como recorrido secundario interrumpible: cualquier despacho real tiene prioridad y cancela el recorrido preventivo inmediatamente.

---

## 3. Conceptos de POO Aplicados

### 3.1 Clases abstractas y herencia

**`Unidad` (clase abstracta base)**  
Define la interfaz común de toda unidad de respuesta. Declara dos métodos puramente virtuales:

```cpp
virtual void atenderIncidente(const Incidente& inc) = 0;
virtual std::string getTipoUnidad() const = 0;
```

Las clases concretas `Ambulancia`, `Bombero` y `Serenazgo` heredan de `Unidad` e implementan estos métodos según el tipo de servicio que prestan. `Unidad` gestiona el ciclo de vida completo (hilo, estado, ruta, métricas) de forma común a los tres tipos.

**`EstadoUnidad` (clase abstracta del patrón State)**  
Define la interfaz de los cuatro estados del ciclo de una unidad:

```cpp
virtual void manejar(Unidad& unidad) = 0;
virtual std::string nombre() const = 0;
```

Las subclases concretas son `EstadoLibre`, `EstadoEnRuta`, `EstadoAtendiendo` y `EstadoRegresando`.

**`EstrategiaDespacho` (clase abstracta del patrón Strategy)**  
Define la interfaz de selección de unidades:

```cpp
virtual std::shared_ptr<Unidad> seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente& incidente) = 0;
virtual std::string nombre() const = 0;
```

Las tres estrategias concretas implementan este contrato con criterios distintos.

**`IObserver` e `IObservable`**  
Definen el contrato del patrón Observer. `IObservable` mantiene internamente la lista de suscriptores con un `std::mutex` propio para acceso thread-safe. `CentralDespacho` hereda de `IObservable` y llama a `notificar()` cada vez que ocurre un evento de despacho.

### 3.2 Polimorfismo

El polimorfismo en tiempo de ejecución se usa en tres lugares clave:

- `CentralDespacho` llama `m_estrategia->seleccionar(...)` sin conocer qué implementación concreta está activa. Cambiar la estrategia en caliente no requiere modificar la central.
- `Unidad::tick()` delega a `m_estado->manejar(*this)`. El estado actual determina el comportamiento: avanzar en ruta, atender el incidente o regresar a base.
- El array de unidades `std::vector<std::shared_ptr<Unidad>>` almacena punteros base; cada `tick()` invoca el método correcto según la subclase concreta.

### 3.3 Templates (plantillas)

La estructura de datos del grafo y el algoritmo de Dijkstra se implementaron como plantillas C++17, desacoplando la topología del grafo de los datos asociados a cada arista:

```cpp
template<typename T>
class Grafo { ... };

template<typename T>
ResultadoDijkstra dijkstra(const Grafo<T>& grafo, int origen);
```

`MapaArequipa` instancia `Grafo<std::string>` para asociar nombres de zona a las aristas. La cola de prioridad del simulador también se implementó como plantilla (`ColaPrioridad<T>`), permitiendo su reutilización con distintos tipos de elemento y comparadores.

La implementación completa de los templates reside en los archivos `.h` (no en `.cpp`) porque el compilador C++ necesita ver la implementación en el momento de instanciar el template para cada tipo `T`.

### 3.4 Smart pointers y gestión de recursos

Se usaron smart pointers de forma sistemática para evitar leaks de memoria y expresar claramente la semántica de propiedad:

- `std::unique_ptr<EstadoUnidad> m_estado` en `Unidad`: cada unidad es propietaria exclusiva de su estado; al cambiar de estado el destructor del anterior se llama automáticamente.
- `std::unique_ptr<EstrategiaDespacho> m_estrategia` en `CentralDespacho`: la central es propietaria de la estrategia activa; `setEstrategia()` reemplaza la anterior sin leak.
- `std::shared_ptr<Unidad>` e `std::shared_ptr<Incidente>` para los objetos compartidos entre la central, el hilo generador y la GUI.

### 3.5 Concurrencia

El sistema gestiona cuatro fuentes de concurrencia:

| Componente | Mecanismo |
|---|---|
| `GeneradorIncidentes` | `std::thread` + `std::condition_variable` para pausa/reanudación limpia |
| `SimuladorThread` (un hilo por unidad) | `std::thread` + `std::atomic<bool> m_activo` |
| `CentralDespacho` hilo consumidor | `std::thread` + `std::condition_variable m_cv` bloqueante hasta nuevo incidente |
| Acceso compartido a datos | `std::mutex m_mutex` en la central, `std::recursive_mutex m_mutex` en cada unidad |

La notificación a la GUI desde hilos secundarios se realiza mediante `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`, que encola el evento en el bucle de eventos de Qt y lo ejecuta en el hilo principal, evitando accesos directos a widgets desde hilos secundarios.

### 3.6 Enumeraciones con clase (enum class)

Se usaron `enum class` para los tipos con dominio finito, eliminando conversiones implícitas y nombres globales contaminantes:

```cpp
enum class TipoIncidente    { INCENDIO, ACCIDENTE, ROBO, URGENCIA_MEDICA };
enum class TipoUnidad       { AMBULANCIA, BOMBERO, SERENAZGO };
enum class PrioridadIncidente { BAJA = 1, MEDIA = 2, ALTA = 3, CRITICA = 4 };
```

Los valores de `PrioridadIncidente` tienen asignación explícita para facilitar la comparación directa en la cola de prioridad.

### 3.7 Manejo de errores

Se lanza `std::out_of_range` en `Grafo<T>::vecinos()` si se consulta un nodo inexistente, y `std::invalid_argument` en `UnidadFactory::crear()` si el tipo de unidad no es válido. Las capas superiores capturan estas excepciones para garantizar que un nodo mal configurado no derribe el hilo completo de la central.

---

## 4. Patrones de Diseño Aplicados

### 4.1 Singleton — `CentralDespacho`

**Dónde:** `CentralDespacho::instancia()` en `src/core/CentralDespacho.h`.

**Implementación:**

```cpp
class CentralDespacho : public IObservable {
public:
    static CentralDespacho& instancia();
    CentralDespacho(const CentralDespacho&) = delete;
    CentralDespacho& operator=(const CentralDespacho&) = delete;
private:
    CentralDespacho() = default;
};
```

El constructor es privado y el operador de copia/asignación está eliminado explícitamente. `instancia()` devuelve una referencia a la única instancia estática.

**Justificación:**  
La central de despacho es el estado global del sistema. Si existieran múltiples instancias, las unidades registradas en una no serían visibles para la otra, y los incidentes generados en un hilo podrían terminar en una cola que nadie consume. El Singleton garantiza que toda la GUI, el generador de incidentes y los hilos de unidades comparten exactamente el mismo estado. En términos de **mantenibilidad**, cualquier componente accede a `CentralDespacho::instancia()` sin necesidad de pasar referencias por capas de constructores, lo que simplifica el acoplamiento estructural del sistema.

### 4.2 Observer — `IObserver` / `IObservable` → `MainWindow`

**Dónde:** `src/patterns/IObserver.h`, `src/patterns/IObservable.h/cpp`, `src/gui/MainWindow.cpp`.

**Implementación:**  
`CentralDespacho` hereda de `IObservable`. Cuando despacha una unidad o registra un incidente atendido, llama a `notificar("unidad_asignada")` o `notificar("incidente_atendido")`. `IObservable::notificar()` copia el vector de observadores bajo mutex y lo recorre fuera del lock para evitar interbloqueos si un observador intenta suscribirse durante la notificación.

En `MainWindow`, el adaptador `MainWindowObserver` implementa `IObserver::actualizar()` y reenvía cada evento al hilo de Qt:

```cpp
void actualizar(const std::string& evento) override {
    QMetaObject::invokeMethod(m_window, "onEventoDespacho",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(evento)));
}
```

**Justificación:**  
Sin Observer, `CentralDespacho` necesitaría una referencia directa a `MainWindow` para actualizar la GUI, creando un acoplamiento que violaría el principio de responsabilidad única. Con Observer, `CentralDespacho` desconoce quién la observa: se puede agregar un segundo observador (por ejemplo, un logger de archivo) sin modificar una sola línea de la central. Esto representa una ventaja concreta de **flexibilidad**: el sistema puede tener cero, uno o varios observadores sin que la clase publicadora cambie. La implementación thread-safe (copia del vector + invokeMethod con QueuedConnection) garantiza que los eventos emitidos desde hilos secundarios nunca tocan widgets directamente.

### 4.3 Strategy — `EstrategiaDespacho`

**Dónde:** `src/strategies/EstrategiaDespacho.h` y las tres implementaciones en `src/strategies/`.

**Implementación:**

```cpp
class EstrategiaDespacho {
public:
    virtual std::shared_ptr<Unidad> seleccionar(
        const std::vector<std::shared_ptr<Unidad>>& unidades,
        const Incidente& incidente) = 0;
    virtual std::string nombre() const = 0;
};
```

Las tres estrategias concretas son:
- `EstrategiaMasCercana`: corre Dijkstra desde cada unidad libre y selecciona la de menor distancia al nodo del incidente.
- `EstrategiaPrioridad`: selecciona la unidad del tipo preferido para el incidente (Ambulancia→urgencia médica, Bombero→incendio/accidente, Serenazgo→robo). Si no hay coincidencia, devuelve cualquier unidad libre.
- `EstrategiaBalanceCarga`: selecciona la unidad libre con menor número de incidentes atendidos acumulados.

**Justificación:**  
Sin Strategy, la lógica de los tres algoritmos estaría anidada en un `switch` dentro de `CentralDespacho::despacharUno()`. Agregar una cuarta estrategia requeriría modificar la central, violando el principio abierto/cerrado. Con Strategy, agregar `EstrategiaZonasAdyacentes` implica crear una nueva clase que implementa `EstrategiaDespacho` sin tocar el resto del sistema. En términos de **escalabilidad**, el diseño soporta nuevas estrategias en tiempo de ejecución: el usuario puede cambiar la estrategia desde el `QComboBox` del panel de control mientras la simulación está en marcha, y la central aplica el nuevo criterio desde el siguiente incidente sin reiniciar.

### 4.4 Factory Method — `UnidadFactory` e `IncidenteFactory`

**Dónde:** `src/core/UnidadFactory.h/cpp` y `src/core/IncidenteFactory.h/cpp`.

**Implementación:**

```cpp
class UnidadFactory {
public:
    static std::shared_ptr<Unidad> crear(int id, TipoUnidad tipo,
                                          const std::string& nombre, int nodoBase);
};
```

`crear()` instancia la subclase correcta (`Ambulancia`, `Bombero` o `Serenazgo`) y devuelve un `shared_ptr<Unidad>`. Si el tipo no es válido lanza `std::invalid_argument`. `IncidenteFactory::crearAleatorio()` genera un `Incidente` con tipo y prioridad aleatorios, nodo aleatorio dentro del rango válido del grafo, y un ID autoincremental.

**Justificación:**  
Sin Factory, el código de inicialización de `MainWindow` y los tests unitarios necesitarían conocer los constructores concretos de cada subclase de `Unidad`. Si se agrega un nuevo tipo (por ejemplo, `Dron`), habría que modificar todos los puntos de creación. Con Factory, ese conocimiento está centralizado en un único lugar. En términos de **reutilización**, los tests de estrategias crean escenarios completos con `UnidadFactory::crear(...)` sin saber que internamente se usa `std::make_shared<Ambulancia>(...)`, y el generador de incidentes usa `IncidenteFactory::crearAleatorio()` sin conocer los posibles tipos de incidente.

### 4.5 State — `EstadoUnidad`

**Dónde:** `src/core/states/` con `EstadoUnidad.h` y cuatro implementaciones concretas.

**Implementación:**  
Cada ciclo de ejecución, `Unidad::tick()` delega en `m_estado->manejar(*this)`:

- `EstadoLibre::manejar`: no hace nada. Si hay reubicación preventiva activa, la unidad puede iniciar un recorrido secundario al nodo de mayor riesgo.
- `EstadoEnRuta::manejar`: avanza un nodo en la ruta Dijkstra hacia el incidente. Al llegar al nodo destino, cambia el estado a `EstadoAtendiendo`.
- `EstadoAtendiendo::manejar`: llama a `atenderIncidente()` sobre el incidente asignado. Registra el incidente en el historial y cambia a `EstadoRegresando`.
- `EstadoRegresando::manejar`: avanza un nodo hacia la base original. Al llegar, llama a `CentralDespacho::notificarUnidadLibre()` y cambia a `EstadoLibre`.

**Justificación:**  
Sin State, `Unidad::tick()` contendría un bloque `if/else` anidado que crece con cada nuevo estado. Los estados son mutuamente excluyentes y cada uno tiene su propia lógica de transición; modelarlos como clases separadas hace que agregar un nuevo estado (por ejemplo, `EstadoMantenimiento`) requiera solo crear una nueva subclase sin tocar las existentes. En términos de **mantenibilidad**, cada archivo de estado es independiente: modificar cómo la unidad avanza en ruta no afecta a cómo atiende el incidente ni a cómo regresa a su base.

---

## 5. Conclusiones

### 5.1 Qué se aprendió

El desarrollo de SIDEA evidenció que la separación entre interfaz pública (archivos `.h`) e implementación (archivos `.cpp`) no es solo una convención de estilo sino una herramienta de diseño. Gracias a esa separación fue posible trabajar en seis ramas simultáneas durante los tres sprints sin que los cambios de un módulo rompieran los demás: `CentralDespacho.h` define el contrato que todos los demás usan, y los cambios a `CentralDespacho.cpp` no requerían recompilar módulos que no dependían de ellos.

La concurrencia enseñó que los problemas más difíciles de depurar no son los errores de lógica sino las condiciones de carrera que solo aparecen bajo carga. Usar `std::atomic<bool>` para las banderas de control, `std::mutex` para secciones críticas y `condition_variable` para esperas bloqueantes (en lugar de `sleep` o busy-wait) resultó ser la diferencia entre un programa que funciona el 95% de las veces y uno que funciona siempre.

La integración de Qt con código C++ estándar mostró que el mecanismo de señales y slots de Qt resuelve elegantemente el problema de la comunicación entre hilos y GUI: `Qt::QueuedConnection` garantiza que el slot se ejecute en el hilo correcto sin que el emisor deba conocer el hilo del receptor.

### 5.2 Dificultades y cómo se resolvieron

**Duplicados en merges de ramas concurrentes.** Al integrar las ramas del Sprint 3 aparecieron definiciones duplicadas de funciones en `CentralDespacho.cpp` porque dos ramas habían añadido la misma función de forma independiente. Se resolvió con una revisión explícita de los bloques duplicados y se documentó como regla de integración para sprints futuros.

**El operador de asignación eliminado por `const std::size_t`.** `HistorialIncidentes` tiene un miembro `const std::size_t m_capacidad` y un `std::mutex m_mutex`. Ambos eliminan implícitamente el operador de asignación de copia, lo que hizo fallar la línea `m_historial = HistorialIncidentes{}` en `CentralDespacho::reiniciar()`. La solución fue reemplazar la asignación por `m_historial.limpiar()`, que reinicia el contenido sin mover ni copiar el objeto.

**Tests GUI con timer de 200ms en entorno offscreen.** Los tests de animación esperaban que el timer de `MapaWidget` disparara dentro de los 60ms de timeout del test, pero el timer estaba configurado a 200ms. Se corrigió reduciendo el intervalo del timer de animación a 16ms (60fps) y controlando la duración de los movimientos con `m_duracionMovimientoMs`, que los tests pueden establecer mediante `setDuracionMovimiento()`.

**Observer con doble suscripción.** Un merge creó dos llamadas a `CentralDespacho::instancia().suscribir(obs)` en el constructor de `MainWindow`, con la consecuencia de que cada evento llegaba duplicado a la GUI. Se detectó analizando el constructor resultante del merge y se eliminó la suscripción extra.

**Sincronización entre hilo de despacho y hilo de Qt.** Llamar directamente a un método de `MainWindow` desde el `MainWindowObserver` (que se ejecuta en el hilo de la central) provocaba accesos no sincronizados a widgets Qt. Se resolvió con `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`, que encola la llamada en el event loop de Qt y la ejecuta en el hilo principal.

### 5.3 Reflexión sobre los patrones de diseño

Los cinco patrones aplicados no fueron una lista de requisitos que se cumplió mecánicamente: cada uno resolvió un problema concreto del diseño.

Observer desacoplóa la lógica de negocio de la capa de presentación. Sin él, `CentralDespacho` habría necesitado una referencia directa a `MainWindow`, haciendo imposible testear la central sin construir una ventana. Con Observer, los tests unitarios del núcleo no saben que existe Qt.

Strategy permitió que el comportamiento de asignación se convirtiera en un dato del sistema: el usuario elige el algoritmo desde la interfaz, y el motor de despacho simplemente lo usa. Esto trasladó una decisión de programación a una decisión de operación.

State hizo que el ciclo de vida de una unidad fuera legible. Ver cuatro clases con nombres claros (`EstadoEnRuta`, `EstadoAtendiendo`) es más fácil de entender y depurar que un entero `m_estado` y un `switch` de cuatro ramas en `tick()`.

Factory centralizó el conocimiento de qué subclases existen. Durante el desarrollo, cuando se cambió la firma del constructor de `Ambulancia`, solo fue necesario actualizar `UnidadFactory::crear()` y no los cuatro o cinco lugares donde se instanciaban ambulancias.

Singleton evitó pasar `CentralDespacho&` como parámetro a través de cinco capas de constructores. Es el patrón más discutido en la literatura porque dificulta el testing de aislamiento, pero en un sistema de simulación con un único estado global compartido por definición, su uso está justificado: se compensó verificando que el constructor privado y la eliminación del operador de copia impiden la creación accidental de una segunda instancia.
