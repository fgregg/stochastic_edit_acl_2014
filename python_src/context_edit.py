#!/usr/local/bin/python

import sys
import fst as  pyfst
import itertools


def sharp_at_end(t,STOP_START_CHAR):
    """
    checks if a string violates a sharp condition
    """
    sharp_seen = False
    for i in t:
        if i == STOP_START_CHAR:
            sharp_seen = True
        if i != STOP_START_CHAR and sharp_seen == True:
            return False

    return True

def sharp_at_beginning(t,STOP_START_CHAR):
    """
    checks if a string violates a sharp condition
    """
    sharp_seen = True
    for i in t:
        if i != STOP_START_CHAR:
            sharp_seen = False
        if i == STOP_START_CHAR and sharp_seen == False:
            return False

    return True
        

def key_to_string(stuff):
  key = "".join(stuff[0]) + "-" + "".join(stuff[1]) + "-" + "".join(stuff[2])
  return key

def all_stop_start(context,STOP_START_CHAR):
    for i in context:
        if i != STOP_START_CHAR:
            return False

    return True

def add(context,new,limit):
    new_context = [new]
    for i,c in enumerate(context):
        new_context.append(c)


    return tuple(new_context[0:limit])


def add_reverse(context,new,limit):
    new_context = []
    for i,c in enumerate(context):
        new_context.append(c)

    new_context.append(new)
    return tuple(new_context[0:limit])


def bump(context,new,limit):
    new_context = [new]
    for i,c in enumerate(context):
        if i == 0:
            continue
        new_context.append(c)

    return tuple(new_context[0:limit])


def bump2(context):
    new_context = []
    for i,c in enumerate(context):
        if i == 0:
            continue
        new_context.append(c)
    
    return tuple(new_context)


