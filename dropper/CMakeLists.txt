# CMakeList.txt: proyecto de CMake para MarlborgeNet, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable ("MarlborgeDropper" "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../include/openssl/applink.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeDropper PROPERTY CXX_STANDARD 20)
endif()

set("PRIVATE_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("PUBLIC_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

if(WIN32)
  target_link_libraries(MarlborgeDropper "ws2_32.lib")
  target_link_libraries(MarlborgeDropper "${LIB}/Win32/libcrypto.lib;${LIB}/Win32/libssl.lib")
endif()


target_include_directories(MarlborgeDropper PRIVATE "${PRIVATE_INCLUDES}")
target_include_directories(MarlborgeDropper PUBLIC "${PUBLIC_INCLUDES}")
# TODO: Agregue pruebas y destinos de instalación si es necesario.
