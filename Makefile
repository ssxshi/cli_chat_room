
clean:
	rm ChatRoom

all:
	g++ src/*.cpp -o ChatRoom -Iinclude
