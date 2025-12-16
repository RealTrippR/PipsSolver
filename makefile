DEBUG_FLAGS := -g

compile:
	g++ $(DEBUG_FLAGS) pipsSolver.cpp -o pipsSolver.exe

run:
	./pipsSolver.exe

clean:
	del *.exe