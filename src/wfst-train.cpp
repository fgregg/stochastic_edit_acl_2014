// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the Liccense for the specific language governing permissions and
// limitations under the License.
//
// Author: ryan.cotterell@gmail.com (Ryan Cotterell)
//

#include "wfst-train.h"


/*
 * Takes an arc id and multplies the weights by the 
 * features on that arc. Uses the weights in the 
 * instantiation
 *
 * @param arc_id the arc id
 * @return the resulting dot product
 */
double WFST_Trainer::mult_features_by_weights(int arc_id) {
    double total = 0.0;
    for (int f = 0; f < this->features[arc_id].size(); ++f) {
      int i = this->features[arc_id][f];
      total += this->weights[i];
    }
    return total;
  }

/*
 * Multiplies the features associated with a given arcs 
 * with the weight vector
 *
 * @param arc_id - the unique arc id
 * @param column_vector - the weights
 * @return the resulting dot product
 */
double WFST_Trainer::mult_features_by_weights(int arc_id, 
					      const column_vector &weights) {
    double total = 0.0;
    for (int f = 0; f < this->features[arc_id].size(); ++f) {
      int i = this->features[arc_id][f];
      total += weights(i);
    }
    return total;
}



/*
 * This function clears the current training data 
 * and adds new training data. This obviates the need
 * to reinstantiate the trainer for the same machine just
 * because the training data has changed. This is useful
 * in scenarios where the training comprises the M step
 * in an EM procedure. 
 *
 * @param data_formatted the formatted training data
 *
 */
void WFST_Trainer::add_training_data(fsa_data data_formatted) {
  this->data_formatted.clear();
  this->data_formatted = data_formatted;
  data_formatted_features.clear();
  for (fsa_data::iterator it = this->data_formatted.begin(); 
       it != this->data_formatted.end(); ++it) {
    datum_formatted exemplar = *it;
    VectorFst<FeatureArc> fst_upper_features;
    VectorFst<FeatureArc> fst_lower_features;
    
    fst_upper_features.SetInputSymbols(fst->InputSymbols());
    fst_upper_features.SetOutputSymbols(fst->InputSymbols());
    
    fst_lower_features.SetInputSymbols(fst->OutputSymbols());
    fst_lower_features.SetOutputSymbols(fst->OutputSymbols());
    
    map_to_feature_arcs(&exemplar.first,&fst_upper_features,true);
    map_to_feature_arcs(&exemplar.second,&fst_lower_features,true);

    VectorFst<FeatureArc> medial_features;
    VectorFst<FeatureArc> final_features; 
    
    medial_features.SetInputSymbols(fst_feature.InputSymbols());
    medial_features.SetOutputSymbols(fst_feature.OutputSymbols());
    final_features.SetInputSymbols(fst_feature.OutputSymbols());
    final_features.SetOutputSymbols(fst_feature.OutputSymbols());
    
    Compose<FeatureArc>(fst_upper_features,fst_feature,&medial_features);
    Compose<FeatureArc>(medial_features,fst_lower_features,&final_features);
        
    data_formatted_features.push_back(final_features);
  }
}


/*
 * Returns the number of arcs in an FST
 * 
 * @param fst - an fst whose arcs are to be counted
 * @return the number of arcs in the given fst
 */
int WFST_Trainer::get_number_arcs(VectorFst<LogArc> *fst) {
  int arc_count = 0;
  for (StateIterator<VectorFst<LogArc> > siter(*fst); !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
   
    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); !aiter.Done(); aiter.Next()) {
      ++arc_count;
    }
  }
  return arc_count;

}


/*
 * 
 *  Turns a string into a VectorFst<FeatureArc> with (1,1) 
 * that is the multiplicative *   identity of both elements 
 *  of the pair
 * 
 *  @param str - the string
 *  @param fst - the target fst
 *  @param syms - the symbol table
*/
void WFST_Trainer::string_to_stdfst(string str, VectorFst<StdArc> *fst, 
				   const SymbolTable *syms) {
  int state_id = 0;
  fst->SetInputSymbols(syms);
  fst->SetOutputSymbols(syms);
  
  for (int i = 0; i < str.length(); ++i) {
    string s = str.substr(i,1);
    if (state_id == 0) {
      fst->AddState();
      fst->SetStart(0);
    }
    fst->AddState();
    StdArc feature_arc = StdArc(syms->Find(s),syms->Find(s), 
				TropicalWeight::One(),state_id + 1);
    fst->AddArc(state_id,feature_arc);
    ++state_id;
  }
  fst->SetFinal(state_id,TropicalWeight::One());
}


