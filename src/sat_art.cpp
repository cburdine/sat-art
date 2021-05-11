
// Import the CImg library
// for more information see:
// https://cimg.eu/
#include "cimg/CImg.h"

#include "hilbert_curve.h"
#include "sat.h"
#include "pbar.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>

using namespace cimg_library;

// informational strings:
const string VERSION = "SAT ART version 1.0";
const string ABOUT = "\nThis is a program that renders k-SAT sentences as pictures.\n";
const string USAGE = 
"usage: sat_art [-b beta] ( -f dimacs_file | --random [-c n_clauses] [-v n_vars] [-s seed] [-o dimacs_file_out] ) [img_out]\n\
run \"sat_art -h\" for more information.";

const string ARG_HELP = "\
    -b beta                                                                     \n\
        The rate of decay of the image shader (must be greater than 0.0).       \n\
                                                                                \n\
    -f dimacs_file                                                              \n\
        A dimacs (.cnf) file to read a sentence from. Note that if the sentence \n\
        is too large, one the first few variables will be evaluated.            \n\
                                                                                \n\
    --random                                                                    \n\
        Generates a random sentence instead of reading from a DIMACS file.      \n\
                                                                                \n\
    -c n_clauses                                                                \n\
        The number of clauses in the randomly generated sentence.               \n\
                                                                                \n\
    -v n_vars                                                                   \n\
        The number of variables in the randomly generated sentence.             \n\
                                                                                \n\
    -s seed                                                                     \n\
        A seed string for the randomly generated sentence.                      \n\
                                                                                \n\
    -o dimacs_file_out                                                          \n\
        An output dimacs (.cnf) file to save a randomly generated sentence.     \n\
                                                                                \n\
    img_out                                                                     \n\
        The output image (optional)                                             \
";   


// The maximum number of variables supported without projection:
const int MAX_N_VARIABLES = 30; 

// color types:
const unsigned char BLACK[3]    = { 0  , 0  , 0   };
const unsigned char WHITE[3]    = { 255, 255, 255 };
const unsigned char PURPLE[3]   = { 100, 0  , 100 };
const unsigned char GREEN[3]    = { 0  , 255, 0   };

// utility function for blending colors:
void color_interp(const unsigned char a[3], const unsigned char b[3], double x, unsigned char* result ){
    for(int i = 0; i < 3; ++i){
        result[i] = ((unsigned char) ((1.0-x)*a[i] + x*b[i]));
    }
}

/*
    This struct represents the command line args:
*/
struct parsed_args {
        
    // the beta value (colormap decay ~~ (# variables)/(# clauses) )
    double beta = 0.5;
    
    // DIMACS file to read from:
    string file_in = "";
    
    // flag to randomly generate a sentence:
    bool random = false;

    // the seed for a randomly generated sentence:
    string seed = "";
    
    // number of clauses for a random sentence:
    int n_clauses = 32;
    
    // number of variables for a random sentence:
    int n_variables = 16;
    
    // file to save resulting sentence to:
    string file_out = ""; 

    // file to save resulting image to:
    string img_out = "";
};

// functions for parsing args:
parsed_args parse_args(int argc, char** argv);
void arg_err(string token, string msg, int exit_code = 1);

