all:
	g++ -std=c++17 entitygen.cpp -o entitygen
	g++ -std=c++17 datagen.cpp -o datagen
	g++ -std=c++17 obsgen.cpp -o obsgen

entitygen:
	g++ -std=c++17 entitygen.cpp -o entitygen

runentitygen:
	entitygen data/demo/config.txt

datagen:
	g++ -std=c++17 datagen.cpp -o datagen

rundatagen:
	datagen data/demo/config.txt

obsgen:
	g++ -std=c++17 obsgen.cpp -o obsgen

runobsgen:
	obsgen data/demo/config.txt

viewdata:
	vim data/demo/output/data.csv

viewobs:
	vim data/demo/output/observations.csv

clean:
	/bin/rm -rf core.* vgcore.* entitygen datagen obsgen

