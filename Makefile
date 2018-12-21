BIN_DIR = bin
SRC_DIR = src
POCO_DIR = external/poco

SRCS = queue.cpp \
	websocket_connection.cpp \
	request_handler_factory.cpp \
	websocket_server.cpp \
	server_main.cpp

OBJS = $(SRCS:.cpp=.o)

EXEC = server

CFLAGS = -g -std=c++11 -Isrc
CFLAGS += -I$(POCO_DIR)/include/

LDFLAGS = -L$(POCO_DIR)/lib -lPocoUtil -lPocoXML -lPocoJSON -lPocoNet -lPocoFoundation -lpthread

vpath %.cpp src/

all: $(EXEC)
	@if [ ! -d bin ]; then \
		mkdir -p bin; \
		chmod o+w bin; \
	fi
	mv $(OBJS) $(BIN_DIR)/
	mv $(EXEC) $(BIN_DIR)/

$(EXEC): $(OBJS)
	g++ -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

.cpp.o:
	g++ $(CFLAGS) -c $<

clean:
	rm -rvf $(BIN_DIR)/*