def main():
    added_arcs = {}

    #upper left context
    ulc_max =  int(sys.argv[1])
    #upper right context
    urc_max =  int(sys.argv[2])
    #lower left context
    llc_max =  int(sys.argv[3])

    V1 = []
    V2 = []
    #read in the upper alphabet
    """
    file_in = open(sys.argv[4])
    for line in file_in:
        line = line.rstrip("\n")
        V1.append(line)
    file_in.close()
    file_out = open(sys.argv[5])
    for line in file_out:
        line = line.rstrip("\n")
        V2.append(line)
    file_out.close()
    """

    syms_in = open(sys.argv[4])
    syms_out = open(sys.argv[5])

    state_label = open(sys.argv[6],'w')
    arc_label = open(sys.argv[7],'w')
    debug = open(sys.argv[8],'w')
    arc_to_action = open(sys.argv[9],'w')

    for line in syms_in:
        line = line.rstrip("\n")
        V1.append(line)
    syms_in.close()


    
    for line in syms_out:
        line = line.rstrip("\n")
        V2.append(line)
    syms_out.close()

    #V1 = ['a','b']
    #V2 = ['a','b']
 

    STOP_START_CHAR = "#"
    #V1 = ['a','b']
    #V2 = ['a','b']
    #V1 = ['a','b','c','d','e','f','g','h','i','j','k',
    #'l','m','n','o','p','q','r','s','t','u','v','w',
    #     'x','y','z']


    #creates the symbol table
    syms_in = pyfst.SymbolTable()
    syms_out = pyfst.SymbolTable()
    
    #overload epsilon

    syms_in["<EPS>"] = 0
    syms_out["<EPS>"] = 0
    syms_in[STOP_START_CHAR] = 1
    syms_out[STOP_START_CHAR] = 1
    
    for i,v in enumerate(V1):
        syms_in[v] = i + 2
    for i,v in enumerate(V2):
        syms_out[v] = i + 2


    contextual_edit = pyfst.LogTransducer(syms_in,syms_out)
                
    #arc_map
    arc_map = {}
    state_to_context = {}
    #make machine machine
    
    state_num = 1
    all_lookup = {}
    bold_lookup = {}
    non_bold_lookup = {}
    end_bold_lookup = {}
    #creates bold states
    
    
    print "CONTEXT STARTED!!!"
    for i in range(ulc_max+1):
        for j in range(llc_max+1):
            for k in range(urc_max+1):
                for symbols_ulc in itertools.product(set(V1 + [STOP_START_CHAR]),repeat=ulc_max - i):
                    for symbols_llc in itertools.product(set(V2 + [STOP_START_CHAR]),repeat=llc_max - j):
                        for symbols_urc in itertools.product(set(V1 + [STOP_START_CHAR]),repeat=urc_max - k):
                        
                            #ensure that the sharp is only a 
                            #beginning and ending symbol
                            if not sharp_at_beginning(symbols_ulc,STOP_START_CHAR):
                                continue
                            if not sharp_at_end(symbols_urc,STOP_START_CHAR):
                                continue

                            
                            if len(symbols_ulc) == 0 and ulc_max > 0:
                                continue
                            if len(symbols_llc) == 0 and llc_max > 0:
                                continue
                                
                            if ulc_max > 0 and llc_max > 0 and len(symbols_ulc) == 0 and len(symbols_llc) == 0 and  len(symbols_urc) ==  0:
                                continue

                            contextual_edit.add_state()

                    
                            key = (symbols_ulc,symbols_urc,symbols_llc)
                            state_to_context[state_num] = key
                            
                            #determine if "bold" state
        
                            #print len(symbols_ulc) == ulc_max
                            #print len(symbols_urc) == urc_max
                            #print len(symbols_llc) == llc_max
                            #print
                            if len(symbols_ulc) == ulc_max and len(symbols_urc) == urc_max and len(symbols_llc) == llc_max:
                                                               
                                bold_lookup[(symbols_ulc,symbols_urc,symbols_llc)] = state_num
                            else:
                                non_bold_lookup[(symbols_ulc,symbols_urc,symbols_llc)] = state_num
                            
                            all_lookup[(symbols_ulc,symbols_urc,symbols_llc)] = state_num
                            #print symbols_ulc
                            #print symbols_llc
                            #print symbols_urc
                            #print

                            #ADD FINAL STATES
                            if len(symbols_urc) == urc_max and all_stop_start(symbols_urc,STOP_START_CHAR):
                                    contextual_edit.add_arc(state_num,0,
                                                            syms_in.find(0),syms_out.find(0))
                                    arc_map[(state_num,0,0,0)] = "FINAL"
                    
                            state_num += 1
        #print split
        #print "\t".join(map(lambda x: ''.join(x[i] for i in range(len(x))),[urc_syms,ulc_syms,llc_syms]))
    


    print "CONTEXT DONE!!!"
    contextual_edit[0].initial = False
    contextual_edit[0].final = True
    state_to_context[0] = "-#-";

    #print out states
    for k,v in bold_lookup.items():
        #if k not in end_bold_lookup:
        state_label.write("B: " + key_to_string(k) + "\t" + str(v) + "\n")
        #else:
        #    state_label.write( "F: " + k + "\t" + str(v) + "\n")
    for k,v in non_bold_lookup.items():
        state_label.write( key_to_string(k) + "\t" + str(v) + "\n")



    state_label.write( "0\tFINAL\n")
    

    
    state_label.close()
    
    
    if ulc_max > 0:
        if llc_max > 0:
            contextual_edit[all_lookup[((STOP_START_CHAR,),(),(STOP_START_CHAR,))]].initial = True
        else:
            contextual_edit[all_lookup[((STOP_START_CHAR,),(),())]].initial = True
    else:
        if llc_max > 0:
            contextual_edit[all_lookup[((),(),(STOP_START_CHAR,))]].initial = True
        else:
            contextual_edit[all_lookup[((),(),())]].initial = True

    #####

    ####
    
    for state in bold_lookup:
        ulc = state[0]
        urc = state[1]
        llc = state[2]

        
        
        #INSERTION 
        #bold to bold
        for v2 in V2:
            
            llc_prime = bump(llc,v2,llc_max)
            key = (ulc,urc,llc_prime)

            debug.write("INS\t" + " <EPS>\t" + v2 + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
            arc_label.write( "INS\t" + "<EPS>\t" + v2 + "\t" + key_to_string(key) + "\t" + str(all_lookup[state]) + "\t" + str(all_lookup[key]) + "\n")
            
            contextual_edit.add_arc(all_lookup[state],all_lookup[key],
                                     syms_in.find(0),v2)
            

            arc_map[(all_lookup[state],all_lookup[key],0,syms_out[v2])] = "INS"
            
            #print v2
            #print str(bold_state) + "\t->\t" + str(key)
            #print

        #SUBSTITUTION
        #bold to bold
        for v2 in V2:
            if len(urc) == 0 or  urc[0] == STOP_START_CHAR:
                continue

            ulc_prime = add(bump2(ulc),urc[0],ulc_max)
            urc_prime = bump2(urc)
         
            llc_prime = bump(llc,v2,llc_max)


            key = (ulc_prime,urc_prime,llc_prime)
            
            debug.write("SUB\t" + " <EPS>\t" + v2 + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
            arc_label.write( "SUB\t" + "<EPS>\t" + v2 + "\t" + key_to_string(key) + "\t" + str(all_lookup[state]) + "\t" + str(all_lookup[key]) + "\n")
            
            contextual_edit.add_arc(all_lookup[state],all_lookup[key],
                                     syms_in.find(0),v2)
            arc_map[(all_lookup[state],all_lookup[key],0,syms_out[v2])] = "SUB"
            
            

            
        #DELETION
        #bold to non-bold
       
        if len(urc) == 0 or  urc[0] == STOP_START_CHAR:
            continue

            
        ulc_prime = add(ulc,urc[0],ulc_max)
        urc_prime = bump2(urc)
            
        key = (ulc_prime,urc_prime,llc)
        
        debug.write("DEL\t" + " <EPS>\t<EPS>\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
        arc_label.write( "DEL\t" + "<EPS>\t<EPS>\t" + key_to_string(key) + "\t" + str(all_lookup[state]) + "\t" + str(all_lookup[key]) + "\n")
            
        contextual_edit.add_arc(all_lookup[state],all_lookup[key],
                                    syms_in.find(0),syms_out.find(0))

        
        arc_map[(all_lookup[state],all_lookup[key],0,0)] = "DEL"
            
                #print str(bold_state) + "\t->\t" + str(key)
                #print


    for state in non_bold_lookup:
        ulc = state[0]
        urc = state[1]
        llc = state[2]
        
        for v1 in V1 + [STOP_START_CHAR]:
           
            if v1 == STOP_START_CHAR and urc_max == 0:
                continue


            urc_prime = add_reverse(urc,v1,urc_max)
            key = (ulc,urc_prime,llc)
            
            go = False
            sharp_seen = False
           
            for i in urc_prime:
                
                if i == STOP_START_CHAR:
                    sharp_seen = True
                elif sharp_seen:
                    go = True
            
           

            #print   
            #print key_to_string(state) + "\t" + key_to_string(key)
            #print


             
            if go:
                continue


            debug.write("ABS\t" + v1 + "\t<EPS>\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")


            arc_label.write("ABS\t" + v1 + "\t<EPS>\t" + key_to_string(key) + "\t" + str(all_lookup[state]) + "\t" + str(all_lookup[key]) + "\n")

            contextual_edit.add_arc(all_lookup[state],all_lookup[key],
                                    v1,syms_out.find(0))

            
            arc_map[(all_lookup[state],all_lookup[key],syms_in[v1],0)] = "ABS"
            

    contextual_edit.write("contextual_edit.fst",syms_in,syms_out)
    syms_in.write("symbol_table_in.txt")
    syms_out.write("symbol_table_out.txt")


    arc_label.close()
    state_label.close()
    debug.close()
   
    state_id = 0
    arc_id = 0
    for state in contextual_edit:
        for arc in state:
            key = (state_id,arc.nextstate,arc.ilabel,arc.olabel)
            
            if key in arc_map:
                
                tmp = str(arc_id) + "," + "\t".join(["-".join(["".join(x) for x in state_to_context[state_id]]), "-".join(["".join(x) for x in state_to_context[arc.nextstate]]),str(syms_in.find(key[2])), str(syms_out.find(key[3])), arc_map[key]])
                
                arc_to_action.write(tmp + "\n")
            else:
 		print arc_id               
            
            arc_id += 1
        
        state_id += 1
        
    arc_to_action.close()
    sys.exit(0)




    
    for key,state_id1 in bold_lookup.items():
        ulc_syms,urc_syms,llc_syms = key.split("-")
        #add insertions that stay in the same state
       
        if llc > 0:
            for ins in V2:
                new_key = "-".join([ulc_syms,urc_syms ,llc_syms[1:] + ins])
                
                
                state_id2 = None
                if len(urc_syms) == 0:
                    state_id2 = 0
                    debug.write("INS" +  "\t<EPS>" + ins + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
                    arc_label.write( "INS\t" + "<EPS>\t" + ins + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_out.find(0),ins)

                    
                state_id2 = bold_lookup[new_key]
                debug.write("INS" +  "\t<EPS>" + ins + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
                arc_label.write( "INS\t" + "<EPS>\t" + ins + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                contextual_edit.add_arc(state_id1,state_id2,syms_out.find(0),ins)
        else:
            for ins in V2:
                new_key = "-".join([ulc_syms,urc_syms ,llc_syms[1:]])
                
                state_id2 = None
                if len(urc_syms) == 0:
                    state_id2 = 0
                    debug.write("INS" +  "\t<EPS>" + ins + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
                    arc_label.write( "INS\t" + "<EPS>\t" + ins + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_out.find(0),ins)
                

                state_id2 = bold_lookup[new_key]
                
                
                debug.write("INS" +  "\t<EPS>" + ins + "\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
                arc_label.write( "INS\t" + "<EPS>\t" + ins + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                contextual_edit.add_arc(state_id1,state_id2,syms_out.find(0),ins)


        #adds deletions         
        #if key in end_bold_lookup:
        #    continue

        if ulc > 0:
            #prevents deletion when nothing is observed to the right
            #final state coming
            if len(urc_syms) > 0:
                new_key = "-".join([ulc_syms[1:] + urc_syms[0:1],urc_syms[1:],llc_syms])
                state_id2 = non_bold_lookup[new_key]
            
                if key in end_bold_lookup:
                    state_id2 = bold_lookup[new_key]

                debug.write("DEL" +  "\t<EPS>\t<EPS>\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
                arc_label.write( "DEL\t" +"<EPS>\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")

                contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),syms_out.find(0))
        else:
            new_key = "-".join([ulc_syms[1:],urc_syms[1:],llc_syms])
            state_id2 = non_bold_lookup[new_key]
           
            if key in end_bold_lookup:
                state_id2 = bold_lookup[new_key]

    
            debug.write("DEL" +  "\t<EPS>\t<EPS>\t" + key_to_string(state) + "\t" + key_to_string(key) + "\n")
            arc_label.write( "DEL\t" +"<EPS>\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
        
            contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),syms_out.find(0))


                       
        #if key in end_bold_lookup:
        #continue
        if len(urc_syms) == 0:
            continue

        #adds substitutions

        if llc > 0:
            for sub in V2:
                if ulc > 0:
                    #prevents substitution when nothing is observed to the right
                    #final state coming
                    if len(urc_syms) > 0:
                        new_key = "-".join([ulc_syms[1:] + urc_syms[0:1] ,urc_syms[1:],llc_syms[1:] + sub])
                
                        
                        state_id2 = end_bold_lookup[new_key]
                        arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                        contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)

                        state_id2 = non_bold_lookup[new_key]
                        arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                        contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)


                else:
                    new_key = "-".join([ulc_syms[1:],urc_syms[1:],llc_syms[1:] + sub])

                    state_id2 = end_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)
                    
                    state_id2 = non_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)

        else:
             for sub in V2:
                if ulc > 0:
                    new_key = "-".join([ulc_syms[1:] + urc_syms[0:1] ,urc_syms[1:],llc_syms[1:]])
                
                    state_id2 = end_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)
                    
                    state_id2 = non_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)


                    
                else:
                    new_key = "-".join([ulc_syms[1:],urc_syms[1:],llc_syms[1:]])
                
                    state_id2 = end_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)
                    
                    state_id2 = non_bold_lookup[new_key]
                    arc_label.write( "SUB\t" + "<EPS>\t" + sub + "\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")
                    contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),sub)
                    



    #adds arcs from non-bold states to non-bold states
    for key,state_id1 in non_bold_lookup.items():
        ulc_syms,urc_syms,llc_syms = key.split("-")
    
        if len(urc_syms) < urc - 1:
            for v in V1:
                new_key = "-".join([ulc_syms,urc_syms + v,llc_syms])
                state_id2 = non_bold_lookup[new_key]
      
                arc_label.write( "ABS\t" + v + "\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")     

                contextual_edit.add_arc(state_id1,state_id2,v,syms_out.find(0))
            

     
    #add arcs from non-bold states to bold states
    for key,state_id1 in non_bold_lookup.items():
        ulc_syms,urc_syms,llc_syms = key.split("-")
        
        if urc > 0:
            for v in V1:
                new_key = "-".join([ulc_syms,urc_syms + v,llc_syms])
                state_id2 = bold_lookup[new_key]

                arc_label.write( "ABS\t" + v + "\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")     

                contextual_edit.add_arc(state_id1,state_id2,v,syms_out.find(0))
       
            # add the observed final
            new_key = "-".join([ulc_syms,urc_syms,llc_syms])
            state_id2 = end_bold_lookup[new_key]
            
            arc_label.write( "ABS\t" + "<EPS>\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")     
            contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),syms_out.find(0))
            
        else:
            for v in V1:
                new_key = "-".join([ulc_syms,urc_syms,llc_syms])
                state_id2 = bold_lookup[new_key]
                
                arc_label.write( "ABS\t" + v + "\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")    
                
                contextual_edit.add_arc(state_id1,state_id2,v,syms_out.find(0))
                
            # add the observed final
            new_key = "-".join([ulc_syms,urc_syms,llc_syms])
            state_id2 = end_bold_lookup[new_key]

            
            arc_label.write( "ABS\t" + "<EPS>\t<EPS>\t" + key + "\t" + str(state_id1) + "\t" + str(state_id2) + "\n")    
            contextual_edit.add_arc(state_id1,state_id2,syms_in.find(0),syms_out.find(0))
            


    arc_label.close()

    contextual_edit.write("contextual_edit.fst",syms_in,syms_out)
    syms_in.write("symbol_table_in.txt")
    syms_out.write("symbol_table_out.txt")

    
    fst = pyfst.LogTransducer(syms_in,syms_in)
  
    string = "hallo";
    last = 0
    for i,c in enumerate(list(string)):
        fst.add_arc(i,i+1,c,c)
        last = i
    fst[i + 1].final = True
    fst.write(string + ".fst",syms_in,syms_in)
    result = fst.compose(contextual_edit)
    result.write("result.fst",syms_in,syms_out)

    

if __name__ == "__main__":
    main()
