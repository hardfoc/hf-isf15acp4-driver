# Run by CTest (hf_isf15acp4_install_package): install, then build a find_package() consumer against the prefix.
file(REMOVE_RECURSE "${PREFIX}" "${CONSUMER_BIN}")

function(run)
  execute_process(COMMAND ${ARGN} RESULT_VARIABLE rc)
  if(NOT rc EQUAL 0)
    message(FATAL_ERROR "failed (${rc}): ${ARGN}")
  endif()
endfunction()

run("${CMAKE_COMMAND}" --install "${BUILD_DIR}" --prefix "${PREFIX}")
run("${CMAKE_COMMAND}" -S "${CONSUMER_SRC}" -B "${CONSUMER_BIN}" "-DCMAKE_PREFIX_PATH=${PREFIX}"
    "-DCMAKE_CXX_COMPILER=${CXX}")
run("${CMAKE_COMMAND}" --build "${CONSUMER_BIN}")
