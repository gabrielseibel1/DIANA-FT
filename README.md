# DIANA - Divisive Analysis

* How to compile
  - $ mkdir build
  - $ cd build
  - $ cmake ..
  - $ make

Compilation should generate 4 binaries:
  - datagen: used to generate the input array, it will generate an array stored in a text file.
  - simple_diana: used to generate the gold output. Gold will be used to check if the execution generates a correct output.
  - radiation_diana: this binary will be used during radiation test, it will execute the benchmark kernel several times
  - quick_timing: this binary is used only for testing purposes, it will measure the time spent in every part of the benchmark. This binary will not record any log information

* How to generate input data and gold output
  - Input data
    $ ./datagen 3500 <optional -f to use float> <#attributes>
  - Gold output
    $ ./simple_diana <-i name of input text file> <-o name of golden output binary file> <-t kmeans threshold>

* How to run a test

Before running the benchmark, the script radiation-benchmarks/scripts/install.py should be executed. This script will create the folder to store the log files and create the conf file in /etc/radiation-benchmark.conf

  $ ./radiation_diana <-i input file> <-o output file> <-g golden file> <-t kmeans threshold>
        - input file: text file for the unsigned int array
        - golden file: binary file with the gold output

* Configuration used for Core i7-7700HQ (simple_diana takes 3.5 seconds with this processor)
  - input and gold generated
    $ ./datagen 3500 -f 20
    $ ./simple_diana -i 3500_20f.txt -o diana_gold.bin -t 0.001
  - configurations executed
    $ ./radiation_diana -i 3500_20f.txt -o diana_out.bin -g diana_gold.bin -t 0.001