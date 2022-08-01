cmake_minimum_required(VERSION 3.23)

project(RayTracing VERSION 0.1 LANGUAGES CXX)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)


#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
#find_package(Qt6 REQUIRED COMPONENTS Widgets)
#find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

# Executable
qt_add_executable(RayTracing MANUAL_FINALIZATION)

# Sources
target_sources(RayTracing PRIVATE
    main.cc
    mainwindow.cc
    mainwindow.hh
    mainwindow.ui
)

target_include_directories(RayTracing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compilation options
target_compile_features(RayTracing PRIVATE cxx_std_20)
set_target_properties(RayTracing PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(RayTracing PRIVATE /W4)

set_target_properties(RayTracing PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Libraries
target_link_libraries(RayTracing PRIVATE Qt6::Widgets)
qt_finalize_executable(RayTracing)
