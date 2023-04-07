/*   HEADER NAME     ALGEBRA.H
 *
 *   PURPOSE
 *            This module contains a set of useful macrodefinitions
 *            realizing standard algebraic operations with vectors
 *            and matrices of 3D euclidean space.
 *            All vectors should be declared as:  double name[3];
 *            (or poiners to double value).
 *            All matrices should be declared as: double name[3][3];
 *            (or double *name[3]).
 *            name - any name of variable
 *            double either float or double (double is recommended).
 *
 *            All macrodefinitions marked 3D corresponds to vectors
 *            with 3 coordinates (example: double vec[3];)
 *            All macrodefinitions marked 2D corresponds to vectors
 *            with 2 coordinates (example: double vec[2];)
 *            All macrodefinitions marked 2x2 corresponds to matrices
 *            with 2 lines and 2 columns (example: double mat[2][2];)
 *            All macrodefinitions marked 2x3 corresponds to matrices
 *            with 2 lines and 3 columns (example: double mat[2][3];)
 *            All macrodefinitions marked 3x3 corresponds to matrices
 *            with 3 lines and 3 columns (example: double mat[3][3];)
 *
 *   NOTES
 *
 * algebra.h,v
 * Revision 4.54  1997/08/06  15:44:43  tbt
 * TBT 5.20
 *
 * Revision 4.52  1996/12/19  18:05:55  tbt
 * TBT 5.10 release
 *
 * Revision 4.51  1995/01/03  06:36:08  cvs
 * =
 *
 * Revision 4.50  1993/08/13  10:45:42  rjs
 * AI version 4.50
 *
 *
 *    Rev 3.0   02 May 1991 17:02:24   'R(AW)'
 * environment/overlap mapping added
 *
 *    Rev 2.23   26 Nov 1990  3:47:28   'R(AW)'
 * Second logfile, initial revision
 *
 *    Rev 1.3   21 Jul 1990  3:53:20   M(JS)
 * Corrections after verification (RTEX02VI.RKM)
 *
 *    Rev 1.2   12 Jul 1990 12:33:30   M(JS)
 * Corrections after validation
 *
 *    Rev 1.1   04 Jul 1990 11:02:08   M(JS)
 * No change.
 *
 *    Rev 1.0   30 Jun 1990  8:17:08   M(JS)
 * Initial revision.
 *
 */

#define MAX2(a, b)     (((a) > (b)) ? (a) : (b))
#define MIN2(a, b)     (((a) < (b)) ? (a) : (b))
#define MEAN2(a, b)    (((a) + (b)) / 2)

#define MAX3(a, b, c)  (MAX2((a), MAX2((b), (c))))
#define MIN3(a, b, c)  (MIN2((a), MIN2((b), (c))))

/*---------------------------------------------------------------------------*/
/* Vector correctness tools  (TRUE - OK, FALSE - otherwise)                  */
/*---------------------------------------------------------------------------*/

#define VECTOR_3D_OK(v)\
  ((v)[0] < MAX_FLOAT && (v)[0] > -MAX_FLOAT &&\
   (v)[1] < MAX_FLOAT && (v)[1] > -MAX_FLOAT &&\
   (v)[2] < MAX_FLOAT && (v)[2] > -MAX_FLOAT)

#define VECTOR_2D_OK(v)\
  ((v)[0] < MAX_FLOAT && (v)[0] > -MAX_FLOAT &&\
   (v)[1] < MAX_FLOAT && (v)[1] > -MAX_FLOAT)

#define VALUE_IS_OK(v)\
  ((v) < MAX_FLOAT && (v) > -MAX_FLOAT)

/*---------------------------------------------------------------------------*/
/* Vector cross product of vectors ub, w, stored in vector v.                */
/* in case of 2D only value.                                                 */
/*---------------------------------------------------------------------------*/

#define CROSS_PRODUCT_3D(v, u, w)\
  {\
  v[0] = (u)[1] * (w)[2] - (u)[2] * (w)[1];\
  v[1] = (u)[2] * (w)[0] - (u)[0] * (w)[2];\
  v[2] = (u)[0] * (w)[1] - (u)[1] * (w)[0];\
  ASSERT(VECTOR_3D_OK(v))\
  }

#define CROSS_PRODUCT_2D(u, v) ((u)[0] * (v)[1] - (u)[1] * (v)[0])


