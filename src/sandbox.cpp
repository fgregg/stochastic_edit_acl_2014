// Licensed under the Apache License, Version 2.0 (the "License");
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


#include "sandbox.h"


int main(int argc, char * argv[]) {

  string data_in_train  = argv[1];

  int num_training_examples = atoi(argv[2]);
  string data_in_test = argv[3];
  int num_test_examples = atoi(argv[4]);

  string upper_syms_in = argv[5];
  string lower_syms_in = argv[6];
  
  string fst_in = argv[7];
  string arc_map = argv[8];
  
  //random number seed for debugging
  int seed = 0; 

  FeatureReader fr = FeatureReader(argv[9]);
  
  int outer_iterations = atoi(argv[10]);
  int inner_iterations = atoi(argv[11]);

  double regularization_coefficient = atof(argv[12]);

  string number_of_training_exemplars = argv[13];
  string backoff_features = argv[14];


  //WeightReader wr = WeightReader(argv[4]);
  
  //Create object of csv_parser and pass the filename as the argument.
  csv_parser csv_train(data_in_train);
  csv_parser csv_test(data_in_test);

  //read in the data_string
  vector<pair<string,string> > data_string_train;
  vector<pair<string,string> > data_string_test;
  vector<datum_formatted> training_data;
  vector<datum_formatted> test_data;

  for (int i = 1; i <= num_training_examples; ++i) {
    pair<string,string> p(csv_train.get_value(i,1),csv_train.get_value(i,2));
    data_string_train.push_back(p);
  }

  for (int i = 1; i <= num_test_examples; ++i) {
    pair<string,string> p(csv_test.get_value(i,1),csv_test.get_value(i,2));
    data_string_test.push_back(p);
  }


  //reader in arc mapper
  map<int,string> arc_mapper;
  csv_parser csv_arc_mapper(arc_map);

  ifstream  arc_mapper_in(arc_map.c_str());
  

  vector< string > vec;
  string line;

 
  while (getline(arc_mapper_in,line)) {
    vector<string> tokens;
    split(tokens, line, is_any_of(","));
    arc_mapper[atoi(tokens[0].c_str())] = tokens[1];
  }
  arc_mapper_in.close();



  SymbolTable *upper = SymbolTable::ReadText(upper_syms_in);
  SymbolTable *lower = SymbolTable::ReadText(lower_syms_in);

  
  //read in the FST for training
  cout << "READING IN FST\n";
  VectorFst<LogArc> *fst = VectorFst<LogArc>::Read(fst_in);
  fst->SetInputSymbols(upper);
  fst->SetOutputSymbols(lower);

  cout << "CONVERTING TRAINING EXAMPLES\n";
  //Read in the training data
  for (int i = 0; i < num_training_examples; ++i) {
    VectorFst<LogArc> fst1,fst2;

    fst1.SetInputSymbols(upper);
    fst1.SetOutputSymbols(upper);
    fst2.SetInputSymbols(lower);
    fst2.SetOutputSymbols(lower);
    std::reverse(data_string_train[i].second.begin(),data_string_train[i].second.end());
    WFST_Trainer_Local::string_to_logfst(data_string_train[i].first,&fst1,upper, upper);
    WFST_Trainer_Local::string_to_logfst(data_string_train[i].second,&fst2,lower,lower);
    training_data.push_back(datum_formatted(fst1,fst2));

    stringstream ss;
    ss << i;
    string id = ss.str();
    // fst1->Write("first" + id + ".fst");
    //fst2->Write("second" + id + ".fst");

    StdVectorFst tmp1;
    StdVectorFst tmp2;

    ArcMap(fst1,&tmp1,Map_LS());
    ArcMap(fst2,&tmp2,Map_LS());

    //tmp1.Write("first_std" + id + ".fst");
    //tmp2.Write("second_std" + id + ".fst");
  }

  cout << "CONVERTING TEST DATA\n";

  //Read in the test data
  for (int i = 0; i < num_test_examples; ++i) {
    VectorFst<LogArc> fst1,fst2;

    fst1.SetInputSymbols(upper);
    fst1.SetOutputSymbols(upper);
    fst2.SetInputSymbols(lower);
    fst2.SetOutputSymbols(lower);
    std::reverse(data_string_test[i].second.begin(),data_string_test[i].second.end());
    WFST_Trainer_Local::string_to_logfst(data_string_test[i].first,&fst1,upper, upper);
    WFST_Trainer_Local::string_to_logfst(data_string_test[i].second,&fst2,lower,lower);
    test_data.push_back(datum_formatted(fst1,fst2));
  }


  
  feature_map features = fr.get_features();
  cout << "REGULARIZATION COEFFICIENT:\t" << regularization_coefficient << "\n";
  cout << "NUMBER OF TRAINING EXEMPLARS:\t" << number_of_training_exemplars << "\n";
  cout << "BACKOFF FEATURES:\t" << backoff_features << "\n";
  int num_features = fr.get_number_features();
  cout << "NUMBER FEATURES:\t" <<  num_features << "\n";

  feature_weights weights(num_features);
  
  srand(seed);

  for (int i = 0; i < num_features; ++i) {
    vector<int> v;
    v.push_back(i);
 
    weights[i] = 0.0; //rand() % 5;
  }
  
  int num_arcs = WFST_Trainer::get_number_arcs(fst);

  cout << features[0].size() << "\n";

  cout << "TRAINING INIT\n";
  cout << "NUM FEATURES\t" << num_features << "\n";
  cout << "NUM ARCS\t" << num_arcs << "\n";

  WFST_Trainer *wfst_trainer = new WFST_Trainer_Local(fst,training_data,num_arcs, num_features, 100, 1, features,weights,regularization_coefficient, false);

  cout << "STARTING TRAINING\n";
  //wfst_trainer->write("untrained.fst");
  wfst_trainer->train(outer_iterations,1e-5, inner_iterations,1e-5);
  //wfst_trainer->write("trained.fst");
  cout << "FINISHED TRAINING\n";

  //FORMAT RYAN
  /*
  Decoder<LogArc> decoder(fst,num_features);
  for (int i = 0; i < num_data_points; ++i) {
    string one_best =  decoder.one_best(data[i].first);
    string input = data_string[i].first;
    string output = data_string[i].second;
    cout << input  << "\t" << output  << "\t" << one_best << "\n";
    string decoded =  decoder.alignment_decode(data[i].first);
    vector<string> split;
    boost::split(split,decoded,is_any_of(" "));
    for (vector<string>::iterator it = split.begin(); it != split.end(); ++it) {
      cout << arc_mapper[atoi(it->c_str())] << "\n";
    }
  }
  */
  //FORMAT JASON

  Evaluator evaluator(fst,num_features,test_data);
 
  cout << "LOG LOSS:\t" << evaluator.log_loss() << "\n";
  cout << "EDIT:\t" << evaluator.one_best_edit() << "\n";
  cout << "EXPECTED EDIT:\t" << evaluator.expected_edit_distance() << "\n";

  /*
  for (int i = 0; i < num_test_examples; ++i) {
    string one_best =  evaluator.one_best(test_data[i].first);
    string input = data_string_test[i].first;
    string output = data_string_test[i].second;
    //cout << input  << "\n";
    //cout << one_best << "\n";

    pair<string,double> p = evaluator.alignment_decode(test_data[i].first);
    pair<string,double> p_forced = evaluator.alignment_decode_forced(test_data[i].first,test_data[i].second);
    string decoded = p.first;
    boost::trim(decoded);
    string decoded_forced = p_forced.first;
    boost::trim(decoded_forced);
    

    pair<string,string> aligned = aligner(decoded,arc_mapper);
    pair<string,string> aligned_forced = aligner(decoded_forced,arc_mapper);

    
    cout << aligned.first << endl << aligned.second << "\t" << p.second << endl;
    cout << aligned_forced.second << "\t" << p_forced.second << endl << aligned_forced.first << endl;
    cout << endl << endl;
    //cout << p_forced.first << "\t" << p_forced.second << endl;
    }
  */
}
/*
  computes the alignment for an input string and its Viterbi decode
 */
