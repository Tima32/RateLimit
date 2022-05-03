RATE_LIMIT=rate_limit.elf

TRAFFIC_FILTER_SOURCES=main.cpp
CPPFLAGS=-I./ArgumentParserLib

CC=$(CROSS_COMPILE)g++

TRAFFIC_FILTER_OBJECTS=$(patsubst %.cpp,%.o,$(TRAFFIC_FILTER_SOURCES))
all: $(RATE_LIMIT)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@

$(RATE_LIMIT): $(TRAFFIC_FILTER_OBJECTS)
	$(CC) $^ -o $@ -pthread -lrt


clean:
	rm -f $(RATE_LIMIT) $(TRAFFIC_FILTER_OBJECTS)

.PHONY: all clean
