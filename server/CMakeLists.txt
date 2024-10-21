# CMakeList.txt: proyecto de CMake para MarlborgeNet, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable ("MarlborgeServer" "main.cpp" "public/Client.h" "private/Client.cpp" "public/Packet.h" "${CMAKE_CURRENT_SOURCE_DIR}/../include/openssl/applink.c")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeServer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Agregue pruebas y destinos de instalación si es necesario.

set("PRIVATE_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("PUBLIC_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

if(WIN32)
  target_link_libraries(MarlborgeServer "ws2_32.lib")
  target_link_libraries(MarlborgeServer "${LIB}/Win32/libcrypto.lib;${LIB}/Win32/libssl.lib")
endif()

file(GLOB CERT "${CMAKE_CURRENT_SOURCE_DIR}/cert/*")
foreach(CERT_FILE ${CERT})
    file(COPY ${CERT_FILE} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/cert/")
endforeach()

target_include_directories(MarlborgeServer PRIVATE "${PRIVATE_INCLUDES}")
target_include_directories(MarlborgeServer PUBLIC "${PUBLIC_INCLUDES}")