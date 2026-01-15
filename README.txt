Este proyecto ejecuta una simulación de combate de Jiu Jitsu brasileño entre dos agentes autónomos. La simulación se ejecuta directamente desde el directorio build y requiere que el archivo de técnicas esté presente en dicha carpeta.

Previa instalación de cadmium

Estructura global del proyecto

JiuJitsuFight/
├── build/                 ← carpeta de compilación
│   ├── CMakeFiles/
│   ├── CMakeCache.txt
│   ├── cmake_install.cmake
│   ├── Makefile
│   ├── Grafo.csv          ← grafo de técnicas (copiado manualmente)
│   └── JiuJitsuFight      ← ejecutable final
│
├── main/                  ← código fuente (models, agents, engine, etc.)
│
├── build_sim.sh           ← script de compilación
├── CMakeLists.txt         ← configuración CMake
└── Grafo.csv              ← grafo original (fuente)

Flujo correcto de uso

Compilar el proyecto:

./build_sim.sh

VERIFICAR LA COPIA DEL GRAFO DENTRO DE CARPETA build.

Ejecutar la simulación:

cd build
./JiuJitsuFight
