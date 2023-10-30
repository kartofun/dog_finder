project(just_library)

add_subdirectory(pybind11)

find_package(OpenCV 4.1.0 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIB_DIR})

pybind11_add_module(just_library just_library.cpp)
target_link_libraries(just_library PUBLIC ${OpenCV_LIBS})