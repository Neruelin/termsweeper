build_term: ## build project
	gcc termsweeper.c -o termsweeper

build_freeglut:
	cd ./freeglut-3.6.0 && sudo cmake . && sudo make install

build_gl: build_freeglut  
	gcc gl.c -o gl -lglut

build_gl_only:  
	gcc -v -I /home/tyler/github/termsweeper/freeglut-3.6.0/include -lglut -L/home/tyler/github/termsweeper/freeglut-3.6.0/lib -l:libglut.so -l:libglut.so.3 -l:libglut.so.3.12.1 gl.c -o gl

run_term: build
	./termsweeper

run_gl: build_gl_only
	./gl

build: build_gl

run: run_gl

clean:
	rm ./gl || rm ./termsweeper || cd ./freeglut-3.6.0 && make clean