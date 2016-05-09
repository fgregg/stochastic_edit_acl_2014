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

#include "data-io.h"


/*
reads a file in the following format and converts it to a feature_map
arc_id (int) \t {0,1},{0,1},...{0,1}

only should read in binary features

 */
FeatureReader::FeatureReader(string file_in) {
  ifstream  in(file_in.c_str());
  

  vector< string > vec;
  string line;

  //maximum feature number
  //this makes the assumption that
  //the features are not just arbitrary integers
  // and were assigned in a sane manner
  int feature_max = 0;


  while (getline(in,line)) {
    vector<string> tokens;
    split(tokens, line, is_any_of("\t"));
    
    if (tokens.size() > 1) {
      
      if (tokens[0].at(0) != '#' && tokens.size() == 2) {
	int arc_num = atoi(tokens[0].c_str());
	vector<string> features_str;
	split(features_str,tokens[1],is_any_of(","));
	

	vector<int> feature_vec;
	for (int i = 0; i < features_str.size(); ++i) {
    	  
	  
	  if (atoi(features_str[i].c_str()) > feature_max) {
	    feature_max = atoi(features_str[i].c_str());
	  }

	  feature_vec.push_back(atoi(features_str[i].c_str()));
	}

	features.push_back(feature_vec);
	
      }
    }
  }

  in.close();

  //add one since feature indexing could start at 0
  this->number_features = feature_max + 1; 
}

/*
Returns the immutable feature map
 */
feature_map FeatureReader::get_features() const {
  return features;
}


/*
Returns the number of features
 */
int FeatureReader::get_number_features() {
  return this->number_features;
}

/*
reads a file in the following format and converts it to feature_weights
arc_id (int) \t float

only should read in binary features

 */
WeightReader::WeightReader(string file_in) {
  ifstream  in(file_in.c_str());
  

  vector< string > vec;
  string line;

  
  while (getline(in,line)) {
    vector<string> tokens;
    split(tokens, line, is_any_of("\t"));
    
    if (tokens.size() > 1) {
      
      if (tokens[0].at(0) != '#' && tokens.size() == 2) {
	int arc_num = atoi(tokens[0].c_str());
	double val = atof(tokens[1].c_str());
	weights.push_back(val);

	
      }
    }
  }
  in.close();


}

/*
Returns the immutable feature map
 */
feature_weights WeightReader::get_weights() const {
  return weights;
}



/*
reads a file in the following format and converts it to training_data
string1 \t string2
string1 \t string2

only should read in binary features

 */
DataReader::DataReader(string file_in) {
  ifstream  in(file_in.c_str());
  

  vector< string > vec;
  string line;


  while (getline(in,line)) {
    vector<string> tokens;
    split(tokens, line, is_any_of("\t"));
    
    if (tokens.size() > 1) {
      
      if (tokens[0].at(0) != '#' && tokens.size() == 2) {
       
	data.push_back(datum(tokens[0],tokens[1]));
	
      }
    }
  }
  in.close();

}

/*
Returns the immutable feature map
 */
training_data DataReader::get_data() const {
  return data;
}
