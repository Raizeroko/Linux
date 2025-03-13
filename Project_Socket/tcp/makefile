all:server client
client:TCPClient.cc
	g++ -o $@ $^
server: main.cc
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f server client