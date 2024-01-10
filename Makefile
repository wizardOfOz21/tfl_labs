.PHONY: generate build-project run test valgrind-run run-fuzz
.SILENT: run

generate:
	mkdir build
	make build-project

build-project:
	cd ./build && cmake .. && make --no-print-directory

run:
	./build/lab5 -g grammars/arith_grammar.txt -i "a a a" -a

valgrind-run:
	valgrind --tool=memcheck make run

check:
	cppcheck --language=c++ ./project/src/*.cpp ./main.cpp

dev:
	docker run --rm -it -v $(PWD):/project --name app tfl_labs-app

stop-docker:
	docker stop tfl_labs-app

run-docker-compose:
	docker-compose up -d --build