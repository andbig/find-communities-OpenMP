#!/bin/bash

INPUT_FILE=$1
INPUT_FORMAT=$2
OUTPUT_FILE=$3
BENCHMARK_RUNS=$4
MAX_THREADS=$5
OTHER_OPTIONS="-i 0.001"

echo "Collecting performance figures in $OUTPUT_FILE using $INPUT_FILE as input graph"
echo "Collecting performance figures in $OUTPUT_FILE using $INPUT_FILE as input graph" > $OUTPUT_FILE

echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS"
echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS" >> $OUTPUT_FILE
OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 0 $OTHER_OPTIONS | grep -E 'Average modularity obtained|Algorithm version|Number of threads:                |Average execution time|Minimum execution time' )"
echo $OUTPUT >> $OUTPUT_FILE
echo $OUTPUT
SEQUENTIAL_MODULARITY_AVERAGE="$(echo $OUTPUT | awk '{print $7}')"
echo "Average modularity: " $SEQUENTIAL_MODULARITY_AVERAGE
SEQUENTIAL_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $11}')"
SEQUENTIAL_TIME_AVERAGE=${SEQUENTIAL_TIME_AVERAGE::-1}
echo "Average time: " $SEQUENTIAL_TIME_AVERAGE
SEQUENTIAL_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $15}')"
SEQUENTIAL_TIME_MINIMUM=${SEQUENTIAL_TIME_MINIMUM::-1}
echo "Minimum time: " $SEQUENTIAL_TIME_MINIMUM

printf "\n\n"

echo "Running: ./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 1 -t 1 $OTHER_OPTIONS"
echo "Running: ./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 1 -t 1 $OTHER_OPTIONS" >> $OUTPUT_FILE
OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 1 -t 1 $OTHER_OPTIONS | grep -E 'Average modularity obtained|Algorithm version|Number of threads:                |Average execution time|Minimum execution time' )"
echo $OUTPUT >> $OUTPUT_FILE
echo $OUTPUT
MODULARITY="$(echo $OUTPUT | awk '{print $14}')"
echo "Average modularity: " $MODULARITY

SINGLE_THREAD_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $18}')"
SINGLE_THREAD_TIME_AVERAGE=${SINGLE_THREAD_TIME_AVERAGE::-1}
echo "Average time: " $SINGLE_THREAD_TIME_AVERAGE

SINGLE_THREAD_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $22}')"
SINGLE_THREAD_TIME_MINIMUM=${SINGLE_THREAD_TIME_MINIMUM::-1}
echo "Minimum time: " $SINGLE_THREAD_TIME_MINIMUM

COMPUTATION="scale=10; $SEQUENTIAL_TIME_AVERAGE / $SINGLE_THREAD_TIME_AVERAGE * 100"
SPEEDUP_AVERAGE_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
COMPUTATION="scale=10; $SEQUENTIAL_TIME_MINIMUM / $SINGLE_THREAD_TIME_MINIMUM * 100"
SPEEDUP_MINIMUM_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"

echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%"
echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%" >> $OUTPUT_FILE

printf "\n\n"

for i in $(seq 2 $MAX_THREADS);
do
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 1 -t $i $OTHER_OPTIONS"
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 1 -t $i $OTHER_OPTIONS" >> $OUTPUT_FILE
	OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 1 -t $i $OTHER_OPTIONS | grep -E 'Average modularity obtained|Algorithm version|Number of threads:                |Average execution time|Minimum execution time' )"
	echo $OUTPUT
	echo $OUTPUT >> $OUTPUT_FILE
	MODULARITY="$(echo $OUTPUT | awk '{print $14}')"
	echo "Average modularity: " $MODULARITY
	echo $OUTPUT >> $OUTPUT_FILE
	THREADS_TIME="$(echo $OUTPUT | awk '{print $18}')"
	THREADS_TIME=${THREADS_TIME::-1}
	echo "Average time: " $THREADS_TIME
	COMPUTATION="scale=10; $SINGLE_THREAD_TIME_AVERAGE / $THREADS_TIME * 100"
	SPEEDUP_AVERAGE="$(echo "$COMPUTATION" | bc)"
	
	COMPUTATION="scale=10; $SEQUENTIAL_TIME_AVERAGE / $THREADS_TIME * 100"
	SPEEDUP_AVERAGE_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	
	THREADS_TIME="$(echo $OUTPUT | awk '{print $22}')"
	THREADS_TIME=${THREADS_TIME::-1}
	echo "Minimum time: " $THREADS_TIME
	COMPUTATION="scale=10; $SINGLE_THREAD_TIME_MINIMUM / $THREADS_TIME * 100"
	SPEEDUP_MINIMUM="$(echo "$COMPUTATION" | bc)"
	
	COMPUTATION="scale=10; $SEQUENTIAL_TIME_MINIMUM / $THREADS_TIME * 100"
	SPEEDUP_MINIMUM_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	
	echo $i "threads --> Speedup compared to one thread = Computed on average: " $SPEEDUP_AVERAGE "% - Computed on minimum = " $SPEEDUP_MINIMUM "%"
	echo $i "threads --> Speedup compared to one thread = Computed on average: " $SPEEDUP_AVERAGE "% - Computed on minimum = " $SPEEDUP_MINIMUM "%" >> $OUTPUT_FILE
	
	echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%"
	echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%" >> $OUTPUT_FILE

	
	printf "\n\n"
