/// @file
///
/// @brief Class declarations for Quatern
///
/// Copyright &copy; INTEGRA, Inc., 1999-2014.
///
/// @internal
/// @author Piv Ildar Valiev                      '99.12.28<BR>
/// @notes  On the base of the file quat.h created by Dan Silva from maxsdk.


#ifndef _QUATERN_HPP_
#define _QUATERN_HPP_


#include "vect3.hpp"
#include "matrix43.hpp"

class ostream;

INTEGRA_NAMESPACE_START

template <class T> class Quatern;

template <class T>
class AngleAxis
  {
  public:
    TVect3<T> axis;
    T angle;

    AngleAxis()
      {
      }
    AngleAxis(const TVect3<T> &axis, T angle)
      {
      this->axis = axis;
      this->angle = angle;
      }
    AngleAxis(const Quatern<T> &q);
    int GetNumRevs();
    void SetNumRevs(int num);
  };

template <class T>
class Quatern
  {
  public:
    T x, y, z, w;

    // Constructors
    Quatern()
      {
      }
    Quatern(T Xx, T Yy, T Zz, T Ww)
      {
      x = Xx;
      y = Yy;
      z = Zz;
      w = Ww;
      }
/*
    Quatern(double Xx, double Yy, double Zz, double Ww)
      {
      x = (T)Xx;
      y = (T)Yy;
      z = (T)Zz;
      w = (T)Ww;
      }*/
    Quatern(const Quatern & a)
      {
      x = a.x;
      y = a.y;
      z = a.z;
      w = a.w;
      }
    Quatern(T af[4])
      {
      x = af[0];
      y = af[1];
      z = af[2];
      w = af[3];
      }
    Quatern(const TMatrix43<T> &mat);
    Quatern(const AngleAxis<T> &aa);
  
    // Access operators
    T &operator[](int i)
      {
      return (&x)[i];
      }
    const T &operator[](int i) const
      {
      return (&x)[i];
      }
  
    // Conversion function
    operator T *()
      {
      return (&x);
      }
  
    // Unary operators
    Quatern operator -() const
      {
      return (Quatern(-x, -y, -z, -w));
      }
    Quatern operator +() const
      {
      return *this;
      }
  
    /// @name Assignment operators.
    //@{
    Quatern & operator -= (const Quatern &);
    Quatern & operator += (const Quatern &);
    Quatern & operator *= (const Quatern &);
    Quatern & operator *= (T);
    Quatern & operator /= (T);
    //@}
  
    Quatern & MakeClosest(const Quatern &qto);
  
    /// Comparison.
    int operator == (const Quatern &a) const;
    /// Set identity quaternion.
    void Identity()
      {
      x = y = z = (T)0.0;
      w = (T) 1.0;
      }

    /// Is identity quaternion.
    int IsIdentity() const;
    /// Normalize.
    void Normalize(); 
    /// Convert quaternion to 3x3 rotation matrix.
    void MakeMatrix(TMatrix43<T> &mat) const;
  
    /// @name Binary operators.
    //@{
    /// Difference of two quaternions.
    Quatern operator - (const Quatern &) const;
    // RB: Changed these to   
    /// Sum of two quaternions.
    Quatern operator + (const Quatern &) const;  // duplicate * and /
    /// Product of two quaternions.
    Quatern operator * (const Quatern &) const;  
    /// Ratio of two quaternions.
    Quatern operator / (const Quatern &) const;  
    //@}
  };

// ----------------------------------------------------
//             PREDEFINED TYPES
// ----------------------------------------------------

/// Predefined instance of 3x3 matrix with elements of float type.
typedef Quatern<float>   QuaternF;
/// Predefined instance of 3x3 matrix with elements of float type.
typedef Quatern<double>  QuaternD;

/// Multiply by scalar.
template <class T>
Quatern<T>  operator * (T, const Quatern<T> &); 
/// Multiply by scalar.
template <class T>
Quatern<T>  operator * (const Quatern<T> & , T); 
/// Divide by scalar.
template <class T>
Quatern<T>  operator / (const Quatern<T> &q, T); 
// Inverse of quaternion (1/q).
template <class T>
Quatern<T>  Inverse(const Quatern<T> &q);  
template <class T>
Quatern<T>  Conjugate(const Quatern<T> &q);
template <class T>
Quatern<T>  LogN(const Quatern<T> &q);
template <class T>
Quatern<T>  Exp(const Quatern<T> &q);
template <class T>
Quatern<T>  Slerp(const Quatern<T> &p, const Quatern<T> &q, T t);
template <class T>
Quatern<T>  LnDif(const Quatern<T> &p, const Quatern<T> &q);
template <class T>
Quatern<T>  QCompA(const Quatern<T> &qprev, const Quatern<T> &q, const Quatern<T> &qnext);
template <class T>
Quatern<T>  Squad(const Quatern<T> &p, const Quatern<T> &a, const Quatern<T> &b, const Quatern<T> &q, T t);
template <class T>
Quatern<T>  qorthog(const Quatern<T> &p, const TVect3<T> &axis);
template <class T>
Quatern<T>  squadrev(
T angle,    // angle of rotation
const TVect3<T> &axis, // the axis of rotation
const Quatern<T> &p,    // start quaternion
const Quatern<T> &a,    // start tangent quaternion
const Quatern<T> &b,    // end tangent quaternion
const Quatern<T> &q,    // end quaternion
T t                  // parameter, in range [0.0,1.0]
);

template <class T>
void  RotateMatrix(TMatrix43<T> &mat, const Quatern<T> &q);
template <class T>
void  PreRotateMatrix(TMatrix43<T> &mat, const Quatern<T> &q);
template <class T>
Quatern<T>  QFromAngleAxis(T ang, const TVect3<T> &axis);
template <class T>
void  AngleAxisFromQ(const Quatern<T> &q, T *ang, TVect3<T> &axis);
template <class T>
T  QangAxis(const Quatern<T> &p, const Quatern<T> &q, TVect3<T> &axis);
template <class T>
void  DecomposeMatrix(const TMatrix43<T> &mat, TVect3<T> &p, Quatern<T> &q, TVect3<T> &s);
template <class T>
Quatern<T>  TransformQuatern(const TMatrix43<T> &m, const Quatern<T> &q);
template <class T>
inline Quatern<T> IdentQuatern()
  {
  return (Quatern<T>(0.0, 0.0, 0.0, 1.0));
  }

// Assumes Euler angles are of the form:
// RotateX(ang[0])
// RotateY(ang[1])
// RotateZ(ang[2])
//
template <class T>
void  QuatToEuler(Quatern<T> &q, T *ang);
// void  EulerToQuat(T *ang, Quatern<T> &q);

template <class T>
ostream  &operator << (ostream &, const Quatern<T> &);

INTEGRA_NAMESPACE_END

#endif // #ifndef _QUATERN_HPP_ 

// 'quatern.hpp' 
