/// @internal
/// @file
///
/// @brief Niederreiter Random number generator
///
/// @author Rkd - Kirill Dmitriev
///
/// Copyright &copy; INTEGRA, Inc., 2002.

#include <integra.hpp>

#include <base/base.hpp>

#include "nrnd.hpp"

INTEGRA_NAMESPACE_START

// ----------------------------------------------------
//                   Niederreiter class
// ----------------------------------------------------

/**
 * @class Niederreiter math/nrnd.hpp
 *
 * Class for Niederreiter Random number generator.
 *
 * Generator of potentially infinite multi-dimensional Niederreiter sequence in
 * base 2.
 *
 * @note Algorithm of that sequence generation, and also source codes on
 * FORTRAN  are available in the following paper:
 * {bratley92lds,
 * - author = "B. Fox and P. Bratley and H. Niederreiter",
 * - title = "Implementation and test of low discrepancy sequences",
 * - text = "B. L. Fox, P. Bratley, and H. Niederreiter. Implementation and test of
 *     low discrepancy sequences. ACM Trans. Model. Comput. Simul., 2(3):195--213,
 *     July 1992.",
 *   year = "1992" }
 */

// Number of bits actually used in sample
const int Niederreiter::NIEDERREITER_NBTS = 63;

// 1 / 2^63
const double Niederreiter::NIEDERREITER_INVV = 1. / 9223372036854775808.0;

INT64 **Niederreiter::cj;
int *Niederreiter::polynomials;
int Niederreiter::instances = 0;


///////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// @param sample0 - index of quasi-random vector to start with
/// @param iseed - seed for initialization of base Rnd class
Niederreiter::Niederreiter(INT64 sample0, unsigned iseed) : Rnd(iseed)
  {
  if (cj == NULL)
    {
    GenerateCJ();
    }

  sample = sample0;
  dindex = 0;
  for (dindex = NIEDERREITER_NDMS - 1; dindex >= 0; --dindex)
    {
    cache[dindex][0] = cache[dindex][1] = 0;
    }

  GenerateDValue();
  instances++;
  }

///////////////////////////////////////////////////////////////////////////////
/// Destructor releases the all allocated buffers
Niederreiter::~Niederreiter()
  {
  instances--;
  if (instances == 0)
    {
    for (int i = 0; i < NIEDERREITER_NDMS; i++)
      delete [] cj[i];
    delete [] cj;
    cj = NULL;
    }
  }

///////////////////////////////////////////////////////////////////////////////
/// Generate double value, corresponding to current 'sample' and 'dindex'
void Niederreiter::GenerateDValue()
  {
  if (++dindex >= NIEDERREITER_NDMS)
    dvalue = Rnd::DRnd();
  else
    {
    INT64 gray, *cj0 = cj[dindex], irnd;
    gray = sample ^ cache[dindex][0];
    irnd = cache[dindex][1];
    while (gray)
      {
      if (gray & 1)
        {
        irnd ^= *cj0;
        }
      cj0++;
      gray >>= 1;
      }
    cache[dindex][0] = sample;
    cache[dindex][1] = irnd;

    dvalue = (double)irnd * NIEDERREITER_INVV;
    Assert(0.0 <= dvalue && dvalue < 1.0);
    }
  }