/*---------------------------------------------------------------------------*/
/* Vector dot product value of vectors u and v = DOT_PRODUCT(u, v);          */
/*---------------------------------------------------------------------------*/

#define OWN_DOT_PRODUCT_3D(u, v)\
  ((u)[0] * (v)[0] + (u)[1] * (v)[1] + (u)[2] * (v)[2])

#define OWN_DOT_PRODUCT_2D(u, v) ((u)[0] * (v)[0] + (u)[1] * (v)[1])


#ifdef DEBUG

#define DOT_PRODUCT_3D(u, v)\
 (VECTOR_3D_OK(u) && VECTOR_3D_OK(v) ? \
  OWN_DOT_PRODUCT_3D(u, v) : \
  printf("DOT_PRODUCT_3D domain error : %s %d (%e %e %e)*(%e %e %e)\n", \
  __FILE__, __LINE__, (u)[0], (u)[1], (u)[2], (v)[0], (v)[1], (v)[2]), \
  OWN_DOT_PRODUCT_3D(u, v))

#define DOT_PRODUCT_2D(u, v)\
 (VECTOR_2D_OK(u) && VECTOR_2D_OK(v) ? \
  OWN_DOT_PRODUCT_2D(u, v) : \
  printf("DOT_PRODUCT_2D domain error : %s %d (%e %e)*(%e %e)\n", \
  __FILE__, __LINE__, (u)[0], (u)[1], (v)[0], (v)[1]), \
  OWN_DOT_PRODUCT_2D(u, v))

#else

#define DOT_PRODUCT_2D OWN_DOT_PRODUCT_2D
#define DOT_PRODUCT_3D OWN_DOT_PRODUCT_3D

#endif


/*---------------------------------------------------------------------------*/
/* Norm (i.e. lenght or so called absolute value of a vector v) = NORM(v);   */
/* attention: function sqrt needs <math.h> and returns double value          */
/*---------------------------------------------------------------------------*/

#define NORM_3D(v) sqrt((double)DOT_PRODUCT_3D(v, v))

#define NORM_2D(v) sqrt((double)DOT_PRODUCT_2D(v, v))

#define OWN_DIST_3D(p1, p2) sqrt((double)\
                         (((p1)[0] - (p2)[0]) * ((p1)[0] - (p2)[0]) +\
                          ((p1)[1] - (p2)[1]) * ((p1)[1] - (p2)[1]) +\
                          ((p1)[2] - (p2)[2]) * ((p1)[2] - (p2)[2])))

#define OWN_DIST_2D(p1, p2) sqrt((double)\
                         (((p1)[0] - (p2)[0]) * ((p1)[0] - (p2)[0]) +\
                          ((p1)[1] - (p2)[1]) * ((p1)[1] - (p2)[1])))

#ifdef DEBUG

#define DIST_3D(p1, p2)\
 (VECTOR_3D_OK(p1) && VECTOR_3D_OK(p2) ? \
  OWN_DIST_3D(p1, p2) : \
  printf("DIST_3D domain error : %s %d (%e %e %e)-(%e %e %e)\n", \
  __FILE__, __LINE__, (p1)[0], (p1)[1], (p1)[2], (p2)[0], (p2)[1], (p2)[2]), \
  OWN_DIST_3D(p1, p2))

#define DIST_2D(p1, p2)\
 (VECTOR_2D_OK(p1) && VECTOR_2D_OK(p2) ? \
  OWN_DIST_2D(p1, p2) : \
  printf("DIST_2D domain error : %s %d (%e %e)-(%e %e2)\n", \
  __FILE__, __LINE__, (p1)[0], (p1)[1], (p2)[0], (p2)[1]), \
  OWN_DIST_2D(p1, p2))

#else

#define DIST_3D  OWN_DIST_3D
#define DIST_2D  OWN_DIST_2D

#endif

/*---------------------------------------------------------------------------*/
/* Normalization of a vector v. A vector becames a versor (i.e vector with   */
/* lenght equal 1.0) keeping the same direction as it had before.            */
/*---------------------------------------------------------------------------*/

#ifdef DEBUG

#define NORMALIZE_3D(v)\
  {\
  double v_length;\
  ASSERT(VECTOR_3D_OK(v))\
  v_length = NORM_3D(v);\
  if (v_length < EPS_FLOAT)\
    printf("DIVISION BY ZERO : %s %d (%e)\n", __FILE__, __LINE__, v_length);\
  v[0] /= v_length;\
  v[1] /= v_length;\
  v[2] /= v_length;\
  }

