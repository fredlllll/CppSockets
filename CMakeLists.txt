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

set( TCP_SERVER_SOURCES
${LIBRARY_SOURCES}
"examples/TcpServer.cpp"
)

set( TCP_SERVER_AND_CLIENT_SOURCES
${LIBRARY_SOURCES}
"examples/TcpServerAndClient.cpp"
)

#add_library(CppSockets SHARED ${LIBRARY_SOURCES})
#set_target_properties(CppSockets PROPERTIES LINKER_LANGUAGE CXX)

add_executable(TcpServerExample ${TCP_SERVER_SOURCES})
set_target_properties(TcpServerExample PROPERTIES LINKER_LANGUAGE CXX)

add_executable(TcpServerAndClientExample ${TCP_SERVER_AND_CLIENT_SOURCES})
set_target_properties(TcpServerAndClientExample PROPERTIES LINKER_LANGUAGE CXX)