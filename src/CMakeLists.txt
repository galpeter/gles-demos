cmake_minimum_required(VERSION 2.8)
project(gles-examples)

include(FindPkgConfig)

pkg_check_modules(EGL egl)
pkg_check_modules(GLESv2 glesv2)

find_package(X11)

include_directories(
  ${EGL_INCLUDE_DIRS}
  ${GLESv2_INCLUDE_DIRS}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(COMMON_FILES
    shaders.c
    matrix.c
    render_common.c
)

add_library(rendercommon STATIC ${COMMON_FILES})

function(add_example BIN_NAME SRC_NAME)
  add_executable(${BIN_NAME} ${SRC_NAME})
  target_link_libraries(${BIN_NAME} rendercommon ${EGL_LIBRARIES} ${GLESv2_LIBRARIES} ${X11_X11_LIB} m)
  if (ARGV2)
    target_compile_definitions(${BIN_NAME} PRIVATE ${ARGV2})
  endif()
endfunction(add_example)


add_example(triangle-no-vao example-triangle-no-vao.c)
add_example(triangle-no-vao-rot example-triangle-no-vao.c WITH_ROTATION)
add_example(triangle-vao-buf example-triangle-vao-buf.c)
add_example(triangle-vao-ptr example-triangle-vao-buf.c WITH_PTR_DATA)
