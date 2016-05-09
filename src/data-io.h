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


#include <cmath>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include<boost/algorithm/string/split.hpp>                                      
#include<boost/algorithm/string.hpp> 


using namespace std;
using namespace boost;

typedef vector<vector<int> > feature_map;
typedef vector<double> feature_weights;
typedef pair<string, string> datum;

typedef vector<datum> training_data ;





/*
 A class that encapsulates reading in the features

 */
class FeatureReader {
 public:
  FeatureReader(string file_in);
  feature_map get_features()  const;
  int get_number_features(); 
 private:
  feature_map features;
  int number_features;


};


class WeightReader {
 public:
  WeightReader(string file_in);
  feature_weights get_weights() const;
 private:
  feature_weights weights;

};


/*
a class that encapsulates reading in the training data
 */
class DataReader {
 public:
  DataReader(string file_in);
  training_data get_data() const;
 private:
  training_data data;
};





