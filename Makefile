SHELL=/bin/bash

main:
	g++ -o main main.cpp

randomtest:
	g++ -o randomtest randomtest.cpp

run: main
	./main ./tests.txt

brute: main randomtest
	for i in {0..5}; do ./randomtest > $${i}.txt; ./main $${i}.txt > out$${i} 2> errout$${i}; done
	rm main
	rm randomtest

cleanrandom:
	for i in {0..5}; do rm -f $${i}.txt out$${i} errout$${i}; done

cleanexe:
	rm -f main
	rm -f randomtest
