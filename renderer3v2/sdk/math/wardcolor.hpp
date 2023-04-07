/// @file
///
/// @brief Image and ImageType class declarations.
///
/// Copyright &copy; INTEGRA, Inc., 2002.
///
/// @internal
/// @author Oek - Edward Kopylov, RUIH '02.10.01.
/// @author Pls - Leva Shapiro, RUIH '02.10.01.


#ifndef _RUIH_WARDCOLOR_HPP_
#define _RUIH_WARDCOLOR_HPP_

#include <base/base.hpp>
#include <math/vect3.hpp>
#include <math/vect4.hpp>

INTEGRA_NAMESPACE_START

// NAME         WardColor
// PURPOSE      Class for color presentation in the Ward form
// PARAMETERS   None
// INHERITED
// NOTES        One byte for fraction of each color component (RGB),
//              and on byte for the same for all components exponent
//

/// @brief Class WardColor is used for color presentation in compact form.
/// One byte for fraction of each color component (RGB),
/// and on byte for the same for all components exponent.
/// Technique is proposed by Greg Ward, and is described in:
/// Greg Ward, "Real Pixels". (Graphics Gems II, pp. 80-84)
class WardColor
  {
  public:
    /// Default constructor.
    inline WardColor();
    /// Constructor from Vect3f.
    inline WardColor(const Vect3f &source);
    /// Constructor from Vect3d.
    inline WardColor(const Vect3d &source);
    /// Constructor from DWORD.
    inline WardColor(DWORD rgbe);
    /// Constructor from four bytes.
    inline WardColor(BYTE r, BYTE g, BYTE b, BYTE e);
    /// Copy constructor.
    inline WardColor(const WardColor &source);

    /// Assignment operator.
    inline void operator = (const WardColor &source);
    /// Assignment operator from Vect3f.
    inline void operator = (const Vect3f &source);
    /// Assignment operator from Vect3d.
    inline void operator = (const Vect3d &source);
    /// Multiplication to a scalar value.
    inline WardColor operator * (double d) const;
    /// Addition of WardColors
    inline WardColor operator + (const WardColor &u) const;

    /// Conversion to Vect3f.
    inline void GetColor(Vect3f &rgb) const;
    /// Conversion to Vect3d.
    inline void GetColor(Vect3d &rgb) const;
    /// Conversion to Vect3f.
    inline Vect3f GetColorF() const;
    /// Conversion to Vect3d.
    inline Vect3d GetColorD() const;
		/// Get RGBE representation.
    inline DWORD GetRGBE() const;
    /// Get RGBE representation as Set4b.
    inline void GetRGBE(Set4b &rgbe) const;

  public:
    /// @name Equality operators.
    //@{
    /// Equality operator.
    inline bool operator == (const WardColor &src) const;
    /// Inequality operator.
    inline bool operator != (const WardColor &src) const;
    //@}

  private:
    /// Transform 3 doubles to the DWORD representation.
    inline DWORD GetWardColor(double dr, double dg, double db) const;
    /// Transform internal DWORD representation to the 3 doubles.
    inline void GetRGBColor(double &x, double &y, double &z) const;

    /// Internal color representation;
    DWORD m_rgbe;
  };

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
WardColor::WardColor()
  {
  m_rgbe = 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from Vect3f.
/// @param[in] source Input color.
WardColor::WardColor(const Vect3f &source)
  {
  m_rgbe = GetWardColor(source.x, source.y, source.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from Vect3d.
/// @param[in] source Input color.
WardColor::WardColor(const Vect3d &source)
  {
  m_rgbe = GetWardColor(source.x, source.y, source.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor from RGBE representation.
/// @param[in] rgbe Input color in the RGBE representation.
WardColor::WardColor(DWORD rgbe)
  {
  m_rgbe = rgbe;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
/// @param[in] r R color component.
/// @param[in] g G color component.
/// @param[in] b B color component.
/// @param[in] e E color component.
WardColor::WardColor(BYTE r, BYTE g, BYTE b, BYTE e)
  {
  m_rgbe = ((unsigned long)r) |
         ((unsigned long)g << 8) |
         ((unsigned long)b << 16) |
         ((unsigned long)e << 24);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] source Source color.
WardColor::WardColor(const WardColor &source)
  {
  m_rgbe = source.m_rgbe;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator.
/// @param[in] source Source color.
void WardColor::operator = (const WardColor &source)
  {
  m_rgbe = source.m_rgbe;
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator from Vect3f.
/// @param[in] source Source color.
void WardColor::operator = (const Vect3f &source)
  {
  m_rgbe = GetWardColor(source.x, source.y, source.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Assignment operator from Vect3d.
/// @param[in] source Source color.
void WardColor::operator = (const Vect3d &source)
  {
  m_rgbe = GetWardColor(source.x, source.y, source.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Multiplication to a scalar value.
/// @param d - Given scalar.
/// @return Result of operation.
WardColor WardColor::operator * (double d) const
  {
  Vect3d rgb;
  GetColor(rgb);
  rgb *= d;
  return WardColor(rgb);
  }

//////////////////////////////////////////////////////////////////////////////
/// Addition of WardColors
/// @param u - Given scalar.
/// @return Result of operation.
WardColor WardColor::operator + (const WardColor &u) const
  {
  Vect3d rgb;
  GetColor(rgb);
  Vect3d rgb2;
  u.GetColor(rgb2);
  rgb += rgb2;
  return WardColor(rgb);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion to Vect3f.
/// @param[out] rgb Output color.
void WardColor::GetColor(Vect3f &rgb) const
  {
  double x, y, z;
  GetRGBColor(x, y, z);
  rgb.x = (float)x;
  rgb.y = (float)y;
  rgb.z = (float)z;
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion to Vect3d.
/// @param[out] rgb Output color.
void WardColor::GetColor(Vect3d &rgb) const
  {
  GetRGBColor(rgb.x, rgb.y, rgb.z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion to Vect3f.
/// @return Output color.
Vect3f WardColor::GetColorF() const
  {
  double x, y, z;
  GetRGBColor(x, y, z);
  return Vect3f(x, y, z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Conversion to Vect3d.
/// @return Output color.
Vect3d WardColor::GetColorD() const
  {
  double x, y, z;
  GetRGBColor(x, y, z);
  return Vect3d(x, y, z);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get RGBE representation.
/// @return RGBE representation.
DWORD WardColor::GetRGBE() const
  {
  return m_rgbe;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get RGBE representation as Set4b.
/// @param[out] rgbe RGBE representation.
void WardColor::GetRGBE(Set4b &rgbe) const
  {
  rgbe = Set4b::Cast((BYTE *)&m_rgbe);
  }

//////////////////////////////////////////////////////////////////////////////
/// Transform 3 doubles to the DWORD representation.
/// @param[in] x R color component.
/// @param[in] y G color component.
/// @param[in] z B color component.
/// @ return color in DWORD representation.
DWORD WardColor::GetWardColor(double x, double y, double z) const
  {
  DWORD wcol;
  Assert(x >= 0 && y >= 0 && z >= 0);
  
  double max_d = Max3(x, y, z);

  if (max_d < 1e-32)
    wcol = 0;
  else
    {
    int exponent;
    double mantissa = frexp(max_d, &exponent);
    double m = mantissa * 256.0 / max_d; // multiplier to range 0 - 255
    wcol = ((DWORD)(x * m)) |
           ((DWORD)(y * m) << 8) |
           ((DWORD)(z * m) << 16) |
           ((DWORD)(exponent + 128) << 24);
    }
  return wcol;
  }

//////////////////////////////////////////////////////////////////////////////
/// Transform internal DWORD representation to the 3 doubles.
/// @param[out] x R color component.
/// @param[out] y G color component.
/// @param[out] z B color component.
void WardColor::GetRGBColor(double &x, double &y, double &z) const
  {
  if (m_rgbe == 0)
    x = y = z = 0;

  double f = ldexp(1.0, (m_rgbe >> 24) - 136);
  x = f * (m_rgbe & 0xFF);
  y = f * ((m_rgbe >> 8) & 0xFF);
  z = f * ((m_rgbe >> 16) & 0xFF);
  }

////////////////////////////////////////////////////////////////////////////
/// Equality operator.
/// @param[in] src - Object for comparison.
/// @return true if objects are equal.
bool WardColor::operator == (const WardColor &src) const
  {
  return m_rgbe == src.m_rgbe;
  }

//////////////////////////////////////////////////////////////////////////////
/// Inequality operator.
/// @param[in] src - Object for comparison.
/// @return true if objects are not equal.
bool WardColor::operator != (const WardColor &src) const
  {
  return m_rgbe != src.m_rgbe;
  }

INTEGRA_NAMESPACE_END

#endif    // _RUIH_WARDCOLOR_HPP_

// End of wardcolor.hpp file
