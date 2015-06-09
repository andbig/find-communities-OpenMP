#include <stdio.h>

#include "parse-args.h"
#include "community-development.h"

void set_default(settings *s) {
	s->input_file = NULL;
	s->graph_type = NOT_WEIGHTED;
	s->minimum_phase_improvement = 0;
	s->minimum_iteration_improvement = 0;
	s->output_graphs_file = NULL;
	s->number_of_threads = 1;
}

void print_help(char *prog_name) {
	printf("\n--- Usage:\n\n"
			"%s input-file [options]\n\n"
			"Available options:\n\n"
			"\t-h\t:\tShows help (Ignores the rest of the input while doing so)\n"
			"\t-w\t:\tInput graph is weighted (Only available option as of now). Default is non-weighted\n"
			"\t-t\t:\tNumber of threads to use during parallel execution (Must be greater than zero, default 1)\n"
			"\t-p number\t:\tStop phase analysis when phase improvement is smaller than number. Number must be between 0.0 and 1.0\n"
			"\t-i number\t:\tStop iteration (over all nodes) analysis when iteration improvement is smaller than number. Number must be between 0.0 and 1.0\n"
			"\t-o file\t:\tISave graphs obtained in each phase in file\n\n"
			"Input file must have the format:\n\n"
			"source-node destination-node [edge-weight]\n\n"
			"Indexes must be non negative, and weights should be greater than zero. The output is undefined if these conditions are not met\n\n", prog_name);
}

int parse_args(int argc, char *argv[], settings *s){
	int valid = 1;
	int i;

	set_default(s);

	if(argc < 2) {
		printf("Wrong number of arguments!\n");

		valid = 0;
	}

	if(valid) {
		s->input_file = argv[1];

		for(i=2; valid && i < argc; i++) {
			if(argv[i][0] == '-') {
				switch(argv[i][1]){

				case 'h':
					valid = 0;
					break;

				case 'w':
					s->graph_type = WEIGHTED;
					break;

				case 't':
					if(i + 1 < argc) {
						i++;
						s->number_of_threads = atoi(argv[i]);

						if(s->number_of_threads <= 0) {
							printf("Invalid number of threads: '%s'", argv[i]);

							valid = 0;
						}
					} else {
						printf("Expected number of threads after '%s'!\n", argv[i]);
						valid = 0;
					}
					break;

				case 'p':
					if(i + 1 < argc) {
						i++;
						s->minimum_phase_improvement = atof(argv[i]);

						if(!valid_minimum_improvement(s->minimum_phase_improvement)) {
							printf("Invalid minimum phase improvement: '%s'", argv[i]);

							valid = 0;
						}
					} else {
						printf("Expected minimum phase improvement after '%s'!\n", argv[i]);
						valid = 0;
					}
					break;

				case 'i':
					if(i + 1 < argc) {
						i++;
						s->minimum_iteration_improvement = atof(argv[i]);

						if(!valid_minimum_improvement(s->minimum_iteration_improvement)) {
							printf("Invalid minimum iteration improvement: '%s'", argv[i]);

							valid = 0;
						}
					} else {
						printf("Expected minimum iteration improvement after '%s'!\n", argv[i]);
						valid = 0;
					}
					break;

				case 'o':
					if(i + 1 < argc) {
						i++;
						s->output_graphs_file = argv[i];
					} else {
						printf("Expected output file name after '%s'!\n", argv[i]);
						valid = 0;
					}
					break;
				default:
					printf("Invalid option '%s'!\n", argv[i]);
					valid = 0;
				}
			} else {
				printf("Invalid input: %s\n", argv[i]);

				valid = 0;
			}
		}
	}

	if(!valid)
		print_help(argv[0]);

	return valid;
}
