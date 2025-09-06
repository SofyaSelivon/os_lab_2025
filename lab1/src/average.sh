#!/bin/bash

sum=0
count=$#

for num in "$@"; do
    if [[ ! $num =~ ^[0-9]+$ ]]; then
        echo "Error: '$num' is not a valid number" >&2
        exit 1
    fi
    ((sum += num))
done


average=$((sum / count))

echo "Sum = $sum"
echo "Count = $count"
echo "Average = $average"