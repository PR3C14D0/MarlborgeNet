# CMakeList.txt: proyecto de CMake para MarlborgeNet, incluya el origen y defina
# la lógica específica del proyecto aquí.
#

# Agregue un origen al ejecutable de este proyecto.
add_executable ("MarlborgeClient" "main.cpp")
#add_library ("MarlborgeDLL" SHARED "dllmain.cpp")
add_executable("MarlborgeDLL" "dllmain.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../include/openssl/applink.c" "public/Packet.h" "public/Server.h" "private/Server.cpp" "public/Memory.h" "private/Memory.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeClient PROPERTY CXX_STANDARD 20)
endif()

set("PRIVATE_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("PUBLIC_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

if(WIN32)
  target_link_libraries(MarlborgeDLL "ws2_32.lib")
  target_link_libraries(MarlborgeDLL "${LIB}/Win32/libcrypto.lib;${LIB}/Win32/libssl.lib")
endif()

target_include_directories(MarlborgeDLL PRIVATE "${PRIVATE_INCLUDES}")
target_include_directories(MarlborgeDLL PUBLIC "${PUBLIC_INCLUDES}")