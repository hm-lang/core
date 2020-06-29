TARGET=hm

run:
	mkdir -p build && cd build && cmake .. && make && ./$(TARGET)

debug:
	mkdir -p buildDebug && cd buildDebug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && gdb $(TARGET)

clean:
	rm -rf build buildDebug test.h
