CXX         = g++

SRCDIR      = src
BUILDDIR    = build

#SRCS        := $(shell find $(SRCDIR) -name *.cpp)
#OBJS        := $(SRCS:%=$(BUILDDIR)/%.o)
CLIENT_SRCS := src/client.cpp
CLIENT_OBJS := $(CLIENT_SRCS:%=$(BUILDDIR)/%.o)
SERVER_SRCS := src/server.cpp src/goodreads.cpp src/database.cpp src/pugixml/pugixml.cpp
SERVER_OBJS := $(SERVER_SRCS:%=$(BUILDDIR)/%.o)
DEPS        := $(CLIENT_OBJS:.o=.d) $(SERVER_OBJS:.o=.d)

CXXFLAGS    = -g -Wall -Wextra -pthread -std=c++17
LDLIBS      = -lcurl -lcurlpp -lsqlite3 -lboost_system -lboost_filesystem


$(BUILDDIR)/Client: $(CLIENT_OBJS)
	$(CXX) $(CLIENT_OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/Server: $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILDDIR)


-include $(DEPS)

MKDIR_P ?= mkdir -p
