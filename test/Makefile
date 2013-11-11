CXX = g++

test: 
	$(CXX) -I../src -o test ../src/oscillator.cc test.cc -lm -lportaudio 

.PHONY: clean

clean:
	rm -f *.o test 
