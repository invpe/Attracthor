#!/bin/bash


# Generate 10000 digits of Pi using bc
echo "scale=10000; 4*a(1)" | bc -l | tr -d '\\n' | fold -w1 > pi_digits.txt