#define NORMALIZE_2D(v)\
  {\
  double v_length;\
  ASSERT(VECTOR_2D_OK(v))\
  v_length = NORM_2D(v);\
  if (v_length < EPS_FLOAT)\
    printf("DIVISION BY ZERO : %s %d (%e)\n", __FILE__, __LINE__, v_length);\
  v[0] /= v_length;\
  v[1] /= v_length;\
  }

#else

#define NORMALIZE_3D(v)\
  {\
  double v_length;\
  v_length = NORM_3D(v);\
  v[0] /= v_length;\
  v[1] /= v_length;\
  v[2] /= v_length;\
  }

#define NORMALIZE_2D(v)\
  {\
  double v_length;\
  v_length = NORM_2D(v);\
  v[0] /= v_length;\
  v[1] /= v_length;\
  }

#endif


/*---------------------------------------------------------------------------*/
/* Trigonometric functions of angle between two vectors for 2D and 3D cases  */
/*---------------------------------------------------------------------------*/

#define COSINE_3D(u, v) (DOT_PRODUCT_3D(u, v) / (NORM_3D(u) * NORM_3D(v)))

#define SINE_2D(u, v) (CROSS_PRODUCT_2D(u, v) / (NORM_2D(u) * NORM_2D(v)))

#define COSINE_2D(u, v) (DOT_PRODUCT_2D(u, v) / (NORM_2D(u) * NORM_2D(v)))


/*---------------------------------------------------------------------------*/
/* Transformation of a vector into another one using a matrix.               */
/*  The transformation can be represented by a 3D algebraic equation:        */
/*  u = a * v;                                                               */
/*                                                                           */
/* u[3]    target vector                                                     */
/* v[3]    source vector                                                     */
/* a[3][3] transformation matrix                                             */
/*---------------------------------------------------------------------------*/

#define AFIN_TRANSFORM_3x3(u, a, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1] + (a)[0][2] * (v)[2];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1] + (a)[1][2] * (v)[2];\
  u[2] = (a)[2][0] * (v)[0] + (a)[2][1] * (v)[1] + (a)[2][2] * (v)[2];\
  ASSERT(VECTOR_3D_OK(u))\
  }

#define AFIN_TRANS_3x3(u, a, b, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1] + (a)[0][2] * (v)[2] + (b)[0];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1] + (a)[1][2] * (v)[2] + (b)[1];\
  u[2] = (a)[2][0] * (v)[0] + (a)[2][1] * (v)[1] + (a)[2][2] * (v)[2] + (b)[2];\
  ASSERT(VECTOR_3D_OK(u))\
  }

#define TRANSFORM_2x3(u, a, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1] + (a)[0][2] * (v)[2];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1] + (a)[1][2] * (v)[2];\
  ASSERT(VECTOR_2D_OK(u))\
  }

#define TRANSFORM_3x2(u, a, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1];\
  u[2] = (a)[2][0] * (v)[0] + (a)[2][1] * (v)[1];\
  ASSERT(VECTOR_3D_OK(u))\
  }

#define TRANSFORM_2x2(u, a, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1];\
  ASSERT(VECTOR_2D_OK(u))\
  }

/*---------------------------------------------------------------------------*/
/* Affine transformation of 2D plane onto another 2D plane using 3X3 affine  */
/* transformation matrix (analogous use as above)                            */
/*---------------------------------------------------------------------------*/

#define AFFIN_TRANSFORM(u, a, v)\
  {\
  u[0] = (a)[0][0] * (v)[0] + (a)[0][1] * (v)[1] + (a)[0][2];\
  u[1] = (a)[1][0] * (v)[0] + (a)[1][1] * (v)[1] + (a)[1][2];\
  ASSERT(VECTOR_2D_OK(u))\
  }

/*---------------------------------------------------------------------------*/
/* Determinator of a 3D matrix a = DET_3x3(a)                                */
/*---------------------------------------------------------------------------*/

#define DET_3x3(a)\
  ((a)[0][0] * MINOR_0_0_DET(a) +\
   (a)[1][0] * MINOR_1_0_DET(a) +\
   (a)[2][0] * MINOR_2_0_DET(a))

