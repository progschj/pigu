
CFLAGS+=-O3 -march=armv6 -mfpu=vfp -mfloat-abi=hard -Wall -Wextra
LDFLAGS+=-lGLESv2 -lpigu
INCLUDES+=-I../include
LIBS+=-L../lib
SRC=cube.cpp
OBJ=${SRC:.cpp=.bin}

$(OBJ): %.bin: %.cpp
	$(CXX) $< -o $@ $(CFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS)

clean:
	-rm *.bin