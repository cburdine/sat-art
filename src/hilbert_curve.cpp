# include <cstdlib>
# include <ctime>
# include <iomanip>
# include <iostream>
#include "hilbert_curve.h"

using namespace std;

llint i4_power ( llint i, llint j ){
  llint k;
  llint value;

  if ( j < 0 ){
    if ( i == 1 ){
      value = 1;
    }
    else if ( i == 0 )
    {
      cerr << "\n";
      cerr << "I4_POWER - Fatal error!\n";
      cerr << "  I^J requested, with I = 0 and J negative.\n";
      exit ( 1 );
    }
    else
    {
      value = 0;
    }
  }
  else if ( j == 0 )
  {
    if ( i == 0 )
    {
      cerr << "\n";
      cerr << "I4_POWER - Fatal error!\n";
      cerr << "  I^J requested, with I = 0 and J = 0.\n";
      exit ( 1 );
    }
    else
    {
      value = 1;
    }
  }
  else if ( j == 1 )
  {
    value = i;
  }
  else
  {
    value = 1;
    for ( k = 1; k <= j; k++ )
    {
      value = value * i;
    }
  }
  return value;
}

void rot ( llint n, llint & x, llint & y, llint rx, llint ry ){
  llint t;

  if ( ry == 0 )
  {
    if ( rx == 1 )
    {
      x = n - 1 - x;
      y = n - 1 - y;
    }
    t = x;
    x = y;
    y = t;
  }
  return;
}

void timestamp ( ){
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct std::tm *tm_ptr;
  std::time_t now;

  now = std::time ( NULL );
  tm_ptr = std::localtime ( &now );

  std::strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr );

  std::cout << time_buffer << "\n";

  return;
# undef TIME_SIZE
}

void d2xy ( llint m, llint d, llint & x, llint & y ){
  llint n;
  llint rx;
  llint ry;
  llint s;
  llint t = d;

  n = i4_power ( 2, m );

  x = 0;
  y = 0;
  for ( s = 1; s < n; s = s * 2 )
  {
    rx = ( 1 & ( t / 2 ) );
    ry = ( 1 & ( t ^ rx ) );
    rot ( s, x, y, rx, ry );
    x = x + s * rx;
    y = y + s * ry;
    t = t / 4;
  }
  return;
}

llint xy2d ( llint m, llint x, llint y ){
  llint d = 0;
  llint n;
  llint rx;
  llint ry;
  llint s;

  n = i4_power ( 2, m );

  for ( s = n / 2; s > 0; s = s / 2 )
  {
    rx = ( x & s ) > 0;
    ry = ( y & s ) > 0;
    d = d + s * s * ( ( 3 * rx ) ^ ry );
    rot ( s, x, y, rx, ry );
  }
  return d;
}

llint gray2d(llint g){
    llint d = 0;
    while(g){
        d ^= g;
        g >>= 1;
    }
    return d;
}

llint d2gray(llint d){
    return d ^ (d >> 1);
}
