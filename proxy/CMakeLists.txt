# CMakeList.txt: proyecto de CMake para MarlborgeNet, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable ("MarlborgeProxy" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeProxy PROPERTY CXX_STANDARD 20)
endif()

# TODO: Agregue pruebas y destinos de instalación si es necesario.
