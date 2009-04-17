set (guiapp_SOURCES
	main.cpp
	messagehandler.cpp
)
set (guiapp_MOC_HEADERS
	chatwindow.h
	messagehandler.h
)
QT4_WRAP_CPP (guiapp_MOC_SOURCES ${guiapp_MOC_HEADERS})
QT4_WRAP_UI (guiapp_ui chatwindow.ui)
include_directories(${ircclient_SOURCE_DIR}/src)
include_directories(${ircclient_SOURCE_DIR}/mirc/includes)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
link_directories(${ircclient_BINARY_DIR}/src)
link_directories(${ircclient_BINARY_DIR}/mirc/src)

add_executable(guiapp ${guiapp_SOURCES} ${guiapp_MOC_SOURCES} ${guiapp_ui})
target_link_libraries (guiapp ircclient mirc ${LIBS})