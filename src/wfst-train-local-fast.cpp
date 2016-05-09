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

#include "wfst-train-local-fast.h"



/*
 * Calculates the observed counts (without parallelization)
 *
 * @param exemplar_num - the training examplar number
 * @param fst - the FST in with FeatureArcs
 * @param fst_log - the FST with the actual weights in log space
 * @param alpha - the alpha values
 * @param beta - the beta values
 *
 */
void WFST_Trainer_Local::calculate_observed_counts(int exemplar_num, 
					    VectorFst<FeatureArc> *fst, 
					    VectorFst<LogArc> *fst_log, 
					    vector<LogWeight> *alpha, 
					    vector<LogWeight> *beta) 
{
  //normalizing constant
  LogWeight Z = Times((*alpha)[0], (*beta)[0]);
  
  int arc_id = 0;
  for (StateIterator<VectorFst<FeatureArc> > siter(*fst); 
       !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
    map<int,int> arc_mapper;
    int tmp = arc_id;
    
    for (ArcIterator<VectorFst<FeatureArc> > aiter1(*fst,state_id); 
	 !aiter1.Done(); aiter1.Next()) {
      const FeatureArc &arc = aiter1.Value();
      int old_arc_id = round(exp(-arc.weight.Value2().Value()));
      arc_mapper[tmp] = old_arc_id - 1;
      ++tmp;
    }

    for (ArcIterator<VectorFst<LogArc> > aiter2(*fst_log,state_id); 
	 !aiter2.Done(); aiter2.Next()) {
      const LogArc &arc = aiter2.Value();
      int old_arc_id2 = arc_mapper[arc_id];
      LogWeight val = ((Times(Times((*alpha)[state_id],arc.weight.Value()), 
			      (*beta)[arc.nextstate])));

      double log_val = Divide(val,Z).Value();
      this->observed_counts[old_arc_id2] = logadd(this->observed_counts[old_arc_id2],-log_val);
      ++arc_id;
    }
  }
}


/*
 *
 * Calculates the expected counts
 * based on the observed counts class
 * member. It must be called after
 * calculate observed counts for 
 * meaningful results.
 */
void WFST_Trainer_Local::calculate_expected_counts() {
  int arc_id = 0;

  for (StateIterator<VectorFst<LogArc> > siter(*fst); !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
    int tmp_arc_id = arc_id;

    vector<double> total_traversals(symbol_bound,-DBL_MAX);

    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); !aiter.Done(); aiter.Next()) {

      const LogArc &arc = aiter.Value();
      total_traversals[arc.ilabel] 
	= logadd(total_traversals[arc.ilabel],this->observed_counts[tmp_arc_id]);
      tmp_arc_id += 1;
    }
    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); !aiter.Done(); aiter.Next()) {
      const LogArc &arc = aiter.Value();
      double arc_prob = arc.weight.Value();
     
      this->expected_counts[arc_id] = logadd(this->expected_counts[arc_id],
					     total_traversals[arc.ilabel] - arc_prob);
      arc_id += 1;
    }
  }
}


/*
 * This is the wrapper value2 the optimization library dlib so 
 * it can optimize a pointer to a member function
 *
 * @param weights - the parameters
 * @return the value of the function
 */
double WFST_Trainer_Local::value(const column_vector &weights) const {
  return const_cast<WFST_Trainer_Local*>(this)->value2(weights);
}

/*
 * This computes the function value at a given weight vector.
 * This should be parallelized by dividing the training examples
 * into subsets and doing each on a different thread.
 *
 * @param weights - the value of the weights
 * @param the value of the function
 *
 */
double WFST_Trainer_Local::value2(const column_vector &weights) {
  
  cout << "LIKELIHOOD\n";
  double likelihood = 0.0;
  update_arc_weights(weights);

  // ``filler'' FSTs to replace
  // the results from the composition
  // declared outside for loop for efficiency
  VectorFst<LogArc> medial;
  VectorFst<LogArc> final;
  
  medial.SetInputSymbols(fst->InputSymbols());
  medial.SetOutputSymbols(fst->OutputSymbols());
  final.SetInputSymbols(fst->InputSymbols());
  final.SetOutputSymbols(fst->OutputSymbols());

  for (fsa_data::iterator it = this->data_formatted.begin(); it != this->data_formatted.end(); ++it) {
    datum_formatted exemplar = *it;
    Compose<LogArc>(exemplar.first,*fst,&medial);
    Compose<LogArc>(medial,exemplar.second,&final);
    LogWeight expected_weight = ShortestDistance(final);

    likelihood += expected_weight.Value();
  }

  return (likelihood + this->regularization_coefficient * dlib::dot(weights,weights)) / this->data_formatted.size() 
;
}


/*
 * This is a wrapper for gradient2 in order to remove the const
 * so it can be called by the optimization library dlib
 *
 * @param weights - the weight vector where the gradient should be computed
 * @return the gradient value at that point
 */
