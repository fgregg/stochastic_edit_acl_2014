#!/bin/bash

#/Users/ryancotterell/Documents/research/wfst-train/data/markus/symbol_tables
train_num=$1;
lambda=$2;
backoff=$3;
feature_file="typo_features";
if [ $backoff == "yes" ];
then 
    feature_file="typo_features_backoff";
fi

test_num=100;
outer_iterations=20;
inner_iterations=5;
./sandbox data/typos/train1.2 $train_num data/typos/dev1.2 $test_num data/markus/symbol_tables/syms_in data/markus/symbol_tables/syms_out  experiment020/contextual_edit.fst experiment020/arc_to_action experiment020/$feature_file $outer_iterations $inner_iterations $lambda $train_num $backoff
