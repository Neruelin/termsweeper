

build: ## build project
	gcc termsweeper.c -o termsweeper

run: build
	./termsweeper