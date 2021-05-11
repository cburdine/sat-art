#include "sat.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>

using namespace std;


sat_sentence sat_sentence::from_ksat(int sentence[], int n_clauses, int k ){
    sat_sentence new_sentence;
    
    for(int i = 0; i < n_clauses; ++i){
        new_sentence.clauses.push_back(vector<int>(3,0));
        
        for(int j = 0; j < k; ++j){
            new_sentence.clauses[i][j] = sentence[k*i + j];
        }
    }
    return new_sentence;
}

int sat_sentence::num_variables(){
    int max_var = 0;
    for(auto& cl : clauses){
        for(int lit : cl){
            if(abs(lit) > max_var){
                max_var = abs(lit);
            }
        }
    }
    return max_var;
}

sat_sentence sat_sentence::from_dimacs(istream& is){
    
    sat_sentence new_sentence;
    
    string line;
    int line_no = 0;
    while(getline(is, line)){
        
        ++line_no;
        istringstream iss = istringstream(line);
        vector<int> literals;
        int token;
         
        while(iss >> token){
            literals.push_back(token);
        }

        // ignore comments and formatting lines
        // (we will assume .cnf format here):
        is >> line;
        if( line[0] == 'c' || 
            line[0] == 'p' || 
            line[0] == '%' || 
            line[0] == '#' ){
            continue;
        }
        
        if(literals.size() <= 0){
            cerr << "Fatal error- Invalid DIMACS file: " << endl;
            cerr << line_no << ": " << line << endl;
            exit(1);
        }
        
        vector<int> reduced_literals;
        for(int lit : literals){
            if(lit != 0){
                reduced_literals.push_back(lit);
            }
        }

        new_sentence.clauses.push_back(reduced_literals);
    }
    return new_sentence;
}

sat_sentence sat_sentence::random_ksat(int num_clauses, int num_variables, string seed, int k){
    
    sat_sentence new_sentence;
    seed_seq s0(seed.begin(), seed.end());
    minstd_rand0 rng(s0);
    uniform_int_distribution<int> var_sample(1,num_variables);
    uniform_int_distribution<int> sign_sample(0,1);

    // impose num_variables < num_clauses for now:
    if(num_variables > num_clauses){
        cerr << "Fatal error- number of clauses must be greater than number of variables"
             << endl;
        exit(1);
    }

    new_sentence.clauses = vector<vector<int>>(num_clauses, vector<int>(k,0));

    for(auto& cl : new_sentence.clauses){
        for(auto& lit : cl){
            lit = var_sample(rng);
            if(sign_sample(rng)){
                lit *= -1;
            }
        }
    }

    for(int i = 0 ; i < num_variables; ++i){
        new_sentence.clauses[i][0] = i+1;
        if(sign_sample(rng)){
            new_sentence.clauses[i][0] *= -1;
        }
    }

    return new_sentence;
}


bool sat_sentence::evaluate(llint assignment, int n, bool wrap_assignments){
    for(auto& cl : clauses){
        bool satisfied = false;
        for(int lit : cl){
            
            if(wrap_assignments){
                bool var_value = (assignment >> (abs(lit) % n)) & 1;        
                if((lit > 0) == var_value){
                    satisfied = true;
                    break;
                }
            } else if((abs(lit)) < n) {
                bool var_value = (assignment >> n) & 1;
                if((lit > 0) == var_value){
                    satisfied = true;
                    break;
                }
            }
            
        }
        if(!satisfied){
            return false;
        }
    }
    return true;
}

int sat_sentence::n_unsatisfied_clauses(llint assignment, int n, bool wrap_assignments){
    int n_unsatisfied = clauses.size();
    for(auto& cl : clauses){
        for(int lit : cl){ 
            if(wrap_assignments){
                bool var_value = (assignment >> (abs(lit) % n)) & 1;        
                if((lit > 0) == var_value){
                    --n_unsatisfied;
                    break;
                }
            } else if((abs(lit)) < n) {
                bool var_value = (assignment >> n) & 1;
                if((lit > 0) == var_value){
                    --n_unsatisfied;
                    break;
                }
            }
        }
    }
    return n_unsatisfied;
}

ostream& operator<<(ostream& os, sat_sentence& s){
    for(auto& cl : s.clauses){
        os << "(";
        for(int lit : cl){
            os << setw(4) << lit;
        }
        os << ") ";
    }
    return os;
}

