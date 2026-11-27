param(
    [string]$QtRoot = 'C:\Qt\6.12.0\mingw_64',
    [string]$ToolchainRoot = 'C:\Qt\Tools\mingw1310_64',
    [string]$CMakeRoot = 'C:\Qt\Tools\CMake_64'
)

$ErrorActionPreference = 'Stop'
$proyecto = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$marca = Get-Date -Format 'yyyyMMdd-HHmmss'
$trabajo = Join-Path ([System.IO.Path]::GetTempPath()) "SIDEA-verificacion-$marca-$([guid]::NewGuid().ToString('N').Substring(0, 8))"
$registro = Join-Path $proyecto "build\verificacion-cmake-$marca.log"
$pathAnterior = $env:Path
$plataformaAnterior = $env:QT_QPA_PLATFORM
$transcripcionIniciada = $false
$resultado = 0

function Invoke-Comando {
    param([string]$Ejecutable, [string[]]$Argumentos)
    & $Ejecutable @Argumentos
    if ($LASTEXITCODE -ne 0) {
        throw "El comando $Ejecutable termino con codigo $LASTEXITCODE."
    }
}

try {
    New-Item -ItemType Directory -Path (Join-Path $proyecto 'build') -Force | Out-Null
    Start-Transcript -Path $registro | Out-Null
    $transcripcionIniciada = $true
    $cmake = Join-Path $CMakeRoot 'bin\cmake.exe'
    $ctest = Join-Path $CMakeRoot 'bin\ctest.exe'
    $compilador = Join-Path $ToolchainRoot 'bin\g++.exe'
    $make = Join-Path $ToolchainRoot 'bin\mingw32-make.exe'
    foreach ($archivo in @($cmake, $ctest, $compilador, $make, (Join-Path $QtRoot 'bin\moc.exe'))) {
        if (-not (Test-Path -LiteralPath $archivo -PathType Leaf)) {
            throw "No se encontro la herramienta: $archivo"
        }
    }
    if ($trabajo -match '[^\x00-\x7F]') {
        throw 'La carpeta temporal tiene caracteres no ASCII. Define TEMP con una carpeta sin tildes antes de ejecutar el comprobador.'
    }
    $env:Path = "$(Join-Path $CMakeRoot 'bin');$(Join-Path $ToolchainRoot 'bin');$(Join-Path $QtRoot 'bin');$pathAnterior"

    Write-Host '1/5 Preparando una copia de las fuentes en una ruta sin tildes...'
    New-Item -ItemType Directory -Path $trabajo | Out-Null
    foreach ($carpeta in @('src', 'tests', 'assets', 'examples')) {
        Copy-Item -LiteralPath (Join-Path $proyecto $carpeta) -Destination $trabajo -Recurse
    }
    foreach ($archivo in @('CMakeLists.txt', 'resources.qrc')) {
        Copy-Item -LiteralPath (Join-Path $proyecto $archivo) -Destination $trabajo
    }

    $configuracion = @('-S', $trabajo, '-B', (Join-Path $trabajo 'build'), '-G', 'MinGW Makefiles',
        '-DCMAKE_BUILD_TYPE=Debug', "-DCMAKE_PREFIX_PATH=$($QtRoot.Replace('\', '/'))",
        "-DCMAKE_CXX_COMPILER=$($compilador.Replace('\', '/'))", "-DCMAKE_MAKE_PROGRAM=$($make.Replace('\', '/'))")
    foreach ($relativa in @('build_tsan\_deps\googletest-src', 'build\_deps\googletest-src')) {
        $cache = Join-Path $proyecto $relativa
        if (Test-Path -LiteralPath (Join-Path $cache 'CMakeLists.txt')) {
            $gtest = Join-Path $trabajo 'googletest-local'
            Copy-Item -LiteralPath $cache -Destination $gtest -Recurse
            $configuracion += "-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$($gtest.Replace('\', '/'))"
            break
        }
    }

    Write-Host '2/5 Configurando CMake...'
    Invoke-Comando -Ejecutable $cmake -Argumentos $configuracion
    Write-Host '3/5 Compilando la aplicacion, pruebas y utilidades...'
    Invoke-Comando -Ejecutable $cmake -Argumentos @('--build', (Join-Path $trabajo 'build'),
        '--target', 'SIDEA', 'SIDEA_tests', 'SIDEA_gui_tests', 'SIDEA_capturas', 'SIDEA_comparar', '--parallel', '4')

    Write-Host '4/5 Ejecutando CTest...'
    $env:QT_QPA_PLATFORM = 'offscreen'
    Invoke-Comando -Ejecutable $ctest -Argumentos @('--test-dir', (Join-Path $trabajo 'build'), '--output-on-failure')

    Write-Host '5/5 Comprobando el escenario comparativo y las capturas...'
    Invoke-Comando -Ejecutable (Join-Path $trabajo 'build\SIDEA_comparar.exe') -Argumentos @()
    Invoke-Comando -Ejecutable (Join-Path $trabajo 'build\SIDEA_capturas.exe') -Argumentos @(
        (Join-Path $trabajo 'capturas'), (Join-Path $env:WINDIR 'Fonts\arial.ttf'))

    Write-Host ''
    Write-Host 'COMPROBACION CORRECTA: CMake, compilacion, pruebas, comparacion y capturas.' -ForegroundColor Green
    Write-Host "Ejecutable generado: $(Join-Path $trabajo 'build\SIDEA.exe')"
    Write-Host "Registro: $registro"
    Write-Host 'Las fuentes originales se conservaron. La copia temporal queda disponible para inspeccion.'
} catch {
    $resultado = 1
    Write-Host "COMPROBACION INTERRUMPIDA: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "Registro: $registro"
} finally {
    $env:Path = $pathAnterior
    $env:QT_QPA_PLATFORM = $plataformaAnterior
    if ($transcripcionIniciada) { Stop-Transcript | Out-Null }
}
exit $resultado
