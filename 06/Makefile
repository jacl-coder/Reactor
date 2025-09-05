all:tcpepoll client

tcpepoll:tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp
	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp

client:client.cpp
	g++ -g -o client client.cpp

clean:
	rm -rf tcpepoll client