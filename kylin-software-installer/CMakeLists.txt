cmake_minimum_required(VERSION 3.16)
project(kylin-software-installer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5
find_package(Qt5 COMPONENTS 
    Core 
    Gui 
    Widgets 
    Concurrent 
    Network 
    REQUIRED
)

# Find system libraries
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/welcomescreen.cpp
    src/packageinfoscreen.cpp
    src/dependencyscreen.cpp
    src/installscreen.cpp
    src/completescreen.cpp
    src/packageparser.cpp
    src/dependencyanalyzer.cpp
    src/packagemanager.cpp
    src/logger.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/welcomescreen.h
    src/packageinfoscreen.h
    src/dependencyscreen.h
    src/installscreen.h
    src/completescreen.h
    src/packageparser.h
    src/dependencyanalyzer.h
    src/packagemanager.h
    src/logger.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Network
    ZLIB::ZLIB
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES resources/kylin-software-installer.desktop DESTINATION share/applications)
install(FILES resources/icons/app.png DESTINATION share/pixmaps)