/*---------------------------------------------------------------------------*/
/* Determinators of minors of a 3D matrix                                    */
/* MINOR_n_m_DET(a) is  the determinant when n-line and m-column are deleted */
/* the signum (+1 or -1) are correct.                                        */
/*---------------------------------------------------------------------------*/

#define MINOR_0_0_DET(a) ((a)[1][1] * (a)[2][2] - (a)[1][2] * (a)[2][1])

#define MINOR_0_1_DET(a) ((a)[1][2] * (a)[2][0] - (a)[1][0] * (a)[2][2])

#define MINOR_0_2_DET(a) ((a)[1][0] * (a)[2][1] - (a)[1][1] * (a)[2][0])

#define MINOR_1_0_DET(a) ((a)[0][2] * (a)[2][1] - (a)[0][1] * (a)[2][2])

#define MINOR_1_1_DET(a) ((a)[0][0] * (a)[2][2] - (a)[0][2] * (a)[2][0])

#define MINOR_1_2_DET(a) ((a)[0][1] * (a)[2][0] - (a)[0][0] * (a)[2][1])

#define MINOR_2_0_DET(a) ((a)[0][1] * (a)[1][2] - (a)[0][2] * (a)[1][1])

#define MINOR_2_1_DET(a) ((a)[0][2] * (a)[1][0] - (a)[0][0] * (a)[1][2])

#define MINOR_2_2_DET(a) ((a)[0][0] * (a)[1][1] - (a)[0][1] * (a)[1][0])

/*---------------------------------------------------------------------------*/
/* Determinator of a 2D matrix a = DET_2x2(a)                                */
/*---------------------------------------------------------------------------*/
#define DET_2x2(a) ((a)[0][0] * (a)[1][1] - (a)[0][1] * (a)[1][0])

/*---------------------------------------------------------------------------*/
/* Conditional expressions with two possible values:                         */
/* 1 (when true), 0 (when false).                                            */
/*---------------------------------------------------------------------------*/

#define IS_ZERO_1D(v)     ((v) > -EPS_FLOAT && (v) < EPS_FLOAT)
#define IS_ZERO_2D(v)     (IS_ZERO_1D((v)[0]) && IS_ZERO_1D((v)[1]))
#define IS_ZERO_3D(v)\
  (IS_ZERO_1D((v)[0]) && IS_ZERO_1D((v)[1]) && IS_ZERO_1D((v)[2]))

#define NOT_ZERO_1D(v)     ((v) <= -EPS_FLOAT || (v) >= EPS_FLOAT)
#define NOT_ZERO_2D(v)     (NOT_ZERO_1D((v)[0]) || NOT_ZERO_1D((v)[1]))
#define NOT_ZERO_3D(v)\
  (NOT_ZERO_1D((v)[0]) || NOT_ZERO_1D((v)[1]) || NOT_ZERO_1D((v)[2]))


#define NO_VECTOR_2D(v)  IS_ZERO_2D(v)
#define NO_VECTOR_3D(v)  IS_ZERO_3D(v)

#define THE_SAME_3D(u, v) ((u)[0] == (v)[0] && (u)[1] == (v)[1] && (u)[2] == (v)[2])

#define THE_SAME_2D(u, v) ((u)[0] == (v)[0] && (u)[1] == (v)[1])

#define VERY_CLOSE_3D(u, v)\
  (IS_ZERO_1D((u)[0] - (v)[0]) &&\
   IS_ZERO_1D((u)[1] - (v)[1]) &&\
   IS_ZERO_1D((u)[2] - (v)[2]))

#define VERY_CLOSE_2D(u, v)\
  (IS_ZERO_1D((u)[0] - (v)[0]) &&\
   IS_ZERO_1D((u)[1] - (v)[1]))

/*---------------------------------------------------------------------------*/
/* A set of macros CLEAR stores zero values in all components of vectors and */
/* matrices. These macros are useful to clear (initiate) automatic arrays.   */
/*---------------------------------------------------------------------------*/

#define CLEAR_3D(v)\
  {\
  v[0] = v[1] = v[2] = 0.0;\
  }

#define CLEAR_2D(v)\
  {\
  v[0] = v[1] = 0.0;\
  }

#define CLEAR_3x3(v)\
  {\
  CLEAR_3D(v[0])\
  CLEAR_3D(v[1])\
  CLEAR_3D(v[2])\
  }