done

echo "Running: ./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 2 -t 1 $OTHER_OPTIONS"
echo "Running: ./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 2 -t 1 $OTHER_OPTIONS" >> $OUTPUT_FILE
OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f 2 -b $BENCHMARK_RUNS -a 2 -t 1 $OTHER_OPTIONS | grep -E 'Average modularity obtained|Algorithm version|Number of threads:                |Average execution time|Minimum execution time' )"
echo $OUTPUT >> $OUTPUT_FILE
echo $OUTPUT
MODULARITY="$(echo $OUTPUT | awk '{print $16}')"
echo "Average modularity: " $MODULARITY
SINGLE_THREAD_TIME_AVERAGE="$(echo $OUTPUT | awk '{print $20}')"
SINGLE_THREAD_TIME_AVERAGE=${SINGLE_THREAD_TIME_AVERAGE::-1}
echo "Average time: " $SINGLE_THREAD_TIME_AVERAGE

SINGLE_THREAD_TIME_MINIMUM="$(echo $OUTPUT | awk '{print $24}')"
SINGLE_THREAD_TIME_MINIMUM=${SINGLE_THREAD_TIME_MINIMUM::-1}
echo "Minimum time: " $SINGLE_THREAD_TIME_MINIMUM

printf "\n\n"

for i in $(seq 2 $MAX_THREADS);
do
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 2 -t $i $OTHER_OPTIONS"
	echo "Running: ./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 2 -t $i $OTHER_OPTIONS " >> $OUTPUT_FILE
	OUTPUT="$(./find-communities-OpenMP $INPUT_FILE -f $INPUT_FORMAT -b $BENCHMARK_RUNS -a 2 -t $i $OTHER_OPTIONS | grep -E 'Average modularity obtained|Algorithm version|Number of threads:                |Average execution time|Minimum execution time' )"
	echo $OUTPUT >> $OUTPUT_FILE
	echo $OUTPUT
	MODULARITY="$(echo $OUTPUT | awk '{print $16}')"
	echo "Average modularity: " $MODULARITY
	THREADS_TIME="$(echo $OUTPUT | awk '{print $20}')"
	THREADS_TIME=${THREADS_TIME::-1}
	echo "Average time: " $THREADS_TIME
	COMPUTATION="scale=10; $SINGLE_THREAD_TIME_AVERAGE / $THREADS_TIME * 100"
	SPEEDUP_AVERAGE="$(echo "$COMPUTATION" | bc)"
	
	COMPUTATION="scale=10; $SEQUENTIAL_TIME_AVERAGE / $THREADS_TIME * 100"
	SPEEDUP_AVERAGE_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	
	THREADS_TIME="$(echo $OUTPUT | awk '{print $24}')"
	THREADS_TIME=${THREADS_TIME::-1}
	echo "Minimum time: " $THREADS_TIME
	COMPUTATION="scale=10; $SINGLE_THREAD_TIME_MINIMUM / $THREADS_TIME * 100"
	SPEEDUP_MINIMUM="$(echo "$COMPUTATION" | bc)"
	
	COMPUTATION="scale=10; $SEQUENTIAL_TIME_MINIMUM / $THREADS_TIME * 100"
	SPEEDUP_MINIMUM_SEQUENTIAL="$(echo "$COMPUTATION" | bc)"
	
	echo $i "threads --> Speedup = Computed on average: " $SPEEDUP_AVERAGE "% - Computed on minimum = " $SPEEDUP_MINIMUM "%"
	echo $i "threads --> Speedup = Computed on average: " $SPEEDUP_AVERAGE "% - Computed on minimum = " $SPEEDUP_MINIMUM "%" >> $OUTPUT_FILE
	
	echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%"
	echo $i "threads --> Speedup compared to sequential = Computed on average: " $SPEEDUP_AVERAGE_SEQUENTIAL "% - Computed on minimum = " $SPEEDUP_MINIMUM_SEQUENTIAL "%" >> $OUTPUT_FILE

	printf "\n\n"
done