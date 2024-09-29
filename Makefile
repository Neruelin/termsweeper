

build_freeglut:
	cd ./freeglut-3.6.0 && sudo cmake . && sudo make install

build_gl: build_freeglut  
	gcc gl.c -o gl -lglut

build_gl_only:  
	gcc gl.c -o gl -lGL -lglut

build_term: build_freeglut
	gcc termsweeper.c -o termsweeper -lGL -lglut

build_term_only:
	gcc termsweeper.c -o termsweeper -lGL -lglut

run_gl: build_gl_only
	./gl

run_term: build_term_only
	./termsweeper

run: run_gl

clean:
	rm ./gl || rm ./termsweeper || cd ./freeglut-3.6.0 && make clean