#define CLEAR_2x3(v)\
  {\
  CLEAR_3D(v[0])\
  CLEAR_3D(v[1])\
  }

#define CLEAR_3x2(v)\
  {\
  CLEAR_2D(v[0])\
  CLEAR_2D(v[1])\
  CLEAR_2D(v[2])\
  }

#define CLEAR_2x2(v)\
  {\
  CLEAR_2D(v[0])\
  CLEAR_2D(v[1])\
  }


/*---------------------------------------------------------------------------*/
/* copying 3D and 2D vectors and matrices u - destination, v - source        */
/*---------------------------------------------------------------------------*/
#define MOVE_VECTOR_3D(u, v)\
  {\
  ASSERT(VECTOR_3D_OK(u))\
  v[0] = (u)[0];\
  v[1] = (u)[1];\
  v[2] = (u)[2];\
  }

#define COPY_3D(u, v) MOVE_VECTOR_3D(v, u)

#define COPY_2D(u, v)\
  {\
  ASSERT(VECTOR_2D_OK(v))\
  u[0] = (v)[0];\
  u[1] = (v)[1];\
  }

#define COPY_3x3(a, b)\
  {\
  COPY_3D((a)[0], (b)[0])\
  COPY_3D((a)[1], (b)[1])\
  COPY_3D((a)[2], (b)[2])\
  }

#define COPY_2x3(a, b)\
  {\
  COPY_3D((a)[0], (b)[0])\
  COPY_3D((a)[1], (b)[1])\
  }

#define COPY_2x2(a, b)\
  {\
  COPY_2D((a)[0], (b)[0])\
  COPY_2D((a)[1], (b)[1])\
  }

#define COPY_3x2(a, b)\
  {\
  COPY_2D((a)[0], (b)[0])\
  COPY_2D((a)[1], (b)[1])\
  COPY_2D((a)[2], (b)[2])\
  }


/*---------------------------------------------------------------------------*/
/* sums of the vectors and matrices a = b + c.                               */
/*---------------------------------------------------------------------------*/
#define SUM_3D(a, b, c)\
  {\
  a[0] = (b)[0] + (c)[0];\
  a[1] = (b)[1] + (c)[1];\
  a[2] = (b)[2] + (c)[2];\
  ASSERT(VECTOR_3D_OK(a))\
  }

#define SUM_2D(a, b, c)\
  {\
  a[0] = (b)[0] + (c)[0];\
  a[1] = (b)[1] + (c)[1];\
  ASSERT(VECTOR_2D_OK(a))\
  }

#define SUM_WITH_WEIGHT_3D(a, b, weight_b, c, weight_c)\
  {\
  a[0] = (b)[0] * (weight_b) + (c)[0] * (weight_c);\
  a[1] = (b)[1] * (weight_b) + (c)[1] * (weight_c);\
  a[2] = (b)[2] * (weight_b) + (c)[2] * (weight_c);\
  ASSERT(VECTOR_3D_OK(a))\
  }


#define SUBSTR_2D(a, b)\
  {\
  a[0] -= (b)[0];\
  a[1] -= (b)[1];\
  ASSERT(VECTOR_2D_OK(a))\
  }

#define SUB_2D(c, a, b)\
  {\
  c[0] = (a)[0] - (b)[0];\
  c[1] = (a)[1] - (b)[1];\
  ASSERT(VECTOR_2D_OK(c))\
  }

#define SUB_3D(c, a, b)\
  {\
  c[0] = (a)[0] - (b)[0];\
  c[1] = (a)[1] - (b)[1];\
  c[2] = (a)[2] - (b)[2];\
  ASSERT(VECTOR_3D_OK(c))\
  }

#define SUBSTR_3D(a, b)\
  {\
  a[0] -= (b)[0];\
  a[1] -= (b)[1];\
  a[2] -= (b)[2];\
  ASSERT(VECTOR_3D_OK(a))\
  }

#define SUM_3x3(a, b, c)\
  {\
  SUM_3D((a)[0], (b)[0], (c)[0])\
  SUM_3D((a)[1], (b)[1], (c)[1])\
  SUM_3D((a)[2], (b)[2], (c)[2])\
  }

#define SUM_3x2(a, b, c)\
  {\
  SUM_2D((a)[0], (b)[0], (c)[0])\
  SUM_2D((a)[1], (b)[1], (c)[1])\
  SUM_2D((a)[2], (b)[2], (c)[2])\
  }

