entityGenCompile:
	g++ -std=c++17 entitygen.cpp -o entitygen

entityGenRun:
	entitygen data/demo/config.txt

dataGenCompile:
	g++ -std=c++17 datagen.cpp -o datagen

dataGenRun:
	datagen data/demo/config.txt

view:
	vim data/demo/output/data.csv

clean:
	/bin/rm -rf core.* vgcore.* entitygen datagen

