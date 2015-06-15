# find-communities-OpenMP

*************************************************************************************************************************

Release notes:

This release includes the definition of a framework for the development of solutions to 
the problem of finding communities in large graphs. Currently the only implemented methods 
focus on the Louvain method, but there is room for expansion. 

This framework is intended to be a tool to study the problem, develop, implement and 
analyze new solutions, both sequential and parallel. 

In this release there are three different algorithms implemented. 
 - Sequential Louvain
 - Parallel (Merge & Sort)
 - Parallel (Naive Partitioning)
The user is able to choose the preferred one at program start with the -a option. 
More information is contained in the help (that is shown using the -h option). 

There are also options to control the number of threads, and several execution options.

The implementation also easily allow to control the flow execution to create hybrids from 
the existing solutions and adapt in the best way to the input graph properties. This is, 
however, left to the programmer discretion and, as of now, no runtime tuning is implemented.

*************************************************************************************************************************
Usage:

./find-communities-OpenMP input-file [options]

Available options:

	-h             Shows help
	                 * Ignores the rest of the input while doing so.
	-f number      Use file format identified by number.
	                 * Default is 0
	                 * Complete list of available file format options
	                   can be found below
	-t             Number of threads to use during parallel execution.
	                 * Must be greater than zero
	                 * Default is 1
	-a number      Execute the version of the algorithm identified by number.
	                 * Complete list of available algorithm versions can
	                   be found below
	                 * Default is 1
	-v             Enable verbose logging.
	-p number      Stop phase analysis when phase improvement is smaller
	               than number.
	                 * Number must be between 0.0 and 1.0
	-i number      Stop phase internal iterations when iteration
	               improvement is smaller than number.
	                 * Number must be between 0.0 and 1.0
	-e number      Enable execution option identified by number.
	                 * Complete list of available execution options
	                   can be found below
	-c file        Save communities obtained in each phase in file.
	-g file        Save graphs obtained in each phase in file.
	-b number      Turn benchmarking on.
	                 * Perform the given number of benchmark runs
	                 * Disable file outputs and screen logging
	                 * Disable verbose logging

Available algorithm versions:

	-a 0          Sequential
	                 * Original Louvain method implementation optimized
	-a 1          Parallel (Sort & Select)
	                 * Iterations over all nodes are done in parallel,
	                   potential transfers are sorted and selected by
	                   computed modularity increase
	-a 2          Parallel (Naive partitioning on first phase)
	                 * Partition the input graph during first phase
	                   and performed and optimized Louvain version
	                   over all partitions. Next phases are performed
	                   using sequential implementation

Available execution options:

	-e 0          Use a number of partitions equal to the smallest
	               power of two greater or equal to the number of threads
	               during parallel sorting.
	                 * By default a number of partitions equal to the biggest
	                   power of two smaller or equal to the number of threads
	               	   is used
	                 * Applies only to parallel algorithm

Available file format options:

	-f 0          Edge list, not weighted.
	                 * i.e. each line is of the form:
	                   source-node destination-node
	-f 1          Edge list, weighted.
	                 * i.e. each line is of the form:
	                   source-node destination-node edge-weight
	-f 2          Metis format.

Indexes must be non negative, and weights should be greater than zero.

Output is undefined if any of the above conditions is not met.

*************************************************************************************************************************

For benchmarking purposes, I included also the bash script collect-performance-figures.sh

Usage:

./collect-performance-figures.sh input-graph-file file-format output-benchmarks-file benchmark-runs scale-up-to-threads


