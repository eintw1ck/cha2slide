IF NOT EXIST .build GOTO CONFIGURE_CMAKE
rmdir /s .build
:CONFIGURE_CMAKE
mkdir .build
cd .build
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_MAKE_PROGRAM=mingw32-make -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ..