#!/bin/bash
# benchmark_and_log.sh

# Check if repetitions parameter was provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <repetitions>"
    echo "Example: $0 1000"
    exit 1
fi

# Get repetitions from command line parameter
REPETITIONS=$1

OUTPUT_FILE="benchmark_results.csv"

# Create CSV header
echo "Method,Dataset,Elements,Dimensions,Repetitions,TotalUserTime,TotalSystemTime,AvgUserTime,AvgSystemTime" > $OUTPUT_FILE

for method in 1 2 3; do
    for dataset in datasets/small.csv datasets/medium.csv datasets/large.csv; do
        # Run the normalizer and capture its output
        output=$(./normalize $dataset $method $REPETITIONS)
        
        # Extract information using grep and awk
        elements=$(echo "$output" | grep "Elements" | awk '{print $3}')
        dimensions=$(echo "$output" | grep "Dimensions" | awk '{print $3}')
        total_user=$(echo "$output" | grep "Total User Time" | awk '{print $5}')
        total_system=$(echo "$output" | grep "Total System Time" | awk '{print $5}')
        avg_user=$(echo "$output" | grep "Average User Time" | awk '{print $5}')
        avg_system=$(echo "$output" | grep "Average System Time" | awk '{print $5}')
        
        # Determine method name
        case $method in
            1) method_name="Newton-Raphson" ;;
            2) method_name="SSE" ;;
            3) method_name="Naive" ;;
        esac
        
        # Append to CSV, using the actual repetitions value
        echo "$method_name,$dataset,$elements,$dimensions,$REPETITIONS,$total_user,$total_system,$avg_user,$avg_system" >> $OUTPUT_FILE
    done
done

echo "Benchmark results saved to $OUTPUT_FILE"