CXX         = g++

SRCDIR      = src
BUILDDIR    = build

CLIENT_SRCS := src/client.cpp src/util.cpp
CLIENT_OBJS := $(CLIENT_SRCS:%=$(BUILDDIR)/%.o)
SERVER_SRCS := src/server.cpp src/util.cpp src/clientHandler.cpp src/goodreads.cpp \
               src/database.cpp src/pugixml/pugixml.cpp
SERVER_OBJS := $(SERVER_SRCS:%=$(BUILDDIR)/%.o)
DEPS        := $(CLIENT_OBJS:.o=.d) $(SERVER_OBJS:.o=.d)

CXXFLAGS    = -g -Wall -Wextra -pthread -std=c++17
LDLIBS      = -lcurl -lcurlpp -lsqlite3 -lstdc++fs


.PHONY: all client server clean

all: client server
client: $(BUILDDIR)/Client
server: $(BUILDDIR)/Server

clean:
	$(RM) -r $(BUILDDIR)

$(BUILDDIR)/Client: $(CLIENT_OBJS)
	$(CXX) $(CLIENT_OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/Server: $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


-include $(DEPS)

MKDIR_P ?= mkdir -p
