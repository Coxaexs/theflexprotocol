# Reusable helpers for wrapping PlatformIO environments as CMake targets.

if(NOT DEFINED PLATFORMIO_EXECUTABLE)
  find_program(PLATFORMIO_EXECUTABLE NAMES pio platformio)
endif()

if(NOT PLATFORMIO_EXECUTABLE)
  # Keep configure successful; build will fail with a clear message if pio is missing.
  set(PLATFORMIO_EXECUTABLE pio)
  message(WARNING "PlatformIO executable was not found during configure. Install PlatformIO or set PLATFORMIO_EXECUTABLE.")
endif()

function(add_platformio_environment)
  set(options)
  set(oneValueArgs TARGET_NAME PROJECT_DIR ENV_NAME TARGET_FOLDER)
  set(multiValueArgs)
  cmake_parse_arguments(PIO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT PIO_TARGET_NAME)
    message(FATAL_ERROR "add_platformio_environment requires TARGET_NAME")
  endif()

  if(NOT PIO_PROJECT_DIR)
    message(FATAL_ERROR "add_platformio_environment requires PROJECT_DIR")
  endif()

  if(NOT PIO_ENV_NAME)
    message(FATAL_ERROR "add_platformio_environment requires ENV_NAME")
  endif()

  set(_folder "PlatformIO")
  if(PIO_TARGET_FOLDER)
    set(_folder "${PIO_TARGET_FOLDER}")
  endif()

  add_custom_target(${PIO_TARGET_NAME}_build
    COMMAND ${PLATFORMIO_EXECUTABLE} run -e ${PIO_ENV_NAME}
    WORKING_DIRECTORY ${PIO_PROJECT_DIR}
    COMMENT "Building ${PIO_TARGET_NAME} (${PIO_ENV_NAME})"
    VERBATIM
  )

  add_custom_target(${PIO_TARGET_NAME}_upload
    COMMAND ${PLATFORMIO_EXECUTABLE} run -e ${PIO_ENV_NAME} -t upload
    WORKING_DIRECTORY ${PIO_PROJECT_DIR}
    COMMENT "Uploading ${PIO_TARGET_NAME} (${PIO_ENV_NAME})"
    VERBATIM
  )

  add_custom_target(${PIO_TARGET_NAME}_monitor
    COMMAND ${PLATFORMIO_EXECUTABLE} device monitor
    WORKING_DIRECTORY ${PIO_PROJECT_DIR}
    COMMENT "Opening serial monitor for ${PIO_TARGET_NAME}"
    VERBATIM
  )

  add_custom_target(${PIO_TARGET_NAME}_clean
    COMMAND ${PLATFORMIO_EXECUTABLE} run -e ${PIO_ENV_NAME} -t clean
    WORKING_DIRECTORY ${PIO_PROJECT_DIR}
    COMMENT "Cleaning ${PIO_TARGET_NAME} (${PIO_ENV_NAME})"
    VERBATIM
  )

  set_target_properties(${PIO_TARGET_NAME}_build PROPERTIES FOLDER "${_folder}")
  set_target_properties(${PIO_TARGET_NAME}_upload PROPERTIES FOLDER "${_folder}")
  set_target_properties(${PIO_TARGET_NAME}_monitor PROPERTIES FOLDER "${_folder}")
  set_target_properties(${PIO_TARGET_NAME}_clean PROPERTIES FOLDER "${_folder}")
endfunction()

function(add_platformio_test_environment)
  set(options)
  set(oneValueArgs TARGET_NAME PROJECT_DIR ENV_NAME TARGET_FOLDER)
  set(multiValueArgs)
  cmake_parse_arguments(PIO_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT PIO_TEST_TARGET_NAME)
    message(FATAL_ERROR "add_platformio_test_environment requires TARGET_NAME")
  endif()

  if(NOT PIO_TEST_PROJECT_DIR)
    message(FATAL_ERROR "add_platformio_test_environment requires PROJECT_DIR")
  endif()

  if(NOT PIO_TEST_ENV_NAME)
    message(FATAL_ERROR "add_platformio_test_environment requires ENV_NAME")
  endif()

  set(_folder "PlatformIO/Tests")
  if(PIO_TEST_TARGET_FOLDER)
    set(_folder "${PIO_TEST_TARGET_FOLDER}")
  endif()

  add_custom_target(${PIO_TEST_TARGET_NAME}_test
    COMMAND ${PLATFORMIO_EXECUTABLE} test -e ${PIO_TEST_ENV_NAME}
    WORKING_DIRECTORY ${PIO_TEST_PROJECT_DIR}
    COMMENT "Testing ${PIO_TEST_TARGET_NAME} (${PIO_TEST_ENV_NAME})"
    VERBATIM
  )

  set_target_properties(${PIO_TEST_TARGET_NAME}_test PROPERTIES FOLDER "${_folder}")
endfunction()
