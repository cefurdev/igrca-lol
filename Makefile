CXX = g++
CXXFLAGS = -Isrc/include
LDFLAGS = -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
SRC = main.cpp entity.cpp loadscreen.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = igrica

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)