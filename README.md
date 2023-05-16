# Hash function comparison
## Description & Purpose
A program for testing different hash function implementations and researching ways of optimizing hash table on the task of searching words in literature texts.

## Research summary
The main purpose of the project was
1. Comparing different hash functions by their value distributions.
2. Researching ways of hash table optimization.

Distribution experiment reports:
 - RUS: [DISTRIBUTION_RESULTS_RUS.md](DISTRIBUTION_RESULTS_RUS.md)

Optimization experiment report:
 - RUS: [PERFORMANCE_RESULTS_RUS.md](PERFORMANCE_RESULTS_RUS.md)

## Building

Compile the project (ubuntu linux):

`$ make CASE_FLAGS="[flags]"`

Valid flags:
 - `-D DISTRIBUTION_TEST` - output key distribution in the table,
 - `-D PERFORMANCE_TEST` - perform series of tests measuring time it takes the table to respond to the key search request,
 - `-D TESTED_HASH=[hash_function_hame]` - specify the hash function to use. List of available hash functions - [src/hash/hash_functions.h](src/hash/hash_functions.h),
 - `-D OPTIMIZATION_LEVEL=[0 ... 3]` - optimization stage of the project, matching optimization stages performed during performance experiment (see [PERFORMANCE_RESULTS_RUS.md](PERFORMANCE_RESULTS_RUS.md)),
 - `-D BUCKET_COUNT=[int]` - specify the number of buckets used in the table (default 2027),
 - `-D TEST_COUNT=[int]` - specify the number of tests performed (default 30),
 - `-D TEST_REPETITION=[int]` - specify how many times the algorithm stage will run each test (default 2000)

Run the program (ubuntu linux):

`$ make run`

Reset the project to its original state:

`$ make rm`

## Legal information
### Code of Conduct
For information about our community goals read [**CODE_OF_CONDUCT.md**](CODE_OF_CONDUCT.md).
### Licensing
Project is distributed under MIT license. More licensing information is specified in file [**LICENSE**](LICENSE).
### Contributing
We don't think anyone will be contributing to this project as it was made purely for educational purposes.
But if you still want to contribute you can learn how to do so by reading the file [**CONTRIBUTING.md**](CONTRIBUTING.md).
### Contacts
**(author)** Kudryashov Ilya - *kudriashov.it@phystech.edu*
