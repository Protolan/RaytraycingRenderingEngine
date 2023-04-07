/// @file
///
/// @brief Class definitions for Quatern
///
/// Copyright &copy; INTEGRA, Inc., 1999-2014.
///
/// @internal
/// @author Piv Ildar Valiev                      '99.12.28<BR>
/// @notes  On the base of the file quat.h created by Dan Silva from maxsdk.
/// Quaternion operations --
/// From "Quaternion Calculus and Fast Animation",
/// by Ken Shoemake,  in notes for SIGGRAPH 1987 Course # 10,
/// "Computer Animation: 3-D Motion Specification and Control".


#include <integra.hpp>

#include "quatern.hpp"


#define xDBGQUAT

#define COS_EPSILON  0.000001

#ifdef DBGQUAT
short dbgquat = 1;
#endif

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
/// MakeClosest: modify q so it is on same side of hypersphere as qto
template <class T>
Quatern<T> & Quatern<T>::MakeClosest(const Quatern<T> &qto)
  {
  T dot =  x * qto.x + y * qto.y + z * qto.z + w * qto.w;
  if (dot < 0.0)
    {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    }
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Is identity quaternion.
template <class T>
int Quatern<T>::IsIdentity() const
  {
  return (Math<T>::AboutZero(x) && Math<T>::AboutZero(y) && Math<T>::AboutZero(z));
  }

template <class T>
int Quatern<T>::operator == (const Quatern<T> &a) const
  {
  Quatern<T> q1 = (*this);
  Quatern<T> q2 = a;
  q1.Normalize();
  q2.Normalize();
  return (q1 - q2).IsIdentity();
  }

template <class T>
Quatern<T> & Quatern<T>::operator += (const Quatern<T> &a)
  {
  //x += a.x;     y += a.y;       z += a.z; w+= a.w;
  *this = *this * a;
  return *this;
  }

template <class T>
Quatern<T> & Quatern<T>::operator -= (const Quatern<T> &a)
  {
  //x -= a.x;     y -= a.y;       z-= a.z; w-= a.w;
  *this = *this / a;
  return *this;
  }

template <class T>
Quatern<T> & Quatern<T>::operator *= (T f)
  {
  x *= f;
  y *= f;
  z *= f;
  w *= f;
  return *this;
  }

template <class T>
Quatern<T> & Quatern<T>::operator *= (const Quatern<T> &q)
  {
  (*this) = (*this)*q;
  return (*this);
  }

template <class T>
Quatern<T> & Quatern<T>::operator /= (T f)
  {
  x /= f;
  y /= f;
  z /= f;
  w /= f;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Normalize a quaternion.
template <class T>
void  Quatern<T>::Normalize()
  {
  double l, c;
  l = sqrt(x * x + y * y + z * z + w * w);
  if (l == 0.0)
    {
    w = 1.0;
    y = z = x = 0.0;
    }
  else
    {
    c = 1.0 / l;
    x = T(x * c);
    y = T(y * c);
    z = T(z * c);
    w = T(w * c);
    }
  }

//////////////////////////////////////////////////////////////////////////////
template <class T>
Quatern<T> Inverse(const Quatern<T> &q)
  {
  T l, norminv;
  Quatern<T> qq;
  l =  (q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (l == (T)0.0)
    l = (T)1.0;
  norminv = (T)1.0 / l;
  qq.x = -q.x * norminv;
  qq.y = -q.y * norminv;
  qq.z = -q.z * norminv;
  qq.w = q.w * norminv;
  return (qq);
  }

//////////////////////////////////////////////////////////////////////////////
/// Product of two quaternions
template <class T>
Quatern<T> Quatern<T>::operator * (const Quatern<T> &q) const
  {
  Quatern<T> qp;
  qp.w = w * q.w - x * q.x - y * q.y - z * q.z;
  qp.x = w * q.x + x * q.w + y * q.z - z * q.y;
  qp.y = w * q.y + y * q.w + z * q.x - x * q.z;
  qp.z = w * q.z + z * q.w + x * q.y - y * q.x;
  return (qp);
  }

//////////////////////////////////////////////////////////////////////////////
/// Ratio of two quaternions: This creates a result quaternion r = p/q, such
/// that q*r = p.  (order of multiplication is important)
template <class T>
Quatern<T> Quatern<T>::operator / (const Quatern<T> &q) const
  {
  return (Inverse(q) * (* this));
  }

//////////////////////////////////////////////////////////////////////////////
/// Create Conjugate of quaternions.
template <class T>
Quatern<T>  Conjugate(const Quatern<T> &q)
  {
  Quatern<T> qq;
  qq.x = -q.x;
  qq.y = -q.y;
  qq.z = -q.z;
  qq.w = q.w;
  return (qq);
  }

//////////////////////////////////////////////////////////////////////////////
/// Take the natural logarithm of UNIT quaternion.
template <class T>
Quatern<T>  LogN(const Quatern<T> & q)
  {
  T theta, scale;
  Quatern<T> qq;
  scale = (T)sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
  theta = (T)atan2(scale, q.w);
  if (scale > (T)0.0)
    scale = theta / scale;
  qq.x = scale * q.x;
  qq.y = scale * q.y;
  qq.z = scale * q.z;
  qq.w = (T)0.0;
  return (qq);
  }

//////////////////////////////////////////////////////////////////////////////
/// Exp: exponentiate quaternion (where q.w==0)
template <class T>
Quatern<T>  Exp(const Quatern<T> &q)
  {
  T theta, scale;
  Quatern<T> qq;
  theta = (T)sqrt(q.x * q.x + q.y * q.y + q.z * q.z);
  scale = (T)1.0;
  if (theta > EPSILON)
    scale = (T)sin(theta) / theta;
  qq.x = scale * q.x;
  qq.y = scale * q.y;
  qq.z = scale * q.z;
  qq.w = (T)cos(theta);
  return (qq);
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert quaternion to 3x3 rotation matrix.
///
///  Quaternion need not be unit magnitude.  When it always is,
///  this routine can be simplified
template <class T>
void Quatern<T>::MakeMatrix(TMatrix43<T> &mat)  const
  {
  T s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz, den;

  if (w == 1.0 && x == 0.0 && y == 0.0 && z == 0.0)
    {
    mat.Identity();
    return;
    }
  // For unit q, just set s = 2.0; or or set xs = q.x + q.x, etc
  den =  (x * x + y * y + z * z + w * w);
  if (den == 0.0)
    {
    s = (T)1.0;
    }
  else
    s = (T)2.0 / den;

  xs = x * s;
  ys = y * s;
  zs = z * s;
  wx = w * xs;
  wy = w * ys;
  wz = w * zs;
  xx = x * xs;
  xy = x * ys;
  xz = x * zs;
  yy = y * ys;
  yz = y * zs;
  zz = z * zs;

  mat[0][0] = (T)1.0 - (yy + zz);
  mat[0][1] = xy - wz;
  mat[0][2] = xz + wy;

  mat[1][0] = xy + wz;
  mat[1][1] = (T)1.0 - (xx + zz);
  mat[1][2] = yz - wx;

  mat[2][0] = xz - wy;
  mat[2][1] = yz + wx;
  mat[2][2] = (T)1.0 - (xx + yy);

  mat[3][0] = (T)0.0;
  mat[3][1] = (T)0.0;
  mat[3][2] = (T)0.0;
  // mat.flags |= POS_IDENTI & SCL_IDENTI;
  // mat.flags &= ~(ROT_IDENTI);
  }

//////////////////////////////////////////////////////////////////////////////
template <class T>
void RotateMatrix(TMatrix43<T> &mat, const Quatern<T> &q)
  {
  TMatrix43<T> t;
  q.MakeMatrix(t);
  // t.ClearIdentFlag(ROT_IDENTI);
  mat = mat * t;
  }

//////////////////////////////////////////////////////////////////////////////
template <class T>
void PreRotateMatrix(TMatrix43<T> &mat, const Quatern<T> &q)
  {
  TMatrix43<T> t;
  q.MakeMatrix(t);
  // t.ClearIdentFlag(ROT_IDENTI);
  mat = t * mat;
  }

static int nxt[3] = {1, 2, 0};


//////////////////////////////////////////////////////////////////////////////
/// MakeQuat: Convert 3x3 rotation matrix to unit quaternion
template <class T>
Quatern<T>::Quatern(const TMatrix43<T> &mat)
  {
  T tr, s;
  int i, j, k;

#if 0
  if (mat.IsIdentity())
    {
    Identity();
    return;
    }
#endif

  tr = mat[0][0] + mat[1][1] + mat[2][2];
  if (tr > 0.0)
    {
    s = (T)sqrt(tr + (T)1.0);
    w = s * (T)0.5;
    s = (T)0.5 / s;
    x = (mat[2][1] - mat[1][2]) * s;
    y = (mat[0][2] - mat[2][0]) * s;
    z = (mat[1][0] - mat[0][1]) * s;
    }
  else
    {
    i = 0;
    if (mat[1][1] > mat[0][0])
      i = 1;
    if (mat[2][2] > mat[i][i])
      i = 2;
    j = nxt[i];
    k = nxt[j];
    s = (T)sqrt((mat[i][i] - (mat[j][j] + mat[k][k])) + (T)1.0);
    (*this)[i] = s * (T)0.5;
    if (s != (T)0.0)
      s = (T)0.5 / s;
    w = (mat[k][j] - mat[j][k]) * s;
    (*this)[j] = (mat[j][i] + mat[i][j]) * s;
    (*this)[k] = (mat[k][i] + mat[i][k]) * s;
    }
  }




// -------slerp: Spherical linear interpolation of UNIT quaternions.
//   As t goes from 0 to 1, qt goes from p to q.
//
//      slerp(p,q,t) = (p*sin((1-t)*omega) + q*sin(t*omega)) / sin(omega)
//
//      NOTE: for repeated slerps on the same curve, can precompute
//      omega, sinom, p/sinom, q/sinom.  Also can use table lookups for
//      the sin(t*omega) etc.
//----------------------------------------------------
template <class T>
Quatern<T>  Slerp(const Quatern<T> &p, const Quatern<T> &q, T t)
  {
  T omega, cosom, sinom, sclp, sclq;
  int i;
  Quatern<T> qt(0.0f, 0.0f, 0.0f, 0.0f);
  cosom = p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
  if (((T)1.0 + cosom) > COS_EPSILON)
    {
    // usual case
    if (((T)1.0 - cosom) > COS_EPSILON)
      {
      //usual case
      omega = (T)acos(cosom);
      sinom = (T)sin(omega);
#ifdef DBGQUAT
      if (sinom == (T)0.0)
        printf(" sinom = 0.0 in slerp \n");
#endif
      sclp = (T)sin(((T)1.0 - t)*omega) / sinom;
      sclq = (T)sin(t * omega) / sinom;
      }
    else
      {  // ends very close -- just lerp
      sclp = (T)1.0 - t;
      sclq = t;
      }
    for (i = 0; i < 4; i++)
      qt[i] = sclp * p[i] + sclq * q[i];
    }
  else
    {
    /* p and q nearly opposite on sphere-- this is a 360 degree
                   rotation, but the axis of rotation is undefined, so
                   slerp really is undefined too.  So this apparently picks
                   an arbitrary plane of rotation. However, I think this
                   code is incorrect.
                   */
    qt.x = -p.y;
    qt.y = p.x;  // qt is orthogonal to p
    qt.z = -p.w;
    qt.w = p.z;
    sclp = (T)sin(((T)1.0 - t) * HALFPI);
    sclq = (T)sin(t * HALFPI);
    for (i = 0; i < 3; i++)
      qt[i] = sclp * p[i] + sclq * qt[i];
    }
  return (qt);
  }

template <class T>
Quatern<T> Quatern<T>::operator + (const Quatern<T> &q) const
  {
  //return(Quatern<T>(x + q.x,y + q.y,z + q.z,w + q.w));
  return *this * q;
  }

template <class T>
Quatern<T> Quatern<T>::operator - (const Quatern<T> &q) const
  {
  //return(Quatern<T>(x - q.x,y - q.y,z - q.z,w - q.w));
  return *this / q;
  }

template <class T>
Quatern<T>  operator * (const Quatern<T> & q, T s)
  {
  return (Quatern<T>(q.x * s, q.y * s, q.z * s, q.w * s));
  }

template <class T>
Quatern<T>  operator * (T s, const Quatern<T> & q)
  {
  return (Quatern<T>(q.x * s, q.y * s, q.z * s, q.w * s));
  }

template <class T>
Quatern<T>  operator / (const Quatern<T> & q, T s)
  {
  return (Quatern<T>(q.x / s, q.y / s, q.z / s, q.w / s));
  }

//Compute the "log difference" of two quaternions, p and q,
//      as  ln(qinv(p)*q)
template <class T>
Quatern<T>  LnDif(const Quatern<T> &p, const Quatern<T> &q)
  {
  Quatern<T> r;
  r = q / p;
  return (LogN(r));
  }

// Compute a, the term used in Boehm-type interpolation.
// a[n] = q[n]* qexp(-(1/4)*( ln(qinv(q[n])*q[n+1]) +ln( qinv(q[n])*q[n-1] )))
template <class T>
Quatern<T>  QCompA(const Quatern<T> &qprev, const Quatern<T> &q, const Quatern<T> &qnext)
  {
  Quatern<T> qm, qp, r;
  //qm = LnDif(qprev,q); // RB: I think the parameters are reversed
  qm = LnDif(q, qprev);
  qp = LnDif(q, qnext);
  //r = (T).25*(qm-qp); // RB: I changed + and - to duplicate * and /
                            //  so my templates would work.
  r = -(T).25 * (Quatern<T>(qm.x + qp.x, qm.y + qp.y, qm.z + qp.z, qm.w +
                          qp.w));
  return (q * Exp(r));
  }

//////////////////////////////////////////////////////////////////////////////
/// Squad(p,a,b,q; t) = Slerp(Slerp(p,q;t), Slerp(a,b;t); 2(1-t)t). 
template <class T>
Quatern<T>  Squad(const Quatern<T> &p, const Quatern<T> &a, const Quatern<T> &b, const Quatern<T> &q, T t)
  {
  T k = (T)2.0 * ((T)1.0 - t)*t;
  return (Slerp(Slerp(p, q, t), Slerp(a, b, t), k));
  }

// template <class T> 
// Quatern<T>  EulerToQuatern(T x, T y, T z) {
//  TMatrix43<T> mat;
//      mat.IdentityMatrix();
//      mat.RotateX(x);
//      mat.RotateY(y);
//      mat.RotateZ(z);
//      return(Quatern<T>(mat));
//      }

//////////////////////////////////////////////////////////////////////////////
/// Rotate p by 90 degrees (quaternion space metric) about axis.
template <class T>
Quatern<T>  qorthog(const Quatern<T> &p, const TVect3<T> &axis)
  {
  return (p * Quatern<T>(axis.x, axis.y, axis.z, (T)0.0));
  }

// Quaternion interpolation for angles > 2PI.
template <class T>
Quatern<T>  squadrev(
T angle,    // angle of rotation
const TVect3<T> &axis, // the axis of rotation
const Quatern<T> &p,          // start quaternion
const Quatern<T> &a,          // start tangent quaternion
const Quatern<T> &b,          // end tangent quaternion
const Quatern<T> &q,          // end quaternion
T t                 // parameter, in range [0.0,1.0]
)
  {
  T s, v;
  T omega = angle * (T).5;
  T nrevs = (T)0.0;
  Quatern<T> r, pp, qq;

#ifdef DBGQUAT
  if (dbgquat)
    {
    printf("t = %.5f, ang=%.5f, ax =(%.5f,%.5f,%.5f) \n", t, angle, axis[0],
    axis[1], axis[2]);
    printf("p = (%.5f,%.5f,%.5f,%.5f) \n", p[0], p[1], p[2], p[3]);
    printf("a = (%.5f,%.5f,%.5f,%.5f) \n", a[0], a[1], a[2], a[3]);
    printf("b = (%.5f,%.5f,%.5f,%.5f) \n", b[0], b[1], b[2], b[3]);
    printf("q = (%.5f,%.5f,%.5f,%.5f) \n", q[0], q[1], q[2], q[3]);
    }
#endif
  if (omega < PI - EPSILON)
    {
    r = Squad(p, a, b, q, t);
#ifdef DBGQUAT
    if (dbgquat)
      {
      printf("r1 = (%.5f,%.5f,%.5f,%.5f) \n", r[0], r[1], r[2], r[3]);
      }
#endif
    return (r);
    }

  while (omega > (PI - EPSILON))
    {
    omega -= PI;
    nrevs += (T)1.0;
    }
  if (omega < (T)0.0)
    omega = (T)0.0;
  s = t * angle / PI;                  /* 2t(omega+Npi)/pi */

  if (s < (T)1.0)
    {
    pp = qorthog(p, axis);
    r = Squad(p, a, pp, pp, s);         /* in first 90 degrees */
    }
  else
    if ((v = s + (T)1.0 - (T)2.0 * (nrevs + (omega / PI))) <=
        (T)0.0)
      {
      /* middle part, on great circle(p,q) */
      while (s >= (T)2.0)
        s -= (T)2.0;
      pp = qorthog(p, axis);
      r = Slerp(p, pp, s);
      }
  else
    {    /* in last 90 degrees */
    qq = -qorthog(q, axis);
    r = Squad(qq, qq, b, q, v);
    }
#ifdef DBGQUAT
  if (dbgquat)
    {
    printf("r2 = (%.5f,%.5f,%.5f,%.5f) \n", r[0], r[1], r[2], r[3]);
    }
#endif
  return (r);
  }

// --- Convert [angle,axis] representation to equivalent quaternion
template <class T>
Quatern<T>  QFromAngleAxis(T ang, const TVect3<T> &axis)
  {
  T omega, s;
  omega = ang * (T)0.5;
  s = (T)sin(omega);
  return (Quatern<T>(s * axis.x, s * axis.y, s * axis.z, (T)cos(omega)));
  }

// --- Convert quaternion to equivalent [angle,axis] representation
template <class T>
void  AngleAxisFromQ(const Quatern<T> &q, T *ang, TVect3<T> &axis)
  {
  int i;
  T omega, s;
  Quatern<T> qn = q;
  qn.Normalize();
  omega = (T)acos(qn[3]);
  *ang = (T)2.0 * omega;
  s = (T)sin(omega);
  if (fabs(s) > (T).000001)
    for (i = 0; i < 3; i++)
      axis[i]  = qn[i] / s;
  else
    axis = TVect3<T>(0, 0, 0); // RB: Added this so axis gets initialized
  }

//      Compute [angle,axis] corresponding to the rotation from
//    p to q.  Function returns angle, sets axis.
template <class T>
T  QangAxis(const Quatern<T> & p, const Quatern<T> & q, TVect3<T> & axis)
  {
  T ang;
  AngleAxisFromQ(q / p, &ang, axis);
  return (ang);
  }

#define EPSILON2 (1.0e-8)

#define EPSILON3 0.001

template <class T>
void QuatToEuler(Quatern<T> &q, T *ang)
  {
  TMatrix43<T> mat;

  q.MakeMatrix(mat);

  T sinY, cosY;

  TMatrix3<T> tm = mat.Matrix3();
  sinY = tm[0][2];
  if (sinY < -1.0)
    sinY = -1.0;
  if (sinY > 1.0)
    sinY = 1.0;
  cosY = (T)sqrt(1.0f - sinY * sinY);
  if (tm[0][0] < 0.0 && tm[2][2] < 0.0)
    cosY = -cosY;
  if (fabs(cosY) > EPSILON3)
    {
    ang[0] = (T)atan2(tm[1][2] / cosY, tm[2][2] / cosY);
    ang[1] = (T) - atan2(sinY, cosY);
    ang[2] = (T)atan2(tm[0][1] / cosY, tm[0][0] / cosY);
    }
  else
    {
    ang[0] = (T)atan2(-tm[2][1], tm[1][1]);
    ang[1] = (T) - asin(sinY);
    ang[2] = 0.0f;
    }
  }

//void EulerToQuat(T *ang, Quat &q)
//      {
//  TMatrix43<T> tm(1);
//      tm.RotateX(ang[0]);
//      tm.RotateY(ang[1]);
//      tm.RotateZ(ang[2]);
//      q = Quatern<T>(tm);
//      }

template <class T>
Quatern<T>::Quatern(const AngleAxis<T> &aa)
  {
  // Make the angle between 0-180
  T angle = aa.angle;
  TVect3<T> axis = aa.axis;
  if (angle < 0.0)
    {
    angle = -angle;
    axis  = -axis;
    }
  while (angle > PI2)
    angle -= (T)PI2;
  if (fabs(angle) > PI)
    angle -= (T)PI2;
  if (angle < 0.0)
    {
    angle = -angle;
    axis  = -axis;
    }

  *this = QFromAngleAxis(aa.angle, aa.axis);
  }

//---------------------------------------------------------------------
template <class T>
AngleAxis<T>::AngleAxis(const Quatern<T> &q)
  {
  AngleAxisFromQ(q, &angle, axis);
  }

template <class T>
int AngleAxis<T>::GetNumRevs()
  {
  return int(angle / PI2);
  }

template <class T>
void AngleAxis<T>::SetNumRevs(int num)
  {
  angle += T(num) * PI2;
  }

//-----------------------------------------------------------
//        Instantiations
//-----------------------------------------------------------

LIB_TEMPLATE_INSTANCE(Quatern, double)
LIB_TEMPLATE_INSTANCE(Quatern, float)


INTEGRA_NAMESPACE_END

// 'quatern.cpp' 
