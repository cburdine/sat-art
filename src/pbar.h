#include <iostream>

/*
    Author: Colin Burdine
    Last Updated: 05.10.2021

    This is a struct to display a progress bar:

    Members:
        + progress 
            value on [0.0,1.0] that represents the progress
        
        + barwidth
            the width of the progress bar
        
        # show()
            shows/updates the progress bar

*/
struct pbar {
    double progress = 0.0;
    int barwidth = 70;
    
    void show(){
        std::cout << "[";
        int pos = barwidth * progress;
        for (int i = 0; i < barwidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();     
    }
};
