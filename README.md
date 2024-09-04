# TPSim

**TPSim** (Timing and Power Simulator) is a gate-level simulator with timing and power estimation capabilities. It receives a combinatorial Verilog netlist as input and simulates the circuit in an event-driven fashion, using pre-characterized look-up tables to estimate events' delay, energy, and other parameters.

## Build

### Dependencies

This project mainly depends on Boost (>= 1.80.0). The Verilog parser depends on Flex and Bison. The project also uses Catch2 as the testing framework. On Ubuntu, these libraries can be installed through the following command:

```sh
sudo apt install libboost-all-dev flex bison
```

### Steps

1. Navigate to the project folder
    ```sh
    cd TPSim
    ```
2. Invoke CMake to create the build directory and generate Makefiles
    ```sh
    cmake -S . -B build
    ```
3. Invoke CMake to build the project
    ```sh
    cmake --build build
    ```

## Run

Usage:

```sh
./tpsim <verilog file> -s <stimuli file> [options]
```

Use the flag `--help` or `-h` to display all the options.

Tests can be executed through `ctest` or the compiled binary `./test`.