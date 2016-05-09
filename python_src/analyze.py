#!/usr/bin/python

import sys

def main():
    
    key_words = ["REGULARIZATION COEFFICIENT","NUMBER OF TRAINING EXEMPLARS","BACKOFF FEATURES","NUMBER FEATURES"]
 
    if sys.argv[2] == "log":
        key_words.append("LOG")
    elif sys.argv[2] == "edit":
        key_words.append("EDIT")

    results = {}
    f = open(sys.argv[1])
  
    key_list = []
    for line in f:
        line = line.strip("\n")
        for i,key_word in enumerate(key_words):
            if key_word in line:
                #print line
                key_list.append(line.split("\t")[1])
    
    f.close()
    #experiment020_dev.sh.o7782252
    name = sys.argv[1].split(".")[0][10:]
    if len(key_list) == 5:
        print name + "\t" + "\t".join(key_list)

if __name__ == "__main__":
    main()