///////////////////////////////////////////////////////////////////////////////
/// Generate table of irreducable polynomials.
void Niederreiter::GeneratePolynomials()
  {
  int i, n, p1, p2, l;
  int e_p1, e_p2, e_b;
  int buffer[NIEDERREITER_NDMS];

  //  generate all polynomials to buffer
  for (n = 1, buffer[0] = 0x2, p2 = 0, l = 4; n < NIEDERREITER_NDMS; ++n)
    {
    //  search for the next irreducable polynomial
    for (p1 = buffer[n - 1] + 1; ; ++p1)
      {
      //  find degree of polynomial p1
      for (e_p1 = 30; (p1 & (1 << e_p1)) == 0; --e_p1);

      //  try to divide p1 by all polynomials in buffer
      for (i = 0; i < n; ++i)
        {
        //  find the degree of buffer[i]
        for (e_b = e_p1; (buffer[i] & (1 << e_b)) == 0; --e_b);

        //  divide p2 by buffer[i] until the end
        for (p2 = (buffer[i] << ((e_p2 = e_p1) - e_b)) ^ p1; p2 >= buffer[i]; p2 = (buffer[i] << (e_p2 - e_b)) ^ p2)
          {
          //  compute new degree of p2
          for ( ; (p2 & (1 << e_p2)) == 0; --e_p2);
          }

        //  division without remainder!!! p1 is not irreducable
        if (p2 == 0)
          {
          break;
          }
        }
      //  all visisions were with remainder - p1 is irreducable
      if (p2 != 0)
        {
        buffer[n] = p1;
        l  += e_p1 + 2;
        break;
        }
      }
    }

  //  convert all polynomials from buffer to polynomials table
  polynomials = new int[l];
  for (n = 0, l = 0; n < NIEDERREITER_NDMS; ++n)
    {
    //  find degree of polynomial p1
    for (p1 = buffer[n], e_p1 = 30; (p1 & (1 << e_p1)) == 0; --e_p1);

    //  fill polynomials table with values for this polynomial
    polynomials[l++] = 1;
    for (--e_p1; e_p1 >= 0; --e_p1)
      {
      polynomials[l++] = (p1 >> e_p1) & 1;
      }
    polynomials[l++] = -1;
    }
  polynomials[l] = -1;
  }

void Niederreiter::GenerateCJ()
  {
  GeneratePolynomials();

  //  irreducable polynomial p
  int *p = polynomials, e, d;
  //  polynomial b
  int  b_arr[1024], *b, m;
  //  v array
  int     v_arr[1024], *v;
  //  temporary polynomial, required to do multiplication of p and b
  int     t_arr[1024], *t;
  //  subsidiary variables
  int i, j, u, m1, ip, it;

  cj = new INT64 *[NIEDERREITER_NDMS];

  //  cycle over monic irreducible polynomials
  for (d = 0; p[0] != -1; p += e + 2)
    {
    //  allocate memory for cj array for dimention (ip + 1)
    INT64 *cj_own = new INT64[NIEDERREITER_NBTS];
    for (i = 0; i < NIEDERREITER_NBTS; ++i)
      {
      cj_own[i] = 0;
      }

    //  determine the power of irreducable polynomial
    for (e = 0; p[e + 1] != -1; ++e);
    //  polynomial b in the beginning is just '1'
    (b = b_arr + 1023)[m = 0] = 1;
    //  v array needs only (NIEDERREITER_NBTS + e - 2) length
    v = v_arr + 1023 - (NIEDERREITER_NBTS + e - 2);

    //  cycle over all coefficients
    for (j = NIEDERREITER_NBTS - 1, u = e; j >= 0; --j, ++u)
      {
      if (u == e)
        {
        u  = 0;
        //  multiply b by p (polynomials multiplication)
        for (i = 0, t = t_arr + 1023 - (m1 = m); i <= m; ++i)
          {
          t[i] = b[i];
          }
        b = b_arr + 1023 - (m += e);

        for (i = 0; i <= m; ++i)
          {
          b[i] = 0;
          for (ip = e - (m - i), it = m1; ip <= e && it >= 0; ++ip, --it)
            {
            if (ip >= 0)
              {
              b[i] ^= p[ip] & t[it];
              }
            }
          }
        //  multiplication of polynomials finished

        //  calculate v
        for (i = 0; i < m1; ++i)
          {
          v[i] = 0;
          }
        for ( ; i < m; ++i)
          {
          v[i] = 1;
          }
        for ( ; i <= NIEDERREITER_NBTS + e - 2; ++i)
          {
          v[i] = 0;
          for (it = 1; it <= m; ++it)
            {
            v[i] ^= v[i - it] & b[it];
            }
          }
        }

      //  copy calculated v to cj
      for (i = 0; i < NIEDERREITER_NBTS; ++i)
        {
        cj_own[i] |= (INT64)v[i + u] << j;
        }
      }
    cj[d++] = cj_own;
    }
  Swap(cj[1], cj[2]);

  delete []polynomials;
  polynomials = NULL;
  }

INTEGRA_NAMESPACE_END
