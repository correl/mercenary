set (irclib_SOURCES
	ircclient.cpp
	dccserver.cpp
)
set (irclib_MOC_HEADERS
	ircclient.h
	dccserver.h
)
QT4_WRAP_CPP (irclib_MOC_SOURCES ${irclib_MOC_HEADERS})
add_library (ircclient STATIC ${irclib_SOURCES} ${irclib_MOC_SOURCES})