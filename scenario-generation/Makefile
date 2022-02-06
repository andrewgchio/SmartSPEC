all:
	g++ -std=c++17 main.cpp -o main

run:
	main data/demo/config.txt

entityGenCompile:
	g++ -std=c++17 entitygen.cpp -o entitygen

entityGenRun:
	entitygen data/demo/config.txt

view:
	vim data/demo/output/data.csv

clean:
	/bin/rm -rf core.* vgcore.* main entitygen
