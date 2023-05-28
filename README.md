# SimulatedAnnealingPlacer

This project is a simple simulated annealing-based placer that minimizes the total wirelength. It studies the effect of cooling rate on the quality of the placement.

To run the program, clone the repo, compile the C++ code, then run the executable.

For linux:
Run `g++ placer.cpp -o  placer.out` then `./placer.out` in terminal.

For Windows:
Run `g++ placer.cpp -o placer.exe` then `placer.exe` in CMD.

You will be asked to enter the test name and the cooling rate. Note that you need to have the test file in the same directory as the executable.

![image](https://github.com/abdallahabdelaziz1/SimulatedAnnealingPlacer/assets/67734659/0237398c-3a6e-457f-ab4a-576c0f48e0d1)

The project prints the initial placement and wire lenght. Then it optimizes the placement and prints the final placement and total wire length.
