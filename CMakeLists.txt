cmake_minimum_required(VERSION 3.5)

project(beadando_II VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Tile.h
        Tile.cpp
        Stock.h
        Stock.cpp
        Grid.h
        Grid.cpp
        PipeLine.h
        GridPosition.h
        GridPosition.cpp
        PipeLine.cpp
        BuildState.h
        BuildState.cpp
        Phase.cpp
        Phase.h
        Flow.cpp
        Flow.h
        PipeLineValidator.cpp
        PipeLineValidator.h
        PipeLineBuilder.cpp
        PipeLineBuilder.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(beadando_II
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET beadando_II APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(beadando_II SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(beadando_II
            ${PROJECT_SOURCES}
        )
    endif()
endif()


# make doctest available in the project, running csovesteszt.cpp
add_executable(csovesteszt csovesteszt.cpp Tile.cpp GridPosition.cpp Grid.cpp PipeLine.cpp Stock.cpp BuildState.cpp)


target_link_libraries(beadando_II PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(csovesteszt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(beadando_II PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

set_target_properties(csovesteszt PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

install(TARGETS beadando_II
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(beadando_II)
endif()
