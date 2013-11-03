PATH := /usr/um/gcc-4.7.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-4.7.0/lib64
LD_RUN_PATH := /usr/um/gcc-4.7.0/lib64
all: driver.cpp clean
	@g++ -std=c++11 driver.cpp -o MarketMaker
clean:
	@rm -f *.o MarketMaker
