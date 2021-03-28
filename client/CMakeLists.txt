cmake_minimum_required(VERSION 3.10)
project(simple_rpi_gui)
set(EXEC_NAME simple_rpi_gui)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

IF( UNIX )
    set(ARCH linux)
ENDIF( UNIX )
IF( WIN32 )
    set(ARCH windows)
ENDIF( WIN32 )

include_directories(src 
		src/syscall
                src/gui
		../shared_src/socket
		../shared_src/socket/${ARCH}
		)

add_executable(${EXEC_NAME} src/main.cpp
			src/main.hpp
			src/syscall/${ARCH}/Systemcall.cpp
			src/syscall/Systemcall.hpp
                        ../shared_src/socket/OpensslWrapper.cpp
                        ../shared_src/socket/OpensslWrapper.hpp
			../shared_src/socket/ASocket.hpp
			../shared_src/socket/ISocket.hpp
			../shared_src/socket/${ARCH}/Socket.cpp
			../shared_src/socket/${ARCH}/Socket.hpp
                        src/tools/tools.cpp
                        src/tools/tools.h
                        src/tools/serverApi.cpp
                        src/tools/serverApi.hpp
                        src/tools/settingsmanager.cpp
                        src/tools/settingsmanager.hpp
                        src/gui/mainwindow.cpp
                        src/gui/mainwindow.h
                        src/gui/mainwindow.ui
                        src/gui/connection.cpp
                        src/gui/connection.h
                        src/gui/connection.ui
                        src/gui/settingspage.cpp
                        src/gui/settingspage.h
                        src/gui/settingspage.ui
                        src/gui/live/modelive.cpp
                        src/gui/live/modelive.h
                        src/gui/live/modeaudiolive.cpp
                        src/gui/live/modeaudiolive.h
                        src/gui/live/dynamicoutput.cpp
                        src/gui/live/dynamicoutput.h
                        src/gui/live/dynamicoutput.ui
                        src/gui/live/dynamicinput.cpp
                        src/gui/live/dynamicinput.h
                        src/gui/live/dynamicinput.ui
                        src/gui/live/gpiosettingdialog.cpp
                        src/gui/live/gpiosettingdialog.h
                        src/gui/live/gpiosettingdialog.ui
			)

find_package(OpenSSL REQUIRED)
IF (OPENSSL_FOUND)
        target_link_libraries(${EXEC_NAME} OpenSSL::SSL)
ENDIF (OPENSSL_FOUND)

#set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${EXEC_NAME} ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(${EXEC_NAME} Threads::Threads)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
target_link_libraries(${EXEC_NAME} Qt5::Widgets)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)
target_link_libraries(${EXEC_NAME} Qt5::Multimedia)

IF( WIN32 )
	target_link_libraries(${EXEC_NAME} ws2_32)
ENDIF( WIN32 )
