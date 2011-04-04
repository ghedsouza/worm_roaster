#ifndef PERLIN_H
#define PERLIN_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



double Cubic_Interpolate(double v0, double v1, double v2, double v3, double x)
{
	double P = (v3 - v2) - (v0 - v1);
	double Q = (v0 - v1) - P;
	double R = v2 - v0;
	double S = v1;

	return P*pow(x,3) + Q*pow(x,2) + R*x + S;
}

double Cosine_Interpolate(double a, double b, double x)
{
  double ft = x * 3.1415927;
	double f = (1 - cos(ft)) * .5;

	return  a*(1-f) + b*f;
}

double Noise(int x, int y)
{
  int n = x+y*57;
  n = (n<<13)^n;
  return (1.0 - ( (n*(n*n*17731+789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double SmoothedNoise1(double x, double y)
{
  double corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16;
  double sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8;
  double center  =  Noise(x, y) / 4;
  return corners + sides + center;
}

double InterpolatedNoise_1(double x, double y)
{
  int integer_X    = int(x);
  double fractional_X = x - integer_X;

  int integer_Y    = int(y);
  double fractional_Y = y - integer_Y;

  double v1 = SmoothedNoise1(integer_X,     integer_Y);
  double v2 = SmoothedNoise1(integer_X + 1, integer_Y);
  double v3 = SmoothedNoise1(integer_X,     integer_Y + 1);
  double v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);

  double i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
  double i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

  return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

double PerlinNoise_2D(double x, double y, double per, double Oct)
{
  double total = 0;
  double p = per;
  double n = Oct - 1;

  for(int i=0; i<=n; i++)
  {
    double frequency = pow(2.0,i);
    double amplitude = pow(p,i);

    total = total + InterpolatedNoise_1(x * frequency, y * frequency) * amplitude;
  }

  return total;
}


#endif // PERLIN_H
