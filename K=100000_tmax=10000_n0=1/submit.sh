#!/bin/sh

#$ -cwd
#$ -l h_rt=24:00:00

#ulimit -c unlimited

./$@

