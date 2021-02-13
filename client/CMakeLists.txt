cmake_minimum_required(VERSION 3.10)
project(simple_rpi_gui)
set(EXEC_NAME simple_rpi_gui)

IF( UNIX )
    set(ARCH linux)
ENDIF( UNIX )
IF( WIN32 )
    set(ARCH windows)
ENDIF( WIN32 )

include_directories(src 
		src/syscall
		src/openssl_wrapper
		../shared_src/socket
		../shared_src/socket/${ARCH}
		)

add_executable(${EXEC_NAME} src/main.cpp
			src/main.hpp
			src/syscall/${ARCH}/Systemcall.cpp
			src/syscall/Systemcall.hpp
			../shared_src/socket/ASocket.hpp
			../shared_src/socket/ISocket.hpp
			../shared_src/socket/${ARCH}/Socket.cpp
			../shared_src/socket/${ARCH}/Socket.hpp
			src/openssl_wrapper/OpensslWrapper.cpp
			src/openssl_wrapper/OpensslWrapper.hpp
			)

find_package(OpenSSL REQUIRED)
IF (OPENSSL_FOUND)
	target_link_libraries(${EXEC_NAME} OpenSSL::SSL)
ENDIF (OPENSSL_FOUND)


IF( WIN32 )
	target_link_libraries(${EXEC_NAME} ws2_32)
ENDIF( WIN32 )