#define SUM_2x3(a, b, c)\
  {\
  SUM_3D((a)[0], (b)[0], (c)[0])\
  SUM_3D((a)[1], (b)[1], (c)[1])\
  }

#define SUM_2x2(a, b, c)\
  {\
  SUM_2D((a)[0], (b)[0], (c)[0])\
  SUM_2D((a)[1], (b)[1], (c)[1])\
  }


/*---------------------------------------------------------------------------*/
/* multiplying vectors and matrices by a real value                          */
/*---------------------------------------------------------------------------*/
#define MULTIPLY_3D_BY_VALUE(vector, value)\
  {\
  vector[0] *= value;\
  vector[1] *= value;\
  vector[2] *= value;\
  ASSERT(VECTOR_3D_OK(vector))\
  }

#define MULTIPLY_2D_BY_VALUE(vector, value)\
  {\
  vector[0] *= value;\
  vector[1] *= value;\
  ASSERT(VECTOR_2D_OK(vector))\
  }

#define MULTIPLY_3x3_BY_VALUE(matrix, value)\
  {\
  MULTIPLY_3D_BY_VALUE(matrix[0], value);\
  MULTIPLY_3D_BY_VALUE(matrix[1], value);\
  MULTIPLY_3D_BY_VALUE(matrix[2], value);\
  }

#define MULTIPLY_3x2_BY_VALUE(matrix, value)\
  {\
  MULTIPLY_2D_BY_VALUE(matrix[0], value);\
  MULTIPLY_2D_BY_VALUE(matrix[1], value);\
  MULTIPLY_2D_BY_VALUE(matrix[2], value);\
  }

#define MULTIPLY_2x3_BY_VALUE(matrix, value)\
  {\
  MULTIPLY_3D_BY_VALUE(matrix[0], value);\
  MULTIPLY_3D_BY_VALUE(matrix[1], value);\
  }

#define MULTIPLY_2x2_BY_VALUE(matrix, value)\
  {\
  MULTIPLY_2D_BY_VALUE(matrix[0], value);\
  MULTIPLY_2D_BY_VALUE(matrix[1], value);\
  }

#define MULT_3D(a, b)\
  {\
  a[0] *= (b)[0];\
  a[1] *= (b)[1];\
  a[2] *= (b)[2];\
  ASSERT(VECTOR_3D_OK(a))\
  }

/*---------------------------------------------------------------------------*/
/* Multiplying two square matrices: a = b * c.                               */
/* MULT_ELEMENT is a work use macro (result of multiplying a row by column)  */
/*---------------------------------------------------------------------------*/
#define MULT_ELEMENT_3x3(a, b, c, i, j)\
  {\
  a[i][j] = (b)[i][0] * (c)[0][j] + (b)[i][1] * (c)[1][j] + (b)[i][2] * (c)[2][j];\
  }


#define MULTIPLE_3x3(a, b, c)\
  {\
  MULT_ELEMENT_3x3(a, b, c, 0, 0);\
  MULT_ELEMENT_3x3(a, b, c, 0, 1);\
  MULT_ELEMENT_3x3(a, b, c, 0, 2);\
  MULT_ELEMENT_3x3(a, b, c, 1, 0);\
  MULT_ELEMENT_3x3(a, b, c, 1, 1);\
  MULT_ELEMENT_3x3(a, b, c, 1, 2);\
  MULT_ELEMENT_3x3(a, b, c, 2, 0);\
  MULT_ELEMENT_3x3(a, b, c, 2, 1);\
  MULT_ELEMENT_3x3(a, b, c, 2, 2);\
  }

#define MULT_ELEMENT_2x2(a, b, c, i, j)\
  {\
  a[i][j] = (b)[i][0] * (c)[0][j] + (b)[i][1] * (c)[1][j];\
  }

#define MULTIPLE_2x2(a, b, c)\
  {\
  MULT_ELEMENT_2x2(a, b, c, 0, 0);\
  MULT_ELEMENT_2x2(a, b, c, 0, 1);\
  MULT_ELEMENT_2x2(a, b, c, 1, 0);\
  MULT_ELEMENT_2x2(a, b, c, 1, 1);\
  }


