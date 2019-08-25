#!/bin/bash
  qsub -shell n -b y -V -N work -cwd ./SimLYSO input.mac 
