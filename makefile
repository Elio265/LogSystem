logexe : main.cc
	g++ -o $@ $^ -std=c++11 -g

.PHONY : clean
clean:
	rm -rf logexe
