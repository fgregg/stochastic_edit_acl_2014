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


#include "wfst-train.h"


class WFST_Trainer_Local : public WFST_Trainer {
 public:
  //inherited methods

  /**
   * trains the machine with the read data
   * no regularization at the moment
   * it updates the weights 
   */
  void train(int outer_max_iterations, double outer_threshold, int inner_max_iterations, 
	    double inner_threshold);

  /**
   * This constructor takes no arguments and initializes nothing. It should never be 
   * used except when using DLIB's wrapper class for the optimization. 
   */
  WFST_Trainer_Local();

  /**
   * This is the main constructor the WFST Trainer. 
   *
   * @param fst the VectorFst to train
   * @param data_formatted the formatted training data
   * @param num_arcs the number of arcs in the machine
   * @param num_features the number of features in the machine
   * @param symbol_bound an upper bound on the largest symbol id in the symbol table 
   * @param features a map from the arc ids to the features that fire on that arc
   * @param weights the initial weights for all the features
   */
  WFST_Trainer_Local(VectorFst<LogArc> *fst,fsa_data data_formatted,int num_arcs, int num_features, int symbol_bound, int num_threads, feature_map features,feature_weights weights, double regularization_coefficient, bool debug_mode);

    
  /* methods for getting the current value and gradient of the machine */

  /**
   * This is a wrapper function for the DLIB optimization functions
   *
   * @param weights the current weight value
   *
   */
  double value(const column_vector &weights) const;

  /**
   * This function wraps the computation of the gradient so that it can be 
   * passed to the dlib library for optimization. It calls gradient2, which
   * does the actual optimization
   *
   * @param weights computes the gradient for the current value of the weights
   *
   */
  const column_vector gradient(const column_vector &weights) const;

    
 private:

  /* internal gradient and value computations */

  void update_arc_weights(const column_vector &weights);
  void update_arc_weights();


  /**
   * Computes the value of the function at the current weight value
   *
   * @param weights the current weight value
   *
   */
  double value2(const column_vector &weights);

  /**
   * This function does the actual computation of the gradient in parallel.
   * The number of threads in set in the constructor and handled through
   * static (???) function. 
   *
   * @param weights the current vlaue of the weights
   *
   */
  const column_vector gradient2(const column_vector &weights);

  void calculate_observed_counts(int exemplar_num, VectorFst<FeatureArc> *fst, 
				VectorFst<LogArc> *fst_log,vector<LogWeight> *alpha, 
				vector<LogWeight> *beta);
  void calculate_expected_counts();

  
  
};




// wraps the value method the WFST Trainer
// so that a member function pointer can be called
// as if it were a function pointer
class Value
{
 public:

  Value (const WFST_Trainer_Local* input)
    {
      trainer = input;
    }

  double operator() ( const column_vector& arg) const
  {
    return trainer->value(arg);
  }

 private:
  const WFST_Trainer_Local* trainer;
};

// wraps the gradient computation of the WFST trainer
// so that a member function pointer can be called as
// if it were a function pointer
class Gradient
{
 public:

  Gradient (const WFST_Trainer_Local* input)
    {
      trainer = input;
    }

  const column_vector operator() ( const column_vector& arg) const
  {
    return trainer->gradient(arg);
  }

 private:
  const WFST_Trainer_Local* trainer;
};