/*---------------------------------------------------------------------------*/
/* Geometric macrodefinitions.                                               */
/*---------------------------------------------------------------------------*/
#define POINT_ON_LINE(point, start_point, vector, t)\
  {\
  point[0] = start_point[0] + vector[0] * t;\
  point[1] = start_point[1] + vector[1] * t;\
  point[2] = start_point[2] + vector[2] * t;\
  ASSERT(VECTOR_3D_OK(point))\
  }

#define TRIANGLE_CENTRE_3D(centre, a, b, c)\
  {\
  centre[0] = (a)[0] + (b)[0] + (c)[0];\
  centre[1] = (a)[1] + (b)[1] + (c)[1];\
  centre[2] = (a)[2] + (b)[2] + (c)[2];\
  centre[0] /= 3.0;\
  centre[1] /= 3.0;\
  centre[2] /= 3.0;\
  ASSERT(VECTOR_3D_OK(centre))\
  }

#define TRIANGLE_CENTRE_2D(centre, a, b, c)\
  {\
  centre[0] = (a)[0] + (b)[0] + (c)[0];\
  centre[1] = (a)[1] + (b)[1] + (c)[1];\
  centre[0] /= 3.0;\
  centre[1] /= 3.0;\
  ASSERT(VECTOR_2D_OK(centre))\
  }

/*---------------------------------------------------------------------------*/
/* creating a vector as a difference between two points                      */
/*---------------------------------------------------------------------------*/
#define CREATE_VECTOR_3D(vector, start_point, end_point)\
  {\
  vector[0] = end_point[0] - start_point[0];\
  vector[1] = end_point[1] - start_point[1];\
  vector[2] = end_point[2] - start_point[2];\
  ASSERT(VECTOR_3D_OK(vector))\
  }

#define CREATE_VECTOR_2D(vector, start_point, end_point)\
  {\
  vector[0] = end_point[0] - start_point[0];\
  vector[1] = end_point[1] - start_point[1];\
  ASSERT(VECTOR_2D_OK(vector))\
  }

/*---------------------------------------------------------------------------*/
/* creating a vector when all components are given                           */
/*---------------------------------------------------------------------------*/
#define LOAD_3D(vector, value_0, value_1, value_2)\
  {\
  vector[0] = value_0;\
  vector[1] = value_1;\
  vector[2] = value_2;\
  ASSERT(VECTOR_3D_OK(vector))\
  }

#define LOAD_2D(vector, value_0, value_1)\
  {\
  vector[0] = value_0;\
  vector[1] = value_1;\
  ASSERT(VECTOR_2D_OK(vector))\
  }


#define SUBTRACT_3D(a, b, c) CREATE_VECTOR_3D(a, c, b)
#define SUBTRACT_2D(a, b, c) CREATE_VECTOR_2D(a, c, b)



/*---------------------------------------------------------------------------*/
/* Transposing matrices (the line and columns swaps)                         */
/* SWAP() - is work use macro.                                               */
/*---------------------------------------------------------------------------*/
#ifndef SWAP
#define SWAP(x, y)\
    {\
    double z;\
    z = x;\
    x = y;\
    y = z;\
    }
#endif

#define TRANSPOSE_3x3(a)\
  {\
  SWAP((a)[1][0], (a)[0][1]);\
  SWAP((a)[2][0], (a)[0][2]);\
  SWAP((a)[2][1], (a)[1][2]);\
  }

#define TRANSPOSE_2x2(a) SWAP((a)[1][0], (a)[0][1])

#define SWAP_2D(u, v)\
  {\
  SWAP((u)[0], (v)[0]);\
  SWAP((u)[1], (v)[1]);\
  }

#define SWAP_3D(u, v)\
  {\
  SWAP((u)[0], (v)[0]);\
  SWAP((u)[1], (v)[1]);\
  SWAP((u)[2], (v)[2]);\
  }


/* ----------------------------------------------------------------- */
/* Negate vector (turn over): change sign each of component          */
/* ----------------------------------------------------------------- */
#define NEG_2D(u)\
  {\
  ASSERT(VECTOR_2D_OK(u))\
  u[0] = -u[0];\
  u[1] = -u[1];\
  }

#define NEG_3D(u)\
  {\
  ASSERT(VECTOR_3D_OK(u))\
  u[0] = -u[0];\
  u[1] = -u[1];\
  u[2] = -u[2];\
  }

