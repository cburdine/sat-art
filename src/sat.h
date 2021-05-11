#ifndef SAT_H
#define SAT_H

#include <vector>
#include <iostream>

using namespace std;

typedef long long int llint;

struct sat_sentence {
    vector<vector<int>> clauses;
    
    static sat_sentence from_ksat(int sentence[], int num_clauses, int k = 3);

    static sat_sentence from_dimacs(istream& dimacs_file);

    static sat_sentence random_ksat(int num_clauses, int num_variables, string seed="", int k = 3);
        
    int num_variables();
    
    bool evaluate(llint assignment, int n, bool wrap_assignments=true);    

    int n_unsatisfied_clauses(llint assignment, int n, bool wrap_assignments=true);

    void to_dimacs(ostream& dimacs_file, string comment = "");

};

ostream& operator<<(ostream& os, sat_sentence& s);


#endif // SAT_H
