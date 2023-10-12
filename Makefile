.PHONY: generate build-project run

generate:
	mkdir build
	make build-project

build-project:
	cd ./build && cmake .. && make --no-print-directory

run:
	./build/simplifier -r ab -f test.txt

run-tests:
	./build/test/fuzz_test

valgrind-run-tests:
	valgrind --tool=memcheck ./build/test/fuzz_test

valgrind-run:
	valgrind --tool=memcheck ./build/simplifier

check:
	cppcheck --language=c++ ./project/src/*.cpp ./main.cpp

console: 
	./build/console