/*
 * Takes a linear chain FST and converts the lower language to a string
 *
 * @param fst - converts an FST to a string (assume FSA, ilabel and olabel are the same on every arc)
 * @return the string
 */
string WFST_Trainer::fst_to_string(VectorFst<StdArc> *fst) {
  string output = "";
  for (StateIterator<VectorFst<StdArc> > siter(*fst); 
       !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
    for (ArcIterator<VectorFst<StdArc> > aiter(*fst,state_id); 
	 !aiter.Done(); aiter.Next()) {
      const StdArc &cur_arc = aiter.Value();
      string label = fst->OutputSymbols()->Find(cur_arc.olabel);
      std::reverse(label.begin(), label.end());
      output = label + output;
    }
  }
  return output;
}



/*
 * Turns a string into a VectorFst<FeatureArc> with (1,1) that is 
 * the multiplicative identity of both elements 
 *  of the pair
 *
 * @param str - the string to convert
 * @param fst - the fst to fill up
 * @param isyms - the input symbol table
 * @param osyms - the output symbol table
 *
*/
void WFST_Trainer::string_to_logfst(string str, VectorFst<LogArc> *fst, 
				   SymbolTable *isyms, SymbolTable *osyms) {
  int state_id = 0;
  for (int i = 0; i < str.length(); ++i) {
    string s = str.substr(i,1);
    if (state_id == 0) {
      fst->AddState();
      fst->SetStart(0);
    }
    fst->AddState();
    
    LogArc feature_arc = LogArc(isyms->Find(s),osyms->Find(s), LogWeight::One(),state_id + 1);
    fst->AddArc(state_id,feature_arc);
    ++state_id;
  }
  
  fst->SetFinal(state_id,LogWeight::One());
  fst->SetInputSymbols(isyms);
  fst->SetOutputSymbols(osyms);
}

/*
 * Takes a FST and replaces the arcs with the FeatureArc
 *
 * @param fst - the fst to map
 * @param new_fst - the result of the mapping
 * @param bool - whether to keep the original weights on the FSt
 *
 */
void WFST_Trainer::map_to_feature_arcs(VectorFst<LogArc> *fst, 
				      VectorFst<FeatureArc> *new_fst, bool zero) {
  for (int state_num = 0; state_num < fst->NumStates(); ++state_num) {
    new_fst->AddState();
    
  }
  new_fst->SetStart(fst->Start());
  
  //map start and end states
  //maps all states to the new states in new machine
  //maps all final to feature
  
  int arc_id = 0;
  for (StateIterator<VectorFst<LogArc> > siter(*fst); !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
    
    if (fst->Final(state_id) != LogWeight::Zero()) {

      FeatureWeight f_final = FeatureWeight(LogWeight::One(),LogWeight::Zero());
      new_fst->SetFinal(state_id,f_final);
    }
    
    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); !aiter.Done(); aiter.Next()) {
      
      const LogArc &arc = aiter.Value();
      int next_state_id = arc.nextstate;
      FeatureWeight f;
      if (!zero) {
	f = FeatureWeight(LogWeight::One(),LogWeight(-log(arc_id + 1)));
      } else {
	f = FeatureWeight(LogWeight::One(),LogWeight::Zero());
      }
      FeatureArc f_arc = FeatureArc(arc.ilabel,arc.olabel,f,next_state_id);    
      new_fst->AddArc(state_id,f_arc);      
      
      ++arc_id;
    }
  }
}




/**
 * Writes the fst being trained by this trainer to disk
 * using OpenFST's write method.
 *
 * @param file_out the filename to use for the writing
 *
 */

void WFST_Trainer::write(string file_out) {
  this->fst->Write(file_out);
}


