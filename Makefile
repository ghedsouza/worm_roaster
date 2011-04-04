SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
LDFLAGS = $(shell pkg-config --libs gtkmm-2.4 gtkglextmm-1.2)
CPPFLAGS = $(shell pkg-config --cflags gtkmm-2.4 gtkglextmm-1.2)
CXXFLAGS = $(CPPFLAGS) -W -Wall -g
CXX = g++
MAIN = game488

all: $(MAIN)

depend: $(DEPENDS)

clean:
	rm -f *.o *.d $(MAIN)

$(MAIN): $(OBJECTS)
	@echo Creating $@...
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) -lSDL -lSDL_image

%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) -I. -I/usr/include/SDL -o $@ -c $(CXXFLAGS) $<

%.d: %.cpp
	@echo Building $@...
	@set -e; $(CC) -I. -I/usr/include/SDL  -M $(CPPFLAGS) $< \
                  | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
                [ -s $@ ] || rm -f $@

include $(DEPENDS)
