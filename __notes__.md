# Intial configuration of build for the machine.
# From the Oliver root folder.
    mkdir build 
    cd build
    cmake ../

# After initial build, updates to the build can
# be done from the root, folder.
    cmake ./build
    cmake --build ./build

    cmake --build C:/Oly/build/ --target clean