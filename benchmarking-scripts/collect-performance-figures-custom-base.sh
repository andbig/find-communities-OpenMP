#!/bin/bash

# Run in the same folder as find-communities executable (or edit the script accordingly)

INPUT_FILE=$1
INPUT_FORMAT=$2
OUTPUT_FILE=$3
BENCHMARK_RUNS=$4
MAX_THREADS=$5
OTHER_OPTIONS=""
MAX_VERSION=3

echo "Collecting performance figures in $OUTPUT_FILE using $INPUT_FILE as input graph"
echo "Collecting performance figures in $OUTPUT_FILE using $INPUT_FILE as input graph" > $OUTPUT_FILE

echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS"
echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS" >> $OUTPUT_FILE
OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS | grep -E 'Average modularity obtained|Average execution time|Minimum execution time')"
printf "\n"
SEQUENTIAL_MODULARITY_AVERAGE="$(echo $OUTPUT | awk '{print $4}')"
echo "Average modularity: " $SEQUENTIAL_MODULARITY_AVERAGE
echo "Average modularity: " $SEQUENTIAL_MODULARITY_AVERAGE >> $OUTPUT_FILE
SEQUENTIAL_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $8}')"
echo "Average time: " $SEQUENTIAL_TIME_AVERAGE
echo "Average time: " $SEQUENTIAL_TIME_AVERAGE >> $OUTPUT_FILE
SEQUENTIAL_TIME_AVERAGE=${SEQUENTIAL_TIME_AVERAGE::-1}
SEQUENTIAL_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $12}')"
echo "Minimum time: " $SEQUENTIAL_TIME_MINIMUM
echo "Minimum time: " $SEQUENTIAL_TIME_MINIMUM >> $OUTPUT_FILE
SEQUENTIAL_TIME_MINIMUM=${SEQUENTIAL_TIME_MINIMUM::-1}

printf "\n\n"


for j in $(seq 1 $MAX_VERSION);
do
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t 1 $OTHER_OPTIONS"
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t 1 $OTHER_OPTIONS" >> $OUTPUT_FILE
	OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t 1 $OTHER_OPTIONS | grep -E 'Average modularity obtained|Average execution time|Minimum execution time')"
	printf "\n"
	MODULARITY="$(echo $OUTPUT | awk '{print $4}')"
	echo "Average modularity: " $MODULARITY
	echo "Average modularity: " $MODULARITY >> $OUTPUT_FILE

	SINGLE_THREAD_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $8}')"
	echo "Average time: " $SINGLE_THREAD_TIME_AVERAGE
	echo "Average time: " $SINGLE_THREAD_TIME_AVERAGE >> $OUTPUT_FILE
	SINGLE_THREAD_TIME_AVERAGE=${SINGLE_THREAD_TIME_AVERAGE::-1}

	SINGLE_THREAD_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $12}')"
	echo "Minimum time: " $SINGLE_THREAD_TIME_MINIMUM
	echo "Minimum time: " $SINGLE_THREAD_TIME_MINIMUM >> $OUTPUT_FILE
	SINGLE_THREAD_TIME_MINIMUM=${SINGLE_THREAD_TIME_MINIMUM::-1}

	COMPUTATION="scale=10; $SEQUENTIAL_TIME_AVERAGE / $SINGLE_THREAD_TIME_AVERAGE * 100"
	SPEEDUP_AVERAGE_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	COMPUTATION="scale=10; $SEQUENTIAL_TIME_MINIMUM / $SINGLE_THREAD_TIME_MINIMUM * 100"
	SPEEDUP_MINIMUM_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	printf "\n"
	echo "Speedup compared to sequential   Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "%"
	echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM_SEQUENTIAL "%" 
	echo "Speedup compared to sequential   Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "%" >> $OUTPUT_FILE
	echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM_SEQUENTIAL "%" >> $OUTPUT_FILE

	printf "\n\n"

	for i in $(seq 2 $MAX_THREADS);
	do
		echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t $i $OTHER_OPTIONS"
		echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t $i $OTHER_OPTIONS" >> $OUTPUT_FILE
		OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a $j -t $i $OTHER_OPTIONS | grep -E 'Average modularity obtained|Average execution time|Minimum execution time')"
		printf "\n"
		MODULARITY="$(echo $OUTPUT | awk '{print $4}')"
		echo "Average modularity: " $MODULARITY
		echo "Average modularity: " $MODULARITY >> $OUTPUT_FILE

		THREADS_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $8}')"
		echo "Average time: " $THREADS_TIME_AVERAGE
		echo "Average time: " $THREADS_TIME_AVERAGE >> $OUTPUT_FILE
		THREADS_TIME_AVERAGE=${THREADS_TIME_AVERAGE::-1}

		THREADS_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $12}')"
		echo "Minimum time: " $THREADS_TIME_MINIMUM
		echo "Minimum time: " $THREADS_TIME_MINIMUM >> $OUTPUT_FILE
		THREADS_TIME_MINIMUM=${THREADS_TIME_MINIMUM::-1}
	

		COMPUTATION="scale=10; $SINGLE_THREAD_TIME_AVERAGE / $THREADS_TIME_AVERAGE * 100"
		SPEEDUP_AVERAGE="$(echo "$COMPUTATION" | bc)"
	
		COMPUTATION="scale=10; $SEQUENTIAL_TIME_AVERAGE / $THREADS_TIME_AVERAGE * 100"
		SPEEDUP_AVERAGE_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	
		COMPUTATION="scale=10; $SINGLE_THREAD_TIME_MINIMUM / $THREADS_TIME_MINIMUM * 100"
		SPEEDUP_MINIMUM="$(echo "$COMPUTATION" | bc)"
	
		COMPUTATION="scale=10; $SEQUENTIAL_TIME_MINIMUM / $THREADS_TIME_MINIMUM * 100"
		SPEEDUP_MINIMUM_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
		printf "\n"
		echo "Speedup compared to one thread   Computed on average: " $SPEEDUP_AVERAGE "%"
		echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM "%" 
		echo "Speedup compared to sequential   Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "%"
		echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM_SEQUENTIAL "%" 		
		printf "\n"
		echo "Speedup compared to one thread   Computed on average: " $SPEEDUP_AVERAGE "%" >> $OUTPUT_FILE
		echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM "%" >> $OUTPUT_FILE
		echo "Speedup compared to sequential   Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "%" >> $OUTPUT_FILE
		echo "                                 Computed on minimum: " $SPEEDUP_MINIMUM_SEQUENTIAL "%" >> $OUTPUT_FILE
	
		printf "\n\n"
	done
done

