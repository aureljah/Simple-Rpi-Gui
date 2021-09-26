**Disclaimer: The build process is not in final stage and some other manipulation may be necessary to compile this project succesfully.**

# Requirement
Make sure you have all the requirements below
### Client
- **CMake 3.10+**
- **OpenSSL 1.1.0+** *(You can also find windows binaries on openssl wiki)*
- **Qt 5.13**

### Server *(on Raspberry Pi)*
- **OpenSSL 1.1.0+**
- **pigpio** *(Follow instruction on http://abyz.me.uk/rpi/pigpio/download.html)*
- **g++**
- **C++11** *(might be needed to be activated as experimental feature in old version of gcc)*
- **libpthread**
- **libcrypto**


# Build / Install
## Server (Raspberry Pi)
```
$ cd server
$ make rpi
$ ./server
```
*Note: The command `make test_build` is also available to make a server for a non-raspberry linux computer. This will make a server that doesn't use the features of a Raspberry Pi and can serve for testing purpose.*

### Linux - Client
You can either open ``CMakeList.txt`` with `QtCreator`.
  
**Or you can use the following commands:** 
  
`$ cd client`
`$ make build` *(to build with cmake)*
`$ cd build` *(or an other dir defined as `BUILD_DIR` in the`Makefile`)*
`$ make` *(to compile and generate binairies)*
`$ ./simple_rpi_gui`
  
*Note: `$ make clean` can be used to clean the build folder and delete all generated files.*

### Linux - Generate certificate (all the parameters is up to you EXCEPT the name)
`$openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout key.pem -out mycert.pem`
  
*`mycert.pem` will contain the certificate and `key.pem` will contain the private key.*
  
**For now, the certificate must be named `mycert.pem` and all private key must be named `key.pem`. They must be in the same folder as the program (e.g. `client/` or `server/`).**

### Windows - Client
You can either open ``CMakeList.txt`` with `QtCreator`.
  
**Or you can use the following commands:** 
  
`$ win_make.bat` *(to build with cmake)*
`$ cd build`
  
**Then, it's depend of your IDE / compiler.**
- For Visual Studio, you can open the solution file: `simple_rpi_gui.sln`.

  
*Note: `$ win_make.bat clean` can be used to clean the build folder and delete all generated files.*
  
### Note for developer
**You may want to include these include dir into your editor for server (these are based on vscode)**
`${workspaceFolder}/../../shared_src/socket/linux/`
`${workspaceFolder}/../../shared_src/socket/`
