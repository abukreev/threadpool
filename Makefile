CXX=g++
SRCDIR=src
OBJDIR=obj
BINDIR=bin
CXXFLAGS= -c -Wall -I$(SRCDIR) 
LDFLAGS=
LIBS=-lpthread
SOURCE_FILES=main.cpp
SOURCES=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(SOURCE_FILES))
OBJECT_FILES=$(patsubst %.cpp,%.o,$(SOURCE_FILES))
OBJECTS=$(patsubst %.o,$(OBJDIR)/%.o,$(OBJECT_FILES))

EXECUTABLE=$(BINDIR)/test

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJDIR) $(OBJECTS) $(BINDIR) $(SOURCES)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -f $(OBJECTS)
