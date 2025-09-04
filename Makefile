# Project settings
TARGET = game
SRC = src/*.cpp
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -g
LIBDIR = lib
INCDIR = include

# Libraries (Raylib + system deps)
LDFLAGS = -L$(LIBDIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CPPFLAGS = -I$(INCDIR)

# Build rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	LD_LIBRARY_PATH=$(LIBDIR):$$LD_LIBRARY_PATH ./$(TARGET)

clean:
	rm -f $(TARGET)
