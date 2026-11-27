# Validación de S3-B2 y S3-B6

Comprobado el 25 de noviembre de 2026 en Windows, con Qt 6.12.0 y MinGW GCC 13.1.0.

- Aplicación `SIDEA`, tests y utilidades de capturas/comparación compilados y enlazados.
- `resources.qrc` procesado por `rcc`; los siete PNG se cargan desde `:/icons/` y tienen 32×32 píxeles.
- **66 pruebas del núcleo y 4 de GUI aprobadas**. Incluyen recorrido preventivo por aristas, reserva e interrupción por emergencia, prioridad de pendientes, desactivación, reubicación al liberarse, métricas por modo, reinicio y cambios del toggle con hilos activos. En GUI se verifican los recursos, el render/ocultación de incidentes, la interpolación y los controles de inicio/pausa/reanudación/reinicio.
- Los tres `.puml` pasan `-checkonly` y generan SVG con PlantUML 1.2026.10. Los SVG incluidos corresponden a las fuentes finales.
- Capturas reales de widgets Qt revisadas en `docs/capturas/`, generadas por `SIDEA_capturas` con fuente Arial explícita en modo offscreen.
- `SIDEA_comparar` completa 24/24 incidentes en cada una de seis ejecuciones independientes. El CSV y la tabla del README coinciden.

## Particularidades de este entorno

CMake pudo configurar el proyecto con Qt y GoogleTest local, pero su etapa automática de generación falló al crear subprocesos (`libuv process spawn failed: operation not permitted`) y al interpretar la ruta con tildes desde MinGW Make. La compilación y ejecución local se verificaron invocando directamente `moc`, `uic`, `rcc`, GCC y el enlazador desde Python, con las listas de fuentes de los mismos `CMakeLists.txt`.

Se repitió la comprobación normal en `build/verificacion` y persistieron ambos errores. Se añadió `tools/verificar_cmake.ps1` para ejecutarla desde PowerShell normal de Windows: copia las fuentes a una carpeta temporal sin tildes, configura y compila con CMake, ejecuta CTest, el comparador y las capturas, y guarda un registro en `build/verificacion-cmake-*.log`. El usuario ejecutó el script correctamente; el registro `build/verificacion-cmake-20261125-153729.log` confirma `COMPROBACION CORRECTA`.

El script de esa verificación y los resultados XML permanecen en `build/manual_build.py` y `build/manual/`; son artefactos locales ignorados por Git. El ejecutable `build/manual/SIDEA.exe` incluye en esa carpeta sus DLL de ejecución y los plugins de plataforma para poder abrirlo localmente. La configuración de CI se amplió para compilar aplicación, tests de GUI y utilidades mediante CMake en Ubuntu; ese CI remoto no se ha ejecutado desde esta sesión.

La carpeta `.git` tiene permiso de solo lectura para el agente. El usuario creó `feature/S3-B2-S3-B6-fernando` desde `origin/develop` y recuperó los cambios mediante stash. Se combinaron las declaraciones de `CentralDespacho.h` y la predicción positiva con desempate por nodo de `ModeloPrediccion.cpp`, y se eliminó una definición duplicada de `getIncidentesPendientes()` producida por la combinación automática. El ciclo de hilos de la ventana usa la central como única dueña de los simuladores. La resolución debe registrarse con `git add` desde la terminal del usuario antes del commit.
