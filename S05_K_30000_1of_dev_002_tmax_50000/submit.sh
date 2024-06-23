#!/bin/sh

#$ -cwd
#$ -l h_rt=036:00:00

#ulimit -c unlimited

./$@

