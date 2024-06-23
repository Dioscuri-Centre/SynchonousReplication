#!/bin/sh

#$ -cwd
#$ -l h_rt=48:00:00

#ulimit -c unlimited

./$@

