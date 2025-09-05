all:tcpepoll client

tcpepoll:tcpepoll.cpp InetAddress.cpp
	g++ -o tcpepoll tcpepoll.cpp InetAddress.cpp

client:client.cpp
	g++ -o client client.cpp

clean:
	rm -rf tcpepoll client