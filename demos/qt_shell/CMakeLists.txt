
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 5.10 COMPONENTS Core Gui Widgets REQUIRED)

set(SOURCE_FILES
		main.cpp
		lavos_renderer.cpp
		lavos_renderer.h)

add_executable(qt_shell ${SOURCE_FILES})
target_link_libraries(qt_shell
		Qt5::Core Qt5::Gui Qt5::Widgets
		lavos_shell_qt
		lavos)