#!/bin/bash
#  qsub -shell n -b y -V -N work1_"$i"_"$trh" -cwd ./SimLYSO input.mac 
  qsub -shell n -b y -V -N work -cwd ./SimLYSO input.mac 
