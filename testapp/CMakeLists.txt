set (cliapp_SOURCES
	main.cpp
	testapp.cpp
)
set (cliapp_MOC_HEADERS
	testapp.h
)
QT4_WRAP_CPP (cliapp_MOC_SOURCES ${cliapp_MOC_HEADERS})
include_directories(${ircclient_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories(${ircclient_BINARY_DIR}/src)

add_executable(cliapp ${cliapp_SOURCES} ${cliapp_MOC_SOURCES} ${cliapp_ui})
target_link_libraries (cliapp ircclient ${LIBS})