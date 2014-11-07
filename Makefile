CXX=g++
CXXFLAGS=-c -g -Wall
LDFLAGS=-lX11
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=xgrabkeyboard

all: $(SOURCES) $(TARGET)
	
$(TARGET): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@
