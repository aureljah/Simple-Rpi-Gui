# simple_rpi_gui
A simple remote gui for control rpi pins written in C++.
it'll be cross-platform (linux and windows) and will have a server side which must be running on rpi.
# WIP


# Usage
  ## linux - Server
    $cd server
    $make
    $./server

  ## linux - Client
    $cd client
    $make or $make build or $make re	(to build with cmake)
    $cd build 				(or an other dir defined as BUILD_DIR in Makefile)
    $make				(to compile and generate binairies)
    $./simple_rpi_gui			(works better when server is runing)

    $make clean 			(to clean the build folder)

  ## Linux - Generate certificate (all the parameters is up to you EXCEPT the name)
     $openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout key.pem -out mycert.pem
     
     -> mycert.pem will contain the certificate and key.pem will contain the private key.
     -> client only need certificate but server need both certificate and private key.
     -->> ATM certificate must be named mycert.pem and all private key must be named key.pem
     	  they must be in the same folder as the program (e.g. client/ or server/)

  ## windows
     $win_make.bat (to build with cmake)
	 $cd build
		Then, depend of your compilo

	 $win_make.bat clean 			(to clean the build folder)

# NOTE
  ## You may want to include these include dir into your editor (these are based on vscode)
                "${workspaceFolder}/../../shared_src/socket/linux/"
                "${workspaceFolder}/../../shared_src/socket/"
