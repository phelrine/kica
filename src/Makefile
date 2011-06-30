CXX = g++
CXXFLAGS = -g
OPENNI = -I /usr/include/ni
INCLUDES = -I ../include $(OPENNI) 
LDFLAGS = -lOpenNI

OS = $(shell uname)
ifeq ($(OS), Darwin)
	LDFLAGS += -framework OpenGL -framework GLUT
else
	LDFLAGS += -lglut
endif

OBJ = ImageDrawer.o TextureDrawer.o DepthDrawer.o SkeletonDrawer.o Capture.o
SRC = $(OBJ:.o.cpp)

.PHONY: all clean depend check-syntax 

all: kica kivi CSVData.o

kica: $(OBJ) kica.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $^ -o $@

kivi: $(OBJ) kivi.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $^ -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

clean:
	$(RM) *.o

depend:
	@$(CXX) -MM $(SRC) > .depend

-include .depend

check-syntax:
	$(CXX) $(INCLUDES) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES)

