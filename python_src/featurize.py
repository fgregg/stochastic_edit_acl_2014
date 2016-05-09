#!/usr/bin/python

import sys




def main():
    #FEATURE 0 NOTHING FIRES

    features_on_arc = {}
    look_up = {}
    unique_contexts = set([])
    f = open(sys.argv[1])


    features_seen = {}
    feature_counter = 1

    for i,line in enumerate(f):
        line = line.rstrip("\n")
        "1421,---p-p<EPS>ABS"
        arc_id,rest = line.split(",")
        
        #look_up[arc_id] = (s,t,contexts[0],contexts[1],contexts[2])

        feature_vector = []
        if rest.split("\t")[-1] == "ABS":
            #print "TEST"
            features_on_arc[i] = 0

            feature_vector = ["0"]
        else:
            contexts = rest.split("\t")[0].split("-")
    
            s = None
            if rest.split("\t")[-1] == "INS":
                s = "<EPS>"
            else:
                s = rest.split("\t")[0].split("-")[1][0]
                contexts[1] = contexts[1][1:]
        #s = rest.split("\t")[-3]
            t = rest.split("\t")[-2]
     

            cont = [s,t,contexts[0],contexts[1],contexts[2]]
            #unique_contexts.add(cont)
        
            feats = []
            #feature 1 full context
            #print cont
            feats.append("0".join(cont))
            #print " ".join(cont)
            #feature 2,3,4,6,7,8
            # include both s and t
            
            
            feats.append("0".join([s,t,'9',contexts[1],contexts[2]]))
            feats.append("0".join([s,t,contexts[0],'9',contexts[2]]))
            feats.append("0".join([s,t,contexts[0],contexts[1],'9']))
            feats.append("0".join([s,t,'9','9',contexts[2]]))
            feats.append("0".join([s,t,contexts[0],'9','9']))
            feats.append("0".join([s,t,'9',contexts[1],'9']))
            feats.append("0".join([s,t,'9','9','9']))
            
            #feature 9
            #do not include s

            feats.append("0".join(['9',t,contexts[0],contexts[1],'9']))
            feats.append("0".join(['9',t,contexts[0],contexts[1],contexts[2]]))

            #do not include t
            

            feats.append("0".join([s,'9',contexts[0],contexts[1],contexts[2]]))
            feats.append("0".join([s,'9','9',contexts[1],contexts[2]]))
            feats.append("0".join([s,'9',contexts[0],'9',contexts[2]]))
            feats.append("0".join([s,'9','9','9',contexts[2]]))

            
            
            for feature in feats:
                if feature not in features_seen:
                    features_seen[feature] = str(feature_counter)
                    feature_counter += 1
                feature_vector.append(features_seen[feature])
            
        print arc_id + "\t" + ",".join(feature_vector)
    #print feature_counter
    f.close()



        
if __name__ == "__main__":
    main()