const column_vector WFST_Trainer_Local::gradient(const column_vector &weights) const {
  return const_cast<WFST_Trainer_Local*>(this)->gradient2(weights);
}


/*
 * Computes the gardient at a given parameter setting theta
 * @param weights - the weight vector where the gradient should be computed
 * @return the gradient value at that point
 */
const column_vector WFST_Trainer_Local::gradient2(const column_vector &weights) {
  if (debug_mode) {
    cout << "GRAD START\n";
  }

  // number of training exemplars
  int N = this->data_formatted_features.size(); 
  update_arc_weights(weights);

  column_vector gradient(num_features);
  for (int f = 0; f < num_features; ++f) {
    gradient(f) = 1.0 / N * this->regularization_coefficient* weights(f);
  }

  for (int i = 0; i < num_arcs; ++i) {
    this->expected_counts[i] = -DBL_MAX;
  }

  calculate_expected_counts();
  for (int i = 0; i < num_arcs; ++i) {
    for (int j = 0; j < features[i].size(); ++j) {
      int feat = features[i][j];
      //renormalize the observed counts with respect
      //to the empirical distribution 
      //this can't be done earlier because they are
      //needed for the correct computation
      // of the expected counts in a per-state
      // normalized model
      //gradient(feat) += 
      //	1.0 / N * (exp(observed_counts[i]) 
      //	   -  exp(expected_counts[i]));
      gradient(feat) += 1.0 / N * 
	(exp(observed_counts[i]) 
		   -  exp(expected_counts[i]));
    }
  }

  Value value(this);

  if (debug_mode) {
    column_vector g = dlib::derivative(value,1e-4)(weights);
    cout << "GRADIENT COMPARISON\n";
    for (int i = 0; i < num_features; ++i) {
      cout << gradient(i) << "\t"  <<  g(i);
      cout << "\n";
    }
  }

  return gradient;
}


/*
 * Trains the model based on the training data passed in
 *
 * @param outer_max_iterations - the maximum number of iterations the outer M step 
can perform
 * @param outer_threshold - the convergence threshold for the outer M step
 * @param inner_max_iterations - the maximum number of iterations the inner M step
 can perform
 * @param inner_threshold - the convergence threshold for the inner M step
 */
void WFST_Trainer_Local::train(int outer_max_iterations, double outer_threshold, int inner_max_iterations, 
                               double inner_threshold) {
  //number of training exemplars
  int N = this->data_formatted_features.size(); 
  //this is the weight vector that will be optimized
  column_vector tmp_weights(num_features);
  for (int i = 0; i < num_features; ++i) {
    tmp_weights(i) = weights[i];
  }

  //initilize data structures outside
  //of the for loop
  vector<LogWeight> alpha, beta;
  VectorFst<LogArc> medial_log, final_log;
      
  medial_log.SetInputSymbols(fst->InputSymbols());
  medial_log.SetOutputSymbols(fst->InputSymbols());
  final_log.SetInputSymbols(fst->OutputSymbols());
  final_log.SetOutputSymbols(fst->OutputSymbols());
    

  //E - STEP
  Value value(this);
  Gradient gradient(this);
    
  for (int iteration_num = 0; iteration_num < outer_max_iterations; ++iteration_num) {
    cout << "OUTER ITERATION: " << iteration_num << "\n";
    for (int i = 0; i < num_arcs; ++i) {
      this->observed_counts[i] = -DBL_MAX;
    }
    //iterates over the exemplars. 
   
    for (int exemplar_num = 0; exemplar_num < this->data_formatted.size(); ++exemplar_num) {
      VectorFst<FeatureArc> exemplar_feature = data_formatted_features[exemplar_num];
      datum_formatted exemplar_log = this->data_formatted[exemplar_num];
      Compose<LogArc>(exemplar_log.first,*fst,&medial_log);
      Compose<LogArc>(medial_log,exemplar_log.second,&final_log);
      ShortestDistance(final_log, &alpha);
      ShortestDistance(final_log, &beta,true);
      calculate_observed_counts(exemplar_num,&exemplar_feature, 
				&final_log, &alpha, &beta);

    }
   
    //IF IN DEBUG MODE BE VERBOSE
    if (this->debug_mode) {
      cout << "START OPT\n";
      
      dlib::find_min(dlib::lbfgs_search_strategy(10),
		     dlib::objective_delta_stop_strategy(inner_threshold,inner_max_iterations).be_verbose(),  
		     value, gradient,
		     tmp_weights, -1);
      
      cout << "END OPT\n";
    }
    //IF NOT IN DEBUG MODE DO NOT BE VERBOSE
    else {
      dlib::find_min(dlib::lbfgs_search_strategy(10),
		     dlib::objective_delta_stop_strategy(inner_threshold,inner_max_iterations).be_verbose(),  
		     value, gradient,
		     tmp_weights, -1);
      
    }
    //M - step
    update_arc_weights(tmp_weights);
  }
}




