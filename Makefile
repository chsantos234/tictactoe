SERVER = server
CLIENT = client

CXX = g++
CXXFLAGS = -pthread -std=c++11

all: $(SERVER) $(CLIENT)

$(SERVER): server.cpp
	$(CXX) $(CXXFLAGS) server.cpp -o $(SERVER)

$(CLIENT): client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o $(CLIENT)

run_server: $(SERVER)
	gnome-terminal -- ./$(SERVER)

run_client1: $(CLIENT)
	gnome-terminal -- ./$(CLIENT)

run_client2: $(CLIENT)
	gnome-terminal -- ./$(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)