pair<string,string> aligner(string decoded, map<int,string> arc_mapper) {
  
    vector<string> split;
    boost::split(split,decoded,is_any_of(" "));


    queue<string> upper;
    string upper_string = "";
    string lower_string = "";
    
    
    int iter = 0;
    for (vector<string>::iterator it = split.begin(); it != split.end(); ++it) {
      vector<string> arc_id_split;
      
      boost::split(arc_id_split,arc_mapper[atoi(it->c_str())],is_any_of("\t"));
      if (arc_id_split[4] == "FINAL") {
	
	string lower_char = arc_id_split[3];
	if (lower_char.compare("<EPS>") == 0) {
	  lower_char = " ";
	}
	upper_string += upper.front();
	lower_string += lower_char;

      }
      //IF CONTEXT ABSORBING ARC
      else if (arc_id_split[4] == "ABS") {
	string upper_char = arc_id_split[2];
	boost::trim(upper_char);

	//make <EPS> space
	if (upper_char.compare("<EPS>") == 0) {
	  string upper_char = " ";
	}

	upper.push(upper_char);
      }
      else if (arc_id_split[4] == "INS") {
	string upper_char = " ";
	string lower_char = arc_id_split[3];


	boost::trim(lower_char);

		
	upper_string += upper_char;
	lower_string += lower_char;
	
      } else {
	string lower_char = arc_id_split[3];

	boost::trim(lower_char);
	if (lower_char.compare("<EPS>") == 0) {
	  lower_char = " ";
	}
		
	upper_string += upper.front();
	lower_string += lower_char;
	
	if (upper.size() > 0) {
	  upper.pop();
	}
      }
      ++iter;

      
    }
    
    return pair<string,string>(upper_string,lower_string);


}


