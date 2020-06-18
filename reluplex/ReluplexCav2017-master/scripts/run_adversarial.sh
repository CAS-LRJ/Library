#!/bin/bash

netfile=./nnet/ACASXU_run2a_1_1_batch_2000.nnet
summary=./logs/adversarial_summary.csv
TIMEOUT=60m

for point in 0 3; do
    for delta in 100 75 50 25 10 ; do
        for exec in ./check_properties/adversarial/adversarialPure.elf ./check_properties/adversarial/adversarialBOXSym.elf ./check_properties/adversarial/adversarialTOPO.elf ; do
            f_delta=`python -c "print(${delta}/1000.0)"`
            echo timeout --foreground --signal=SIGQUIT ${TIMEOUT} ${exec} ${summary} ${netfile} ${point} ${f_delta}
            timeout --foreground --signal=SIGQUIT ${TIMEOUT} ${exec} ${summary} ${netfile} ${point} ${f_delta}
        done
    done
done

#./check_properties/adversarial/adversarialBOXSym.elf logs/adversarial_summary.txt nnet/ACASXU_run2a_1_1_batch_2000.nnet 0 0.1  2>&1 | tee logs/adversarial_stats.txt
#
#./check_properties/adversarial/adversarialTOPO.elf logs/adversarial_summary.txt nnet/ACASXU_run2a_1_1_batch_2000.nnet 0 0.1  2>&1 | tee logs/adversarial_stats.txt
#
#./check_properties/adversarial/adversarialPure.elf logs/adversarial_summary.txt nnet/ACASXU_run2a_1_1_batch_2000.nnet 0 0.1  2>&1 | tee logs/adversarial_stats.txt
