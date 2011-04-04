#include "a2.hpp"
#include <cmath>

using namespace std;

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 *r;
  double rad = (angle*M_PI);
  rad = rad/180;
  Matrix4x4 z( Vector4D( cos(rad), -sin(rad), 0, 0 ),
               Vector4D( sin(rad), cos(rad),  0, 0 ),
               Vector4D( 0,        0,         1, 0 ),
               Vector4D( 0,        0,         0, 1 ));
               
  Matrix4x4 x( Vector4D( 1, 0,        0,         0 ),
               Vector4D( 0, cos(rad), -sin(rad), 0 ),
               Vector4D( 0, sin(rad), cos(rad),  0 ),
               Vector4D( 0, 0,        0,         1 ));
               
  Matrix4x4 y( Vector4D( cos(rad),  0, sin(rad), 0 ),
               Vector4D( 0,         1, 0,        0 ),
               Vector4D( -sin(rad), 0, cos(rad), 0 ),
               Vector4D( 0,         0,        0, 1 ));
  
  if (axis == 'x') r = &x;
  else if (axis == 'y') r = &y;
  else if (axis == 'z') r = &z;
  else
  {
    cerr << "bad axis: " << axis << endl;
    exit(-1);
  }
  
  return *r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
  Matrix4x4 t( Vector4D( 1, 0, 0, displacement[0] ),
               Vector4D( 0, 1, 0, displacement[1] ),
               Vector4D( 0, 0, 1, displacement[2] ),
               Vector4D( 0, 0, 0, 1 ));
  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
  Matrix4x4 s( Vector4D( scale[0], 0, 0, 0 ),
               Vector4D( 0, scale[1], 0, 0 ),
               Vector4D( 0, 0, scale[2], 0 ),
               Vector4D( 0, 0, 0, 1 ));
  return s;
}

