cmake_minimum_required(VERSION 3.12)

project(demo VERSION 1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt5 COMPONENTS Widgets Core Charts REQUIRED)

# Add your source files
set(SOURCES
    main.cpp

    mainwindow.cpp

    module/wave_view/mwaveview.cpp
    module/filter/filter.cpp
    module/udp/udp.cpp
    module/udp/udp_receiver_qt.cpp
    module/file_writer/file_writer_qt.cpp
)

set(HEADS
    main.cpp

    mainwindow.h

    module/wave_view/mwaveview.h
    module/filter/filter.h
    module/udp/udp.h
    module/udp/udp_receiver_qt.h
    module/file_writer/file_writer_qt.h
)

set(UI
    mainwindow.ui
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/module/asio
        ${CMAKE_CURRENT_SOURCE_DIR}/module/filter
        ${CMAKE_CURRENT_SOURCE_DIR}/module/udp
        ${CMAKE_CURRENT_SOURCE_DIR}/module/wave_view
        ${CMAKE_CURRENT_SOURCE_DIR}/module/file_writer
)

# Add the executable target
add_executable(demo ${SOURCES} ${HEADS} ${UI}
    netwindow.h netwindow.cpp netwindow.ui
    module/net_check/netcheck.cpp module/net_check/netcheck.h

)


# Link against the Qt libraries
#target_link_libraries(demo PRIVATE Qt6::Widgets Qt::Charts)
target_link_libraries(demo PUBLIC Qt5::Core Qt5::Widgets Qt5::Charts)
target_link_libraries(demo PUBLIC -lwsock32 -lws2_32)
