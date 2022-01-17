get_filename_component(prefix "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)

file(
    RELATIVE_PATH relative_path
    "/${cloop_INSTALL_CMAKEDIR}" "/${CMAKE_INSTALL_BINDIR}/${cloop_NAME}"
)

file(WRITE "${prefix}/${cloop_INSTALL_CMAKEDIR}/cloopConfig.cmake" "\
get_filename_component(
    CLOOP_EXECUTABLE \"\${CMAKE_CURRENT_LIST_DIR}/${relative_path}\"
    ABSOLUTE
)

include(\"\${CMAKE_CURRENT_LIST_DIR}/cloopTargets.cmake\")
")
