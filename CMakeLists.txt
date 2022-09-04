# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie hier die globale Konfiguration aus,
# und schließen Sie Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.8)

project(CppSockets)

set( LIBRARY_SOURCES
CppSocketsUtil.hpp
Socket.hpp
TcpClient.hpp
TcpServer.hpp
 "Dummy.cpp")

set( TEST_SOURCES
${LIBRARY_SOURCES}
"examples/TcpServer.cpp"
)

add_library(CppSockets SHARED ${LIBRARY_SOURCES})
add_executable(CppSocketsTest ${TEST_SOURCES})
set_target_properties(CppSockets PROPERTIES LINKER_LANGUAGE CXX)