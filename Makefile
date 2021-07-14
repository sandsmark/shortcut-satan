EXECUTABLE=shortcut-satan
CXXFILES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp, %.o, $(CXXFILES))
LDFLAGS+=-ludev
CXXFLAGS+=-Wall -Wextra -pedantic -std=c++2a -fPIC -g

all: $(EXECUTABLE)

%.o: %.cpp Makefile
	$(CXX) -MMD -MP $(CXXFLAGS) -o $@ -c $<

DEPS=$(OBJECTS:.o=.d)
-include $(DEPS)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)
clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(DEPS)
