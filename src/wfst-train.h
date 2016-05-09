// Licensed  under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: ryan.cotterell@gmail.com (Ryan Cotterell)
//

#include <pthread.h>
#include <iostream>
#include <fstream>

#include <string>
#include <set>
#include <cmath>
#include <cfloat>
#include <stdlib.h>
#include <stddef.h>
#include <map>
#include <vector>
#include <time.h>
#include <algorithm>

//OpenFST
#include <fst/fstlib.h>
#include <fst/fst-decl.h>
#include <fst/map.h>

//Boost
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

//DLIB
#include <dlib/optimization.h>

//Glog
//#include <glog/logging.h>

#include "data-io.h"

using namespace fst;
using namespace std;
using namespace boost;

//this is a hack to get backpointers
typedef ExpectationWeight<LogWeight,LogWeight > FeatureWeight;
typedef ArcTpl<FeatureWeight> FeatureArc;

typedef WeightConvertMapper<LogArc, StdArc> Map_LS;
typedef WeightConvertMapper<StdArc, LogArc> Map_SL;

typedef pair<VectorFst<FeatureArc>, VectorFst<FeatureArc> > datum_formatted_features;
typedef vector<VectorFst<FeatureArc> >  fsa_data_features;

typedef pair<VectorFst<LogArc>, VectorFst<LogArc> > datum_formatted;
typedef vector<datum_formatted> fsa_data;


/* //taken from http://dlib.net/optimization_ex.cpp.html */
typedef dlib::matrix<double,0,1> column_vector;

//http://compbio.mit.edu/spimap/pub/spimap/src/common.h

// computes log(a + b) given log(a) and log(b)
inline double logadd(double lna, double lnb)
{
  if (lna == 1.0)
    return lnb;
  if (lnb == 1.0)
    return lna;
    
  double diff = lna - lnb;
  if (diff < 500.0)
    return log(exp(diff) + 1.0) + lnb;
  else
    return lna;
}


// computes log(a + b) given log(a) and log(b)
inline double logsub(double lna, double lnb)
{
  if (lna == 1.0)
    return lnb;
  if (lnb == 1.0)
    return lna;
    
  double diff = lna - lnb;
  if (diff < 500.0) {
    double diff2 = exp(diff) - 1.0;
    if (diff2 == 0.0)
      return -INFINITY;
    else
      return log(diff2) + lnb;
  } else
    return lna;
}


//computes the dot product of two vectors in real space
//first argument is ints
//second is double
inline double dot_product(vector<int> *a, feature_weights *b) {
  if (a->size() != b->size()) {
    //die
  }
  double total = 0.0;
  
  for (int i = 0; i < a->size(); ++i) {
    total += (*a)[i] * exp(-(*b)[i]);
  }
  return total;
}


class WFST_Trainer {
 protected:
  training_data data;
  
  //formatted data and the features
  fsa_data data_formatted;
  fsa_data_features data_formatted_features;

  //feature map and feature weights
  feature_map features;
  feature_weights weights;

  //stats about the machine to be trained
  int num_arcs;
  int num_features;
  
  int symbol_bound;
  int num_threads;

  //reguarlization coefficient
  double regularization_coefficient; 

  //whether to operate in debug mode
  bool debug_mode;

  //global members for tallying counts
  vector<double> observed_counts;
  vector<double> expected_counts;
  vector< vector<double> > expected_arc_traversals;

  //fsts
  VectorFst<FeatureArc> fst_feature;
  VectorFst<LogArc> *fst;
  
  //functions
  double mult_features_by_weights(int arc_id);
  double mult_features_by_weights(int arc_id, const column_vector &weights);

  
  void add_training_data(fsa_data data_formatted);

  
  //calculates the expected counts
  virtual void calculate_observed_counts(int exemplar_num, VectorFst<FeatureArc> *fst, 
				VectorFst<LogArc> *fst_log,vector<LogWeight> *alpha, 
					 vector<LogWeight> *beta) {}
  /**
   * Calculates the expected counts
   */
  virtual void calculate_expected_counts(int exemplar_num) {} 

 public:

  //updates the weights
  //this is different for globally and locally normalized models
  virtual void update_arc_weights(const column_vector &weights) {
    cout << "ORIGINAL\n";
  }
  virtual void update_arc_weights() {}


  virtual void train(int outer_max_iterations, double outer_threshold, int inner_max_iterations, 
		    double inner_threshold) {}
  void write(string file_out);
  static int get_number_arcs(VectorFst<LogArc> *fst);
  static void string_to_logfst(string str, VectorFst<LogArc> *fst,
			       SymbolTable *isyms, SymbolTable *osyms);
  
  static void string_to_stdfst(string str, VectorFst<StdArc> *fst, 
			       const SymbolTable *syms);
  static string fst_to_string(VectorFst<StdArc> *fst);
  static void map_to_feature_arcs(VectorFst<LogArc> *fst, 
				 VectorFst<FeatureArc> *new_fst, bool zero);
  
};
