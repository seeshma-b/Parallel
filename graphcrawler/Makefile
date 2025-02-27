# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lcurl -I /usr/include/rapidjson

# Executable names
CRAWLER_EXEC = crawler
FETCH_EXEC = fetch

# Source files
CRAWLER_SRC = crawler.cpp
FETCH_SRC = fetch.cpp

# Default build
all: $(CRAWLER_EXEC) $(FETCH_EXEC)

# Compile crawler
$(CRAWLER_EXEC): $(CRAWLER_SRC)
	$(CXX) $(CXXFLAGS) -o $(CRAWLER_EXEC) $(CRAWLER_SRC) $(LDFLAGS)

# Compile fetch
$(FETCH_EXEC): $(FETCH_SRC)
	$(CXX) $(CXXFLAGS) -o $(FETCH_EXEC) $(FETCH_SRC) $(LDFLAGS)

# Run BFS crawler (Example: make run ARGS='"Tom Hanks" 2')
run:
	./$(CRAWLER_EXEC) $(ARGS)

# Clean compiled binaries
clean:
	rm -f $(CRAWLER_EXEC) $(FETCH_EXEC)