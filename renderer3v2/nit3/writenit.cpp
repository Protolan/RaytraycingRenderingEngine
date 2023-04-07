#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include "integra.h"

#include "base/str.hpp"
#include "base/file.hpp"
#include "base/marray.hpp"
#include "math/matrix43.hpp"
#include "math/vect2.hpp"
#include "math/vect3.hpp"
#include "base/matrix.hpp"

START_C_DECLS
 #include "ievl.h"
// #include "iosl.h"
#include "icol.h"
#include "imal.h"
#include "iifl.h"`
#include "itoliifl.h"
// #include "itolscan.h"
// #include "suffix.h"
END_C_DECLS

#define C_NUMB_IIF_COMP 5

//////////////////////////////////////////////////////////////////////////////
/// Write NIT file of observer from luminance matrix of RGB components
/// @param nitfile - observer file(IN)
/// @param coldata - matrix of color luminance distribution(IN)
/// @param negvalue - how to process negative values
/// (-1 - keep as is, 0 - move to zero, 1 - reverse sign)(IN)
/// @return SUCCESS/FAILURE
OKAY WriteNITFile(const PathStr &nitfile,
                  const TMatrix<Vect3d> &coldata, int negvalue)
  {
  double white[XY], red[XY], green[XY], blue[XY];
  Str buf, buf1, buf2, layers, format, type;
  int i, j, k;
  float *pf, **table;
  IIF *iif_file;
  INT64 rays;

  if ((iif_file = iif_open(nitfile.XData(), "w")) == NULL)
    {
    printf("\nIt is impossible to create observer file - %s",
           nitfile.XData());
    return FAILURE;
    }

  // Initialize resolution and component types
  layers = "lum red,lum gre,lum blu,lum acc,lum ray";
  format = "fffff";
  type = "LUMINANCE";
  if (iif_init_file(iif_file, coldata.NColumns(), coldata.NRows(),
      layers.XData(), format.XData()) != IIF_OK)
    {
    printf("\nIt is impossible to create observer file - %s",
           nitfile.XData());
    iif_close(iif_file);
    return FAILURE;
    }

  if (iif_put_var(iif_file, "FILE_TYPE", type.XData()) != IIF_OK)
    {
    printf("\nIt is impossible to create observer file - %s",
           nitfile.XData());
    iif_close(iif_file);
    return FAILURE;
    }

  if (iif_put_var(iif_file, "image pixel step", "1 1") != IIF_OK)
    {
    printf("\nIt is impossible to create observer file - %s",
           nitfile.XData());
    iif_close(iif_file);
    return FAILURE;
    }

  buf.Printf("1 1");
  if (iif_put_var(iif_file, "step size [m]", buf.XData()) != IIF_OK)
    {
    printf("\nIt is impossible to create observer file - %s",
           nitfile.XData());
    iif_close(iif_file);
    return FAILURE;
    }

  if (col_get_wrgb(white, red, green, blue) != COL_OK)
    ASSERT(FALSE)
  else
    {
    if (iif_put_var(iif_file, "GAMUT", "Yes") != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }

    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), white[X]);
    if (iif_put_var(iif_file, "WHITE_X", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), white[Y]);
    if (iif_put_var(iif_file, "WHITE_Y", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }

    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), red[X]);
    if (iif_put_var(iif_file, "RED_X", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), red[Y]);
    if (iif_put_var(iif_file, "RED_Y", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }

    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), green[X]);
    if (iif_put_var(iif_file, "GREEN_X", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), green[Y]);
    if (iif_put_var(iif_file, "GREEN_Y", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }

    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), blue[X]);
    if (iif_put_var(iif_file, "BLUE_X", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s", 
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    buf.Printf(IF_V_FORMAT(IF_V_GAMUT), blue[Y]);
    if (iif_put_var(iif_file, "BLUE_Y", buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s",
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    rays = 0;
    buf.Printf(IF_V_FORMAT(IF_V_RAY_NUMBER), rays);
    if (iif_put_var(iif_file, IF_V_NAME(IF_V_RAY_NUMBER),
                    buf.XData()) != IIF_OK)
      {
      printf("\nIt is impossible to create observer file - %s",
             nitfile.XData());
      iif_close(iif_file);
      return FAILURE;
      }
    }

  /* Set data pointers */
  table = new float *[C_NUMB_IIF_COMP];
  if (table == NULL)
    {
    printf("\nMemory allocation error - LUX file");
    iif_close(iif_file);
    delete[] table;
    return FAILURE;
    }
  table[R] = new float[coldata.NColumns() * C_NUMB_IIF_COMP];

  if (table[R] == NULL)
    {
    iif_close(iif_file);
    printf("\nMemory allocation error - NIT file");
    return FAILURE;
    }
  table[G] = table[R] + G * coldata.NColumns();
  table[B] = table[R] + B * coldata.NColumns();
  table[B + 1] = table[R] + (B + 1) * coldata.NColumns();
  table[B + 2] = table[R] + (B + 2) * coldata.NColumns();

  /* Print image */
  for (j = 0; j < coldata.NRows(); j++)
    {
    for (k = 0; k < RGB; k++)
      {
      pf = table[k];
      for (i = 0; i < coldata.NColumns(); i++)
        {
        if (negvalue > 0)
          *pf++ = (float)Abs(coldata(j, i)[k]);
        else if (negvalue == 0 && coldata(j, i)[k] < 0)
          *pf++ = (float)0;
        else
          *pf++ = (float)(coldata(j, i)[k]);
        }
      }
    for (; k < C_NUMB_IIF_COMP; k++)
      {
      pf = table[k];
      for (i = 0; i < coldata.NColumns(); i++)
        *pf++ = (float)0;
      }

    if (iif_write_line(iif_file, (void *)table, j, -1, 0) != IIF_OK)
      {
      printf("\nIt is impossible to write observer file - %s",
             nitfile.XData());
      iif_close(iif_file);
      delete[] table[R];
      delete[] table;
      return FAILURE;
      }
    }

  // Close iif_file 
  delete[] table[R];
  delete[] table;
  iif_close(iif_file);
  return SUCCESS;
  } // End of WriteNITFile()

//////////////////////////////////////////////////////////////////////////
/// Embree device error callback function
void DeviceErrorFunction(void* userPtr, enum RTCError error, const char* str)
  {
  printf("Device error %d: %s\n", error, str);
  }

//////////////////////////////////////////////////////////////////////////
/// Create a box object for Embree
/// @param[in] device Embree device
/// @param[in] p Box origin points
/// @param[in] size Box sizes along 3 axis
/// @param[in] tr Transformation matrix to apply to created geometry
/// @return Embree geometry object containing box
RTCGeometry CreateBox(RTCDevice device, Point3f p, Vect3f size, Matrix43f tr)
  {
  RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  float* vertices = (float*)rtcSetNewGeometryBuffer(geom,
                                                    RTC_BUFFER_TYPE_VERTEX,
                                                    0,
                                                    RTC_FORMAT_FLOAT3,
                                                    3 * sizeof(float),
                                                    8);

  unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer(geom,
                                                          RTC_BUFFER_TYPE_INDEX,
                                                          0,
                                                          RTC_FORMAT_UINT3,
                                                          3 * sizeof(unsigned),
                                                          12);
  if (vertices && indices)
    {
    Point3f points[8];
    points[0] = p;
    points[1] = p + Vect3f(size.x, 0, 0);
    points[2] = p + Vect3f(size.x, 0, size.z);
    points[3] = p + Vect3f(0, 0, size.z);
    points[4] = p + Vect3f(0, size.y, size.z);
    points[5] = p + Vect3f(0, size.y, 0);
    points[6] = p + Vect3f(size.x, size.y, 0);
    points[7] = p + size;
    
    for (int i = 0; i < 8; i++)
      {
      tr.PointTransform(points[i]);
      vertices[i * 3] = points[i].x;
      vertices[i * 3 + 1] = points[i].y;
      vertices[i * 3 + 2] = points[i].z;
      }

    int i = 0;
    indices[i++] = 0;
    indices[i++] = 1;
    indices[i++] = 2;

    indices[i++] = 2;
    indices[i++] = 3;
    indices[i++] = 0;

    indices[i++] = 4;
    indices[i++] = 5;
    indices[i++] = 0;

    indices[i++] = 0;
    indices[i++] = 3;
    indices[i++] = 4;

    indices[i++] = 7;
    indices[i++] = 6;
    indices[i++] = 5;

    indices[i++] = 5;
    indices[i++] = 4;
    indices[i++] = 7;

    indices[i++] = 3;
    indices[i++] = 2;
    indices[i++] = 7;

    indices[i++] = 7;
    indices[i++] = 4;
    indices[i++] = 3;

    indices[i++] = 7;
    indices[i++] = 2;
    indices[i++] = 1;

    indices[i++] = 1;
    indices[i++] = 6;
    indices[i++] = 7;

    indices[i++] = 0;
    indices[i++] = 5;
    indices[i++] = 6;

    indices[i++] = 6;
    indices[i++] = 1;
    indices[i++] = 1;
    }

  rtcCommitGeometry(geom);

  return geom;
  }

/// Which box faces to omit flags
enum OmitFace
  {
  /// Omit nothing
  OMIT_NONE = 0,
  /// Omit +X face
  OMIT_X_POS = 1,
  // Omit -X face
  OMIT_X_NEG = 2,
  /// Omit +Y face
  OMIT_Y_POS = 4,
  /// Omit -Y face
  OMIT_Y_NEG = 8,
  /// Omit +Z face
  OMIT_Z_POS = 16,
  /// Omit -Z face
  OMIT_Z_NEG = 32
  };

//////////////////////////////////////////////////////////////////////////
/// Create a box object for Embree. Some faces may be omitted.
/// @param[in] device Embree device
/// @param[in] p Box origin points
/// @param[in] size Box sizes along 3 axis
/// @param[in] omit A mask of which faces to omit
/// @param[in] tr Transformation matrix to apply to created geometry
/// @return Embree geometry object containing box
RTCGeometry CreateBoxOmit(RTCDevice device, Point3f p, Vect3f size, int omit, Matrix43f tr)
  {
  int omits[6] = { 1, 2, 4, 8, 16, 32 };

  int omit_count = 0;
  for (int i = 0; i < 6; i++)
    if (omit & omits[i])
      omit_count++;

  RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  float* vertices = (float*)rtcSetNewGeometryBuffer(geom,
                                                    RTC_BUFFER_TYPE_VERTEX,
                                                    0,
                                                    RTC_FORMAT_FLOAT3,
                                                    3 * sizeof(float),
                                                    8);

  unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer(geom,
                                                         RTC_BUFFER_TYPE_INDEX,
                                                         0,
                                                         RTC_FORMAT_UINT3,
                                                         3 * sizeof(unsigned),
                                                         12 - omit_count * 2);
  if (vertices && indices)
    {
    Point3f points[8];
    points[0] = p;
    points[1] = p + Vect3f(size.x, 0, 0);
    points[2] = p + Vect3f(size.x, 0, size.z);
    points[3] = p + Vect3f(0, 0, size.z);
    points[4] = p + Vect3f(0, size.y, size.z);
    points[5] = p + Vect3f(0, size.y, 0);
    points[6] = p + Vect3f(size.x, size.y, 0);
    points[7] = p + size;

    for (int i = 0; i < 8; i++)
      {
      tr.PointTransform(points[i]);
      vertices[i * 3] = points[i].x;
      vertices[i * 3 + 1] = points[i].y;
      vertices[i * 3 + 2] = points[i].z;
      }

    int i = 0;
    if (!(omit & OMIT_Y_NEG))
      {
      indices[i++] = 0;
      indices[i++] = 1;
      indices[i++] = 2;

      indices[i++] = 2;
      indices[i++] = 3;
      indices[i++] = 0;
      }

    if (!(omit & OMIT_X_NEG))
      {
      indices[i++] = 4;
      indices[i++] = 5;
      indices[i++] = 0;

      indices[i++] = 0;
      indices[i++] = 3;
      indices[i++] = 4;
      }

    if (!(omit & OMIT_Y_POS))
      {
      indices[i++] = 7;
      indices[i++] = 6;
      indices[i++] = 5;

      indices[i++] = 5;
      indices[i++] = 4;
      indices[i++] = 7;
      }

    if (!(omit & OMIT_Z_POS))
      {
      indices[i++] = 3;
      indices[i++] = 2;
      indices[i++] = 7;

      indices[i++] = 7;
      indices[i++] = 4;
      indices[i++] = 3;
      }

    if (!(omit & OMIT_X_POS))
      {
      indices[i++] = 7;
      indices[i++] = 2;
      indices[i++] = 1;

      indices[i++] = 1;
      indices[i++] = 6;
      indices[i++] = 7;
      }

    if (!(omit & OMIT_Z_NEG))
      {
      indices[i++] = 0;
      indices[i++] = 5;
      indices[i++] = 6;

      indices[i++] = 6;
      indices[i++] = 1;
      indices[i++] = 1;
      }
    }

  rtcCommitGeometry(geom);

  return geom;
  }

//////////////////////////////////////////////////////////////////////////
/// Subdivide sphere face
/// @param[in] v1 First vertex of face to subdivide
/// @param[in] v2 Second vertex of face to subdivide
/// @param[in] v3 Third vertex of face to subdivide
/// @param[in] i1 Index of the first vertex
/// @param[in] i2 Index of the second vertex
/// @param[in] i3 Index of the third vertex
/// @param[in, out] sphere_pints Array of vertexes
/// @param[in, out] sphere_indices Array of triangle indices
/// @param[in] depth Subdivision depth
void SphereSubdivide(Vect3f v1, Vect3f v2, Vect3f v3, const unsigned i1, const unsigned i2, const unsigned i3,
                     TArray<Vect3f> &sphere_points, TArray<Vect3u> &sphere_indices, unsigned int depth) {
  if (depth == 0)
    {
    sphere_indices.Add(Vect3u(i1, i2, i3));
    return;
    }

  Vect3f v12 = (v1 + v2).Normalize();
  Vect3f v23 = (v2 + v3).Normalize();
  Vect3f v31 = (v3 + v1).Normalize();

  unsigned i12 = sphere_points.Length();
  unsigned i23 = i12 + 1;
  unsigned i31 = i23 + 1;

  sphere_points.Add(v12);
  sphere_points.Add(v23);
  sphere_points.Add(v31);

  SphereSubdivide(v1, v12, v31, i1, i12, i31, sphere_points, sphere_indices, depth - 1);
  SphereSubdivide(v2, v23, v12, i2, i23, i12, sphere_points, sphere_indices, depth - 1);
  SphereSubdivide(v3, v31, v23, i3, i31, i23, sphere_points, sphere_indices, depth - 1);
  SphereSubdivide(v12, v23, v31, i12, i23, i31, sphere_points, sphere_indices, depth - 1);
  }

//////////////////////////////////////////////////////////////////////////
/// Start subdividing a sphere
/// @param[out] sphere_pints Array of vertexes
/// @param[out] sphere_indices Array of triangle indices
/// @param[in] depth Subdivision depth
void InitSphere(TArray<Vect3f> &sphere_points, TArray<Vect3u> &sphere_indices, unsigned int depth)
  {
  const float X0 = 0.525731112119133606f;
  const float Z0 = 0.850650808352039932f;
  sphere_points.Truncate();
  sphere_points.Add({ -X0, 0.0,  Z0 });
  sphere_points.Add({  X0, 0.0,  Z0 });
  sphere_points.Add({ -X0, 0.0, -Z0 });
  sphere_points.Add({  X0, 0.0, -Z0 });
  sphere_points.Add({ 0.0,  Z0,  X0 });
  sphere_points.Add({ 0.0,  Z0, -X0 });
  sphere_points.Add({ 0.0, -Z0,  X0 });
  sphere_points.Add({ 0.0, -Z0, -X0 });
  sphere_points.Add({  Z0,  X0, 0.0 });
  sphere_points.Add({ -Z0,  X0, 0.0 });
  sphere_points.Add({  Z0, -X0, 0.0 });
  sphere_points.Add({ -Z0, -X0, 0.0 });


  int tindices[20][3] =
    {
      {0, 4, 1},    { 0, 9, 4 },  { 9, 5, 4 },  { 4, 5, 8 },  { 4, 8, 1 },
      { 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 },  { 5, 2, 3 },  { 2, 7, 3 },
      { 7, 10, 3 }, { 7, 6, 10 }, { 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 },
      { 6, 1, 10 }, { 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 },  { 7, 2, 11 }
    };
  for (int i = 0; i < 20; i++)
    SphereSubdivide(sphere_points[tindices[i][0]], sphere_points[tindices[i][1]], sphere_points[tindices[i][2]], 
                    tindices[i][0], tindices[i][1], tindices[i][2],
                    sphere_points, sphere_indices, depth);
  }

//////////////////////////////////////////////////////////////////////////
/// Create a sphere object for Embree
/// @param[in] device Embree device
/// @param[in] center Sphere center
/// @param[in] radius Sphere radius
/// @param[in] tr Transformation matrix to apply to created geometry
/// @return Embree geometry object containing sphere
RTCGeometry CreateSphere(RTCDevice device, Point3f center, float radius, unsigned int depth, Matrix43f tr)
  {
  TArray<Vect3f> sphere_normals;
  TArray<Vect3u> sphere_indices;

  InitSphere(sphere_normals, sphere_indices, depth);

  TArray<Point3f> sphere_points;
  for (int i = 0; i < sphere_normals.Length(); i++)
    {
    sphere_points.Add(center + sphere_normals[i] * radius);
    tr.PointTransform(sphere_points[i]);
    }

  RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  float* vertices = (float*)rtcSetNewGeometryBuffer(geom,
                                                    RTC_BUFFER_TYPE_VERTEX,
                                                    0,
                                                    RTC_FORMAT_FLOAT3,
                                                    3 * sizeof(float),
                                                    sphere_points.Length());

  unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer(geom,
                                                         RTC_BUFFER_TYPE_INDEX,
                                                         0,
                                                         RTC_FORMAT_UINT3,
                                                         3 * sizeof(unsigned),
                                                         sphere_indices.Length());
  
  if (vertices && indices)
    {
    for (int i = 0; i < sphere_points.Length(); i++)
      {
      vertices[i * 3] = sphere_points[i].x;
      vertices[i * 3 + 1] = sphere_points[i].y;
      vertices[i * 3 + 2] = sphere_points[i].z;
      }

    for (int i = 0; i < sphere_indices.Length(); i++)
      {
      indices[i * 3] = sphere_indices[i].x;
      indices[i * 3 + 1] = sphere_indices[i].y;
      indices[i * 3 + 2] = sphere_indices[i].z;
      }
    }

  rtcCommitGeometry(geom);
  
  return geom;
  }

//////////////////////////////////////////////////////////////////////////
/// Program entry point.
int main()
  {
  mem_init(NULL, NULL, "temp.mem");
  ev_init();
  col_init();

  // Create device
  RTCDevice device = rtcNewDevice(NULL);
  if (device == NULL)
    {
    printf("Device error %d: cannot create device\n", rtcGetDeviceError(NULL));
    return 0;
    }
  rtcSetDeviceErrorFunction(device, DeviceErrorFunction, NULL);

  // Create scene
  RTCScene scene = rtcNewScene(device);

  // Create box
  RTCGeometry box = CreateBoxOmit(device, Point3f(0, 0, 0), Vect3f(1, 1, 1), OMIT_X_POS, Matrix43f(1, 1, 1));
  rtcAttachGeometry(scene, box);

  // Create sphere
  RTCGeometry sphere = CreateSphere(device, Point3f(0, 0, 0), 1, 5, Matrix43f(1, 1, 1));
  rtcAttachGeometry(scene, sphere);

  // Commit scene to Embree
  rtcCommitScene(scene);

  // Try to trace rays
  int sx = 800, sy = 800;
  Point3f org(2, 0.5, 0.5);
  Vect3f up(0, 0, 2);
  Vect3f right(0, 2, 0);
  Vect3f dir(-1, 0, 0);

  // Create intersection context
  struct RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  TArray<struct RTCRayHit> rayhits;
  rayhits.Allocate(sx);
  TMatrix<Vect3d> m = TMatrix<Vect3d>(sy, sx);
  for (int i = 0; i < sy; i++)
    {
#if 1
    // Create bulk of rays
    for (int j = 0; j < sx; j++)
      {
      Point3f p = org + right * (j - sx / 2 + 0.5) / (sx - 1) + up * (i - sy / 2 + 0.5) / (sy - 1);

      rayhits[j].ray.org_x = p.x;
      rayhits[j].ray.org_y = p.y;
      rayhits[j].ray.org_z = p.z;
      rayhits[j].ray.dir_x = dir.x;
      rayhits[j].ray.dir_y = dir.y;
      rayhits[j].ray.dir_z = dir.z;
      rayhits[j].ray.tnear = 0;
      rayhits[j].ray.tfar = (float)MathF::MAX_VALUE;
      rayhits[j].ray.mask = 0;
      rayhits[j].ray.flags = 0;
      rayhits[j].hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rayhits[j].hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
      }

    // Intersect em all
    rtcIntersect1M(scene, &context, rayhits.Data(), sx, sizeof(RTCRayHit));

    // Check what we got
    for (int j = 0; j < sx; j++)
      {
      if (rayhits[j].hit.geomID != RTC_INVALID_GEOMETRY_ID)
        {
        m[i][j].x = rayhits[j].ray.tfar;
        m[i][j].y = rayhits[j].ray.tfar;
        m[i][j].z = rayhits[j].ray.tfar;
        }
      else
        {
        m[i][j].x = 0;
        m[i][j].y = 0;
        m[i][j].z = 0;
        }
      }

#else
    // Here we trace rays one-by-one
    for (int j = 0; j < sx; j++)
      {
      Point3f p = org + right * (j - sx / 2 + 0.5) / (sx - 1) + up * (i - sy / 2 + 0.5) / (sy - 1);

      // Init ray
      struct RTCRayHit rayhit;
      rayhit.ray.org_x = p.x;
      rayhit.ray.org_y = p.y;
      rayhit.ray.org_z = p.z;
      rayhit.ray.dir_x = dir.x;
      rayhit.ray.dir_y = dir.y;
      rayhit.ray.dir_z = dir.z;
      rayhit.ray.tnear = 0;
      rayhit.ray.tfar = (float)MathF::MAX_VALUE;
      rayhit.ray.mask = 0;
      rayhit.ray.flags = 0;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

      // Intersect
      rtcIntersect1(scene, &context, &rayhit);
      
      // Check if we hit one
      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
        {
        m[i][j].x = rayhit.ray.tfar;
        m[i][j].y = rayhit.ray.tfar;
        m[i][j].z = rayhit.ray.tfar;
        }
      else
        {
        m[i][j].x = 0;
        m[i][j].y = 0;
        m[i][j].z = 0;
        }
      }
#endif
    }

  rtcReleaseDevice(device);

  PathStr ps = PathStr("nit.nit");
  WriteNITFile(ps, m, 0);

  col_term();
  ev_term();
  mem_close();

  return 0;
}
