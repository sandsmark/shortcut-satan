LDFLAGS+=-ludev
CXXFLAGS+=-Wall -Wextra -pedantic
CCFILES=$(wildcard *.cpp)
CXXFLAGS += -g -fPIC -std=c++2a -O1 -Wall -Wextra -pedantic
OBJECTS=$(patsubst %.cpp, %.o, $(CCFILES))
EXECUTABLE=shortcut-satan
all: $(EXECUTABLE)


%.o: %.cpp
	$(CXX) -MMD -MP $(CXXFLAGS) -o $@ -c $<

DEPS=$(OBJECTS:.o=.d)
-include $(DEPS)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
