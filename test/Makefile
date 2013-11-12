CXX = g++
CFLAGS = -Wall -std=c++11 -I../src

aws_test: test.o oscillator.o
	$(CXX) $(CFLAGS) -o aws_test oscillator.o test.o -lm -lportaudio

test.o: test.cc
	$(CXX) -c -o $@ $< $(CFLAGS)

oscillator.o: ../src/oscillator.cc
	$(CXX) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o aws_test 