/* ----------------------------------------------------------------- */
/* Orient vector: change sign if DOT_PRODUCT(v, u) < 0               */
/* ----------------------------------------------------------------- */
#define ORIENT_VECTOR_2D(v, u)\
  {\
  if (DOT_PRODUCT_2D(v, u) < 0.0)\
    NEG_2D(v);\
  }

#define ORIENT_VECTOR_3D(v, u)\
  {\
  if (DOT_PRODUCT_3D(v, u) < 0.0)\
    NEG_3D(v);\
  }

/* ----------------------------------------------------------------- */
/* Vector projection                                                 */
/* pr = projection of v onto u                                       */
/* ----------------------------------------------------------------- */
#define PROJECT_2D(projection, u, v)\
  {\
  COPY_2D(projection, u)\
  MULTIPLY_2D_BY_VALUE(projection,\
            DOT_PRODUCT_2D(u, v) / DOT_PRODUCT_2D(u, u))\
  }

#define PROJECT_3D(projection, u, v)\
  {\
  COPY_3D(projection, u)\
  MULTIPLY_3D_BY_VALUE(projection,\
            DOT_PRODUCT_3D(u, v) / DOT_PRODUCT_3D(u, u))\
  }

/* ----------------------------------------------------------------- */
/* Additive vector operation                                         */
/* u += v                                                            */
/* ----------------------------------------------------------------- */
#define ADD_3D_VALUE(u, v)\
  {\
  u[0] += (v);\
  u[1] += (v);\
  u[2] += (v);\
  ASSERT(VECTOR_3D_OK(u))\
  }

#define ADD_2D(u, v)\
  {\
  u[0] += (v)[0];\
  u[1] += (v)[1];\
  ASSERT(VECTOR_2D_OK(u))\
  }
 
#define ADD_3D(u, v)\
  {\
  u[0] += (v)[0];\
  u[1] += (v)[1];\
  u[2] += (v)[2];\
  ASSERT(VECTOR_3D_OK(u))\
  }

/* ----------------------------------------------------------------- */
/* Additive vector operation with weight                             */
/* u += a * v                                                        */
/* ----------------------------------------------------------------- */
#define ADD_WITH_WEIGHT_2D(u, a, v)\
  {\
  u[0] += (a) * (v)[0];\
  u[1] += (a) * (v)[1];\
  ASSERT(VECTOR_2D_OK(u))\
  }

#define ADD_WITH_WEIGHT_3D(u, a, v)\
  {\
  u[0] += (a) * (v)[0];\
  u[1] += (a) * (v)[1];\
  u[2] += (a) * (v)[2];\
  ASSERT(VECTOR_3D_OK(u))\
  }


/*---------------------------------------------------------------------------*/
/* Clip value A to constraints a_min and a_max                               */
/*---------------------------------------------------------------------------*/
/* in fact there can be a compiler warning: "unreachable code" */
#define CLIP_1D(a, a_min, a_max)\
  {\
  if (a < (a_min))\
    a = (a_min);\
  else if (a > (a_max))\
    a = (a_max);\
  ASSERT((a_min) <= (a) && (a) <= (a_max))\
  }

/*---------------------------------------------------------------------------*/
/* A set of macros SET stores some values in all components of vectors and   */
/* matrices. These macros are useful to set (initiate) automatic arrays.     */
/*---------------------------------------------------------------------------*/

#define SET_3D(v, a)\
  {\
  v[0] = v[1] = v[2] = (a);\
  ASSERT(VECTOR_3D_OK(v))\
  }

#define SET_2D(v, a)\
  {\
  v[0] = v[1] = (a);\
  ASSERT(VECTOR_2D_OK(v))\
  }

#define SET_3x3(v, a)\
  {\
  SET_3D(v[0], a);\
  SET_3D(v[1], a);\
  SET_3D(v[2], a);\
  }

#define SET_2x3(v, a)\
  {\
  SET_3D(v[0], a);\
  SET_3D(v[1], a);\
  }

#define SET_3x2(v, a)\
  {\
  SET_2D(v[0], a);\
  SET_2D(v[1], a);\
  SET_2D(v[2], a);\
  }

#define SET_2x2(v, a)\
  {\
  SET_2D(v[0], a);\
  SET_2D(v[1], a);\
  }


/*--------------------------------------------------------------------------*/
/* END OF 'ALGEBRA.H' FILE */
