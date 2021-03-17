###
INCLUDE=-I./. -I./includes
LIBPATH=-L./libs
LIBS=-lcurl
SRC=lnxkern.cpp Errors.cpp
OBJ=lnxkern
STD=--std=c++20
GIT_V=$(shell ./git_version.sh)
DEF=-DGIT_VERS=\"$(GIT_V)\" -D_FILE_OFFSET_BITS=64

build:
	c++ $(STD) -Os -Wall -Wextra -pedantic -ffunction-sections -fdata-sections -Wl,--gc-sections $(DEF) -o $(OBJ) $(SRC) $(INCLUDE) $(LIBPATH) $(LIBS)
	strip -s -R .gnu.version --strip-unneeded $(OBJ)
static:
	g++ $(STD) -Wl,-z,now -Wl,-z,relro -pie -static -static-libstdc++ -Os -Wall -Wextra -pedantic -ffunction-sections -fdata-sections -Wl,--gc-sections $(DEF) -o $(OBJ) $(SRC) $(INCLUDE) $(LIBPATH) $(LIBS)
	strip -s -R .gnu.version --strip-unneeded $(OBJ)
debug:
	c++ $(STD) -g -O0 -Wall -Wextra -pedantic $(DEF) -o $(OBJ) $(SRC) $(INCLUDE) $(LIBPATH) $(LIBS)

.PHONY: install
install:
	cp -v ./$(OBJ) ~/bin/
.PHONY: doxy
doxy:
	doxygen Doxyfile.cfg
#-lstdc++fs
#c++ --std=c++17 -Wall -Wextra -pedantic -o
#-lstdc++fs
#for Linux : -D_FILE_OFFSET_BITS=64
