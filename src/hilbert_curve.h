#ifndef HILBERT_CURVE_H
#define HILBERT_CURVE_H

typedef long long int llint;

using namespace std;

llint i4_power ( llint i, llint j );

void rot( llint n, llint & x, llint & y, llint rx, llint ry );

void timestamp();

void d2xy( llint m, llint d, llint & x, llint & y );

llint xy2d( llint m, llint x, llint y );

llint d2gray(llint d);

llint gray2d(llint g);

#endif // HILBERT_CURVE_H
