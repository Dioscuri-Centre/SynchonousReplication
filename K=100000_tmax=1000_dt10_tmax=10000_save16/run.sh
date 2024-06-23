#!/bin/bash

# syntax: ./run.sh  common_directory_name

mkdir $1
cp quasi_synchr_division_one_species.cpp $1/$1_quasi_synchr_division_one_species.cpp
cp submit.sh $1/submit.sh
cp run.sh $1/run.sh
cd $1
if g++ ../quasi_synchr_division_one_species.cpp -O3 -w -o $1.exe ; then
#	qsub ./submit.sh test.exe xx s 0 2 1.0 0.05 1
	for s in 0 0.02 0.04 0.05 0.08 0.1 0.2
	do
#		for i in {1..5}
#		do
#			echo $s $i
			qsub -N quasi_synchr ./submit.sh ./$1.exe $1"_dev_"$s"_s.dat" s 0 10 1.0 $s 1
#		done
	done
	qsub -N quasi_synchr ./submit.sh ./$1.exe $1"_exp.dat" e 0 10 1.0 0 1
else
echo "error when compiling the program"
fi

cd ..