/*
 * Takes a vector of weights and updates the arc weights based on the features
 * that fire on that arc
 *
 * weights = \vec{\theta}
 * features = \vec{f}
 *
 * w(a) \propto \vec{\theta}^T \vec{f}
 *
 */
void WFST_Trainer_Local::update_arc_weights() {
  int arc_id = 0;
  for (StateIterator<VectorFst<LogArc> > siter(*fst); !siter.Done(); siter.Next()) {
    int state_id = siter.Value();
    
    //total outgoing probability of all arcs from state i
    //map<int,double> total_outgoing_probability;
    vector<double> total_outgoing_probability(num_arcs,-DBL_MAX);
    int tmp_arc_id = arc_id; //store because of two for loops
    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); 
	 !aiter.Done(); aiter.Next()) {
      const LogArc &cur_arc = aiter.Value();
      total_outgoing_probability[cur_arc.ilabel] = 
	logadd(total_outgoing_probability[cur_arc.ilabel],
	       -mult_features_by_weights(tmp_arc_id));
      tmp_arc_id += 1;
      
    }
    
    for (MutableArcIterator<VectorFst<LogArc> > aiter(fst,state_id); 
	 !aiter.Done(); aiter.Next()) {
      
      const LogArc &cur_arc = aiter.Value();
      double new_weight = (mult_features_by_weights(arc_id) + 
			   total_outgoing_probability[cur_arc.ilabel]);
      const LogArc arc = LogArc(cur_arc.ilabel,cur_arc.olabel,
				new_weight,cur_arc.nextstate);
      aiter.SetValue(arc);
      ++arc_id;
    }
  }
}

/*
 * Updates the weights on the arcs of the 
 * FST being trained
 *
 * @param weights - the new weight vector
 */
void WFST_Trainer_Local::update_arc_weights(const column_vector &weights) {
   int arc_id = 0;
  for (StateIterator<VectorFst<LogArc> > siter(*fst); !siter.Done(); siter.Next()) {
    int state_id = siter.Value();

    vector<double> total_outgoing_probability(symbol_bound,-DBL_MAX);
    //store because of two for loops
    int tmp_arc_id = arc_id;
    for (ArcIterator<VectorFst<LogArc> > aiter(*fst,state_id); 
	 !aiter.Done(); aiter.Next()) {
      const LogArc &cur_arc = aiter.Value();
      total_outgoing_probability[cur_arc.ilabel] = 
	logadd(total_outgoing_probability[cur_arc.ilabel],-mult_features_by_weights(tmp_arc_id,weights));
      tmp_arc_id += 1;
      
    }
    for (MutableArcIterator<VectorFst<LogArc> > aiter(fst,state_id); 
	 !aiter.Done(); aiter.Next()) {
      const LogArc &cur_arc = aiter.Value();
      double new_weight = mult_features_by_weights(arc_id,weights) + total_outgoing_probability[cur_arc.ilabel];
      LogArc arc = LogArc(cur_arc.ilabel,cur_arc.olabel,new_weight,cur_arc.nextstate);
      aiter.SetValue(arc);
      ++arc_id;
    }
  }
}


/*
 * Empty constructor used for the dlib
 * optimization library
 */
WFST_Trainer_Local::WFST_Trainer_Local() {

}

/*
 * Constructor..
 *
 * @param fst - the fst to be trained
 * @param data_formatted - the data as probablistic FSAs (allows training
with noisy data)
 * @param num_arcs - the number of arcs in the machine
 * @param num_features - the number of features in the machine
 * @param symbol_bound - the highest integer id assigned to a symbol in the symbol table
 * @param num_threads - the number of threads of the parallelization
 * @param features - the arc -> feature vecor data structure
 * @param weights - the feature_id -> weight data structure
 * @param debug_mode - whether to run in debug_mode
 */
WFST_Trainer_Local::WFST_Trainer_Local(VectorFst<LogArc> *fst,fsa_data data_formatted,int num_arcs, int num_features, int symbol_bound, int num_threads, feature_map features,feature_weights weights, double regularization_coefficient, bool debug_mode) {
  this->fst = fst;
  this->data = data;
  this->weights = weights;
  this->features = features;

  this->num_arcs = num_arcs;
  this->num_features = num_features;

  this->symbol_bound = symbol_bound;
  this->num_threads = num_threads;

  this->debug_mode = debug_mode;

  this->regularization_coefficient = regularization_coefficient;

  observed_counts.resize(num_arcs);
  expected_counts.resize(num_arcs);
  expected_arc_traversals.resize(data_formatted.size());

  update_arc_weights();
		
  fst_feature.SetInputSymbols(fst->InputSymbols());
  fst_feature.SetOutputSymbols(fst->OutputSymbols());
  map_to_feature_arcs(fst,&fst_feature,false);

  cout << "ADD TRAINING DATA\n";
  this->add_training_data(data_formatted);
  cout << "CONSTRUCTOR ENDED\n";
}
