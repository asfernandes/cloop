cmake_minimum_required(VERSION 3.1)

project(cloop CXX)

# ---- cloop executable ----

add_executable(
    cloop
    src/cloop/Expr.cpp
    src/cloop/Generator.cpp
    src/cloop/Lexer.cpp
    src/cloop/Main.cpp
    src/cloop/Parser.cpp
    src/cloop/Expr.h
    src/cloop/Generator.h
    src/cloop/Lexer.h
    src/cloop/Parser.h
)

# Code uses std::auto_ptr
set_property(TARGET cloop PROPERTY CXX_STANDARD 98)

# ---- Install rules ----

if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

include(GNUInstallDirs)

install(
    TARGETS cloop
    EXPORT cloopTargets
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT cloop_Runtime
)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME cloop_Development)

set(
    cloop_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/cloop"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(cloop_INSTALL_CMAKEDIR)

install(
    EXPORT cloopTargets
    NAMESPACE cloop::
    DESTINATION "${cloop_INSTALL_CMAKEDIR}"
)

if(CMAKE_VERSION VERSION_LESS "3.14")
  set(maybe_exe cloop)
  if(WIN32)
    set(maybe_exe cloop.exe)
  endif()
  set(cloop_EXECUTABLE_NAME "${maybe_exe}" CACHE STRING "Executable name")
else()
  cmake_policy(SET CMP0087 NEW)
  set(cloop_EXECUTABLE_NAME "$<TARGET_FILE_NAME:cloop>")
endif()
install(CODE "set(cloop_NAME [[${cloop_EXECUTABLE_NAME}]])")

install(CODE "set(cloop_INSTALL_CMAKEDIR [[${cloop_INSTALL_CMAKEDIR}]])")
install(CODE "set(CMAKE_INSTALL_BINDIR [[${CMAKE_INSTALL_BINDIR}]])")

install(SCRIPT cmake/install.cmake)
