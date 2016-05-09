#!/bin/bash

name=$1;
#qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.0 yes
#lambda=$2;
for i in {1..9}
do 
    x=$(($i * 100))
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.1 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.2 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 3.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.1 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.2 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 3.0 no

done 
for i in {1..6}
do 
    x=$(($i * 1000))
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.1 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.2 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.0 yes 
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.5 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 3.0 yes
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.1 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.2 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 0.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 1.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.0 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 2.5 no
    qsub -q text.q -cwd -l num_proc=4,mem_free=5G,h_vmem=5G,h_rt=24:00:00 $name $x 3.0 no


done 
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_1000.sh
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_2000.sh
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_3000.sh
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_4000.sh
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_5000.sh
#qsub -q text.q -cwd -l num_proc=4,mem_free=20G,h_vmem=20G,h_rt=60:00:00 typo_experiment1_6000.sh
