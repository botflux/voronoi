build:
	cmake -S . -B ./dist
	(cd ./dist && make)