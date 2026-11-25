add_test([=[GrafoTest.AgregarNodosYAristas]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=GrafoTest.AgregarNodosYAristas]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GrafoTest.AgregarNodosYAristas]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_grafo.cpp:5]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[GrafoTest.NodoInexistenteLanzaExcepcion]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=GrafoTest.NodoInexistenteLanzaExcepcion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GrafoTest.NodoInexistenteLanzaExcepcion]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_grafo.cpp:23]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[GrafoTest.TotalNodos]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=GrafoTest.TotalNodos]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GrafoTest.TotalNodos]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_grafo.cpp:28]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[GrafoTest.AristaDirigidaRegistraAmbosExtremos]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=GrafoTest.AristaDirigidaRegistraAmbosExtremos]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GrafoTest.AristaDirigidaRegistraAmbosExtremos]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_grafo.cpp:36]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[GrafoTest.AgregarNodoRepetidoConservaAristas]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=GrafoTest.AgregarNodoRepetidoConservaAristas]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GrafoTest.AgregarNodoRepetidoConservaAristas]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_grafo.cpp:50]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[DijkstraTest.DistanciaCorrecta]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=DijkstraTest.DistanciaCorrecta]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DijkstraTest.DistanciaCorrecta]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_dijkstra.cpp:18]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[DijkstraTest.RutaReconstruida]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=DijkstraTest.RutaReconstruida]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DijkstraTest.RutaReconstruida]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_dijkstra.cpp:25]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[DijkstraTest.NodoInalcanzable]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=DijkstraTest.NodoInalcanzable]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[DijkstraTest.NodoInalcanzable]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_dijkstra.cpp:33]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[ColaPrioridadTest.OrdenCorrecto]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=ColaPrioridadTest.OrdenCorrecto]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ColaPrioridadTest.OrdenCorrecto]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_cola_prioridad.cpp:4]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[ColaPrioridadTest.PopEnColaVaciaLanzaExcepcion]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=ColaPrioridadTest.PopEnColaVaciaLanzaExcepcion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ColaPrioridadTest.PopEnColaVaciaLanzaExcepcion]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_cola_prioridad.cpp:17]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[ColaPrioridadTest.Tamanio]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=ColaPrioridadTest.Tamanio]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ColaPrioridadTest.Tamanio]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_cola_prioridad.cpp:22]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteTest.ConstruccionCorrecta]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteTest.ConstruccionCorrecta]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteTest.ConstruccionCorrecta]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:7]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteTest.MarcarAtendido]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteTest.MarcarAtendido]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteTest.MarcarAtendido]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:16]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteTest.TipoToString]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteTest.TipoToString]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteTest.TipoToString]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:22]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteFactoryTest.CrearConservaDatosYEstadoInicial]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteFactoryTest.CrearConservaDatosYEstadoInicial]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteFactoryTest.CrearConservaDatosYEstadoInicial]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:28]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteFactoryTest.AleatorioRespetaRangosYConservaId]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteFactoryTest.AleatorioRespetaRangosYConservaId]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteFactoryTest.AleatorioRespetaRangosYConservaId]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:40]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteFactoryTest.AleatorioConNodoMaximoCeroSoloUsaNodoCero]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteFactoryTest.AleatorioConNodoMaximoCeroSoloUsaNodoCero]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteFactoryTest.AleatorioConNodoMaximoCeroSoloUsaNodoCero]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:64]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[IncidenteFactoryTest.AleatorioRechazaNodoMaximoNegativo]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=IncidenteFactoryTest.AleatorioRechazaNodoMaximoNegativo]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[IncidenteFactoryTest.AleatorioRechazaNodoMaximoNegativo]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_incidente.cpp:72]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.AmbulanciaCreacion]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.AmbulanciaCreacion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.AmbulanciaCreacion]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:15]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.BomberoCreacion]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.BomberoCreacion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.BomberoCreacion]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:23]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.SerenazgoCreacion]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.SerenazgoCreacion]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.SerenazgoCreacion]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:29]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.AmbulanciaSoloAtiendeUrgenciasMedicas]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.AmbulanciaSoloAtiendeUrgenciasMedicas]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.AmbulanciaSoloAtiendeUrgenciasMedicas]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:36]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.BomberoAtiendeIncendiosYAccidentes]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.BomberoAtiendeIncendiosYAccidentes]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.BomberoAtiendeIncendiosYAccidentes]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:47]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.SerenazgoSoloAtiendeRobos]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.SerenazgoSoloAtiendeRobos]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.SerenazgoSoloAtiendeRobos]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:60]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryCreaTiposCorrecto]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryCreaTiposCorrecto]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryCreaTiposCorrecto]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:71]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryConservaDatosYEstadoInicialParaLosTresTipos]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryConservaDatosYEstadoInicialParaLosTresTipos]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryConservaDatosYEstadoInicialParaLosTresTipos]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:84]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryRechazaTipoDesconocido]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryRechazaTipoDesconocido]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryRechazaTipoDesconocido]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:101]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryCreaUnidadesIndependientes]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryCreaUnidadesIndependientes]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryCreaUnidadesIndependientes]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:108]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryUnidadesArrancanLibres]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryUnidadesArrancanLibres]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryUnidadesArrancanLibres]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:121]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryIncidenteCrearAtributosCorrectos]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryIncidenteCrearAtributosCorrectos]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryIncidenteCrearAtributosCorrectos]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:131]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadTest.FactoryIncidenteAleatorioNodoDentroRango]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadTest.FactoryIncidenteAleatorioNodoDentroRango]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadTest.FactoryIncidenteAleatorioNodoDentroRango]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_unidad.cpp:141]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[EstrategiaTest.PrioridadDevuelveUnidadLibre]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=EstrategiaTest.PrioridadDevuelveUnidadLibre]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[EstrategiaTest.PrioridadDevuelveUnidadLibre]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_estrategias.cpp:24]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[EstrategiaTest.BalanceCargaDevuelveUnidadConMenorCarga]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=EstrategiaTest.BalanceCargaDevuelveUnidadConMenorCarga]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[EstrategiaTest.BalanceCargaDevuelveUnidadConMenorCarga]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_estrategias.cpp:31]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[SimuladorThreadTest.ArrancarDetener]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=SimuladorThreadTest.ArrancarDetener]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[SimuladorThreadTest.ArrancarDetener]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:10]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[SimuladorThreadTest.TresHilosConcurrentes]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=SimuladorThreadTest.TresHilosConcurrentes]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[SimuladorThreadTest.TresHilosConcurrentes]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:23]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[SimuladorThreadTest.DetenerSinIniciar]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=SimuladorThreadTest.DetenerSinIniciar]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[SimuladorThreadTest.DetenerSinIniciar]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:48]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[SimuladorThreadTest.AccesoConcurrenteAUnidad]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=SimuladorThreadTest.AccesoConcurrenteAUnidad]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[SimuladorThreadTest.AccesoConcurrenteAUnidad]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:56]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadThreadSafetyTest.TickYGettersConcurrentes]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadThreadSafetyTest.TickYGettersConcurrentes]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadThreadSafetyTest.TickYGettersConcurrentes]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:73]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UnidadThreadSafetyTest.SetRutaYAvanzarNodoConcurrente]=]  /home/karla/SIDEA/build_tsan/tests/SIDEA_tests [==[--gtest_filter=UnidadThreadSafetyTest.SetRutaYAvanzarNodoConcurrente]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UnidadThreadSafetyTest.SetRutaYAvanzarNodoConcurrente]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[/home/karla/SIDEA/tests/test_threading.cpp:95]==]
    WORKING_DIRECTORY [==[/home/karla/SIDEA/build_tsan/tests]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
set(SIDEA_tests_TESTS [==[GrafoTest.AgregarNodosYAristas]==] [==[GrafoTest.NodoInexistenteLanzaExcepcion]==] [==[GrafoTest.TotalNodos]==] [==[GrafoTest.AristaDirigidaRegistraAmbosExtremos]==] [==[GrafoTest.AgregarNodoRepetidoConservaAristas]==] [==[DijkstraTest.DistanciaCorrecta]==] [==[DijkstraTest.RutaReconstruida]==] [==[DijkstraTest.NodoInalcanzable]==] [==[ColaPrioridadTest.OrdenCorrecto]==] [==[ColaPrioridadTest.PopEnColaVaciaLanzaExcepcion]==] [==[ColaPrioridadTest.Tamanio]==] [==[IncidenteTest.ConstruccionCorrecta]==] [==[IncidenteTest.MarcarAtendido]==] [==[IncidenteTest.TipoToString]==] [==[IncidenteFactoryTest.CrearConservaDatosYEstadoInicial]==] [==[IncidenteFactoryTest.AleatorioRespetaRangosYConservaId]==] [==[IncidenteFactoryTest.AleatorioConNodoMaximoCeroSoloUsaNodoCero]==] [==[IncidenteFactoryTest.AleatorioRechazaNodoMaximoNegativo]==] [==[UnidadTest.AmbulanciaCreacion]==] [==[UnidadTest.BomberoCreacion]==] [==[UnidadTest.SerenazgoCreacion]==] [==[UnidadTest.AmbulanciaSoloAtiendeUrgenciasMedicas]==] [==[UnidadTest.BomberoAtiendeIncendiosYAccidentes]==] [==[UnidadTest.SerenazgoSoloAtiendeRobos]==] [==[UnidadTest.FactoryCreaTiposCorrecto]==] [==[UnidadTest.FactoryConservaDatosYEstadoInicialParaLosTresTipos]==] [==[UnidadTest.FactoryRechazaTipoDesconocido]==] [==[UnidadTest.FactoryCreaUnidadesIndependientes]==] [==[UnidadTest.FactoryUnidadesArrancanLibres]==] [==[UnidadTest.FactoryIncidenteCrearAtributosCorrectos]==] [==[UnidadTest.FactoryIncidenteAleatorioNodoDentroRango]==] [==[EstrategiaTest.PrioridadDevuelveUnidadLibre]==] [==[EstrategiaTest.BalanceCargaDevuelveUnidadConMenorCarga]==] [==[SimuladorThreadTest.ArrancarDetener]==] [==[SimuladorThreadTest.TresHilosConcurrentes]==] [==[SimuladorThreadTest.DetenerSinIniciar]==] [==[SimuladorThreadTest.AccesoConcurrenteAUnidad]==] [==[UnidadThreadSafetyTest.TickYGettersConcurrentes]==] [==[UnidadThreadSafetyTest.SetRutaYAvanzarNodoConcurrente]==])
