# DIANA - Fault Tolerance
A study on the fault tolerance of **DI**visive **ANA**lysis, a top-down clustering algorithm.

### Basic Setup
  #### Compile
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

  #### Generate Input
    $ ./datagen <#instances> <optional -f to use float> <#attributes>
  #### Generate Gold output
    $ ./simple_diana -i <input text file> -o <golden binary file> -t <kmeans threshold> -l <iterations>


## DWC (Duplication With Comparison)
Spacial duplication of the DIANA kernel. Two parallel executions are ran and then the results are compared, 
possibly detecting an SDC. If so, a counter is updated in a file.

    $ ./dwc_diana -i <.txt input> -o <.bin output> -d <.txt name to count detected sdcs> [optional: -a <.txt output1> -b <.txt output2>]

## Selective Hardening
Duplication and comparison of critical variables and the operations that update them.

Yet to be implemented.

## Radiation Tests
Before running the benchmark, the script radiation-benchmarks/scripts/install.py should be executed. This script will create the folder to store the log files and create the conf file in /etc/radiation-benchmark.conf

      $ ./radiation_diana <-i input file> <-o output file> <-g golden file> <-t kmeans threshold>
    
* Configuration used for Core i7-7700HQ (simple_diana takes 3.5 seconds with this processor)
  - input and gold generated
    $ ./datagen 7000 -f 20
    $ ./simple_diana -i 7000_20f.txt -o diana_gold.bin -t 0.001
  - configurations executed
    $ ./radiation_diana -i 7000_20f.txt -o diana_out.bin -g diana_gold.bin -t 0.001 -l 1000000