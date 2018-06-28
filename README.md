# DIANA - Fault Tolerance
A study on the fault tolerance of **DI**visive **ANA**lysis, a top-down clustering algorithm.

## DWC (Duplication With Comparison)
Spacial duplication of the DIANA kernel. Two parallel executions are ran and then the results are compared, 
possibly detecting an SDC.

Yet to be implemented.

## Selective Hardening
Duplication and comparison of critical variables and the operations that update them.

Yet to be implemented.

## Radiation Tests

* How to compile
  - $ mkdir build
  - $ cd build
  - $ cmake ..
  - $ make

Compilation should generate 3 binaries:
  - datagen: used to generate the input array, it will generate an array stored in a text file.
  - simple_diana: used to generate the gold output. Gold will be used to check if the execution generates a correct output.
  - radiation_diana: this binary will be used during radiation test, it will execute the benchmark kernel several times

* How to generate input data and gold output
  - Input data
    $ ./datagen 3500 <optional -f to use float> <#attributes>
  - Gold output
    $ ./simple_diana <-i name of input text file> <-o name of golden output binary file> <-t kmeans threshold> <-l loop iterations>

* How to run a test

Before running the benchmark, the script radiation-benchmarks/scripts/install.py should be executed. This script will create the folder to store the log files and create the conf file in /etc/radiation-benchmark.conf

  $ ./radiation_diana <-i input file> <-o output file> <-g golden file> <-t kmeans threshold>
        - input file: text file for the unsigned int array
        - golden file: binary file with the gold output

* Configuration used for Core i7-7700HQ (simple_diana takes 3.5 seconds with this processor)
  - input and gold generated
    $ ./datagen 7000 -f 20
    $ ./simple_diana -i 7000_20f.txt -o diana_gold.bin -t 0.001
  - configurations executed
    $ ./radiation_diana -i 7000_20f.txt -o diana_out.bin -g diana_gold.bin -t 0.001 -l 1000000