// main routine:
int main(int argc, char** argv){

    // parse args:
    parsed_args pa = parse_args(argc, argv);
     
    // generate SAT sentence:
    sat_sentence sentence;
    if(pa.random){
        
        sentence = sat_sentence::random_ksat(pa.n_clauses, pa.n_variables, pa.seed, 3);
        
        // save sentence (optional):
        if(pa.file_out != ""){
            ofstream file_out(pa.file_out);
            if(!file_out){
                arg_err(pa.file_out, "unable to write to file.");
            }
            sentence.to_dimacs(file_out, "Seed: " + pa.seed);
            file_out.close();
        }
    } else {
        ifstream file_in(pa.file_in);
        if(!file_in){
            arg_err(pa.file_in, "file not found.");
        }
        sentence = sat_sentence::from_dimacs(file_in);
        file_in.close();
        
        pa.n_clauses = sentence.clauses.size();
        pa.n_variables = sentence.num_variables();    
    }
    
     // check that the number of variables is sane:
    if(pa.n_variables > MAX_N_VARIABLES){
        cerr << "Fatal error- memory limit possibly exceeded:\nnumber of variables ("
             << pa.n_variables << ") cannot exceed MAX_N_VARIABLES"
             << " (" << MAX_N_VARIABLES << ")." << endl;
            exit(1);
    }
    

    // initialize a blank image:
    llint img_size = 1<<((pa.n_variables+1)/2);
    llint img_size_x = 1<<(pa.n_variables/2);
    CImg<unsigned char> img(img_size_x,img_size,1,3); 
    img.fill(0); 

 
    // initialize a progress bar (big images may take some time to render):
    llint x, y;
    llint istep = 0;
    llint imax = 1<<pa.n_variables;
    llint istep_max = imax / 1000;
    unsigned char color[3];
    pbar pb;
    pb.show();

    // render image:
    for(int i = 0; i < imax; ++i){
        
        int n_unsat = sentence.n_unsatisfied_clauses(d2gray(i), pa.n_variables); 
        double cmap_val = exp(-pa.beta * (double) n_unsat);
        color_interp(BLACK,GREEN,cmap_val, color);
        
        d2xy((pa.n_variables+1)/2,i,x,y);
        int xp = x;
        int yp = y;
        img.draw_point(xp,yp,color);    
        
        ++istep;
        if(istep >= istep_max){
            istep = 0;
            pb.progress = ((double) i / (double) imax);
            pb.show();
        }
    }
    
    // display image:
    img.display("SAT Art");
     

    return 0;

}

parsed_args parse_args(int argc, char** argv){
    vector<string> args;
    parsed_args pa;
    
    for(int i = 0; i < argc; ++i){
        args.push_back(string(argv[i]));
    }
    
    for(int i =0; i < argc; ++i){
        
        // parse help:
        if(args[i] == "--help" || args[i] == "-h"){
            cout << VERSION << endl;
            cout << ABOUT << endl << endl;
            cout << USAGE << endl << endl;
            cout << ARG_HELP << endl << endl;
            exit(0);
        }

        // parse beta:
        else if(args[i] == "-b"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no beta value given."); }
            istringstream iss(args[i]);
            if(!(iss >> pa.beta) || pa.beta < 0.0){
                arg_err(args[i], "invalid beta value.");
            }
        }

        // parse dimacs file:
        else if(args[i] == "-f"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no DIMACS file given."); }
            if(args[i][0] == '-'){ arg_err(args[i], "invalid file name"); }
            pa.file_in = args[i];
        }

        // parse seed:
        else if(args[i] == "-s"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no seed given."); }
            if(args[i][0] == '-'){ arg_err(args[i], "seed cannot start with '-'."); }
            pa.seed = args[i];
        }

        // parse random:
        else if(args[i] == "--random"){
            pa.random = true;
        }

        // parse clauses:
        else if(args[i] == "-c"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no number of clauses given."); }
            istringstream iss(args[i]);
            if(!(iss >> pa.n_clauses) || pa.n_clauses <= 0){ 
                arg_err(args[i], "invalid number of clauses,"); 
            }
        }
        
        // parse variables:
        else if(args[i] == "-v"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no number of variables given."); }
            istringstream iss(args[i]);
            if(!(iss >> pa.n_variables) || pa.n_variables <= 0){ 
                arg_err(args[i], "invalid number of variables."); 
            }
        }

        // parse file out:
        else if(args[i] == "-o"){
            ++i;
            if(i >= argc){ arg_err(args[i-1],"no output DIMACS file given."); }
            if(args[i][0] == '-'){ arg_err(args[i], "invalid file name (recommended \".cnf\" extension)."); }
            pa.file_out = args[i];
        }

        // parse output image:
        else if(i+1 == argc){
            if(args[i][0] == '-'){ arg_err(args[i], "invalid output image name (recommended \".png\" extension)."); }
            pa.img_out = args[i];
        }
    }

    if((pa.file_in != "") == (pa.random)){
        arg_err("--random", "you must use either the \"--random\" flag or specify an input DIMACS file with the \"-f\" flag.");
    }

    return pa;
}


void arg_err(string token, string msg, int exit_code){
    cerr << "Invalid arg \"" << token << "\": " << endl;
    cerr << msg << endl << endl;
    cerr << USAGE << endl << endl;
    exit(exit_code);
}




















