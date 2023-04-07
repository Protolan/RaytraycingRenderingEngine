/// @file
///
/// @brief Declarations of Time and Timer classes.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01

#ifndef _KLBC_TIME_HPP_
#define _KLBC_TIME_HPP_

#include "export.h"
#include "str.hpp"

INTEGRA_NAMESPACE_START

/// Timers.
class Timer
  {
  public:
    /// @name Constructors, destructor
    //@{
    /// Default constructor.
    INTAPI_BASE Timer();
    /// Copy constructor.
    INTAPI_BASE Timer(const Timer &t);
    /// Destructor.
    INTAPI_BASE ~Timer();
    //@}

  public:
    /// @name Methods
    //@{
    /// Reset the timer.
    INTAPI_BASE void Reset();
    /// Get elapsed time in milliseconds.
    INTAPI_BASE unsigned int Elapsed() const;
    /// Wait for the specified time.
    INTAPI_BASE unsigned int Wait(unsigned int time) const;
    /// Get time in a user friendly form.
    INTAPI_BASE UStr TimeStamp(bool show_ms = false) const;
    //@}

  private:
    /// Get time period.
    static void GetTime(int &out_sec, int &out_msec);

    /// @name Counters
    //@{
    /// Seconds.
    int sec;
    /// Milliseconds.
    int msec;
    //@}
  }; // class Timer

/// Date, time in the habitual form.
class Time
  {
  public:
    /// @name Constructors
    //@{
    /// Default constructor.
    inline Time();
    /// Constructor.
    inline Time(const unsigned t);
    /// Constructor.
    INTAPI_BASE Time(const int year0, const int month0, const int day0,
                     const int hour0 = 0, const int min0 = 0,
                     const int sec0 = 0);
    /// Copy constructor.
    INTAPI_BASE Time(const Time &lt);
    //@}

  public:
    /// Get real current time (not related with time kept in this class).
    INTAPI_BASE static unsigned int Get();

  public:
    /// @name Methods
    //@{
    /// Check - if the time is correct.
    INTAPI_BASE bool IsCorrect() const;
    /// Set time from a string of a particular format.
    INTAPI_BASE bool SetFromSekisuiString(const Str &str);
    /// Get date in a user friendly form.
    INTAPI_BASE UStr DateStamp() const;
    /// Get date in a user friendly form (digital).
    INTAPI_BASE UStr DateStampD() const;
    /// Get time in a user friendly form.
    INTAPI_BASE UStr TimeStamp() const;
    /// Create a string from date and time, which can be used as file name.
    INTAPI_BASE UStr DateTimeString() const;
    /// Convert time to the unsigned int.
    INTAPI_BASE operator unsigned () const;
    /// Set specified time.
    INTAPI_BASE void Set(const unsigned t);
    //@}

  public:
    /// @name Serialization methods
    //@{
    /// Serialize the object.
    INTAPI_BASE void Serialize(Serializer &inout, const char *tag);
    /// Low level serialization.
    INTAPI_BASE void Value(Serializer &inout);
    //@}

  public:
    /// @name Equality operators
    //@{
    /// Equality operator.
    INTAPI_BASE bool operator == (const Time &tc) const;
    /// Inequality operator.
    INTAPI_BASE bool operator != (const Time &tc) const;
    //@}

  public:
    /// @name Components
    //@{
    /// Must be >= 1970.
    int year;
    /// Must be in the range 1-12.
    int month;
    /// Must be in the range 1-31.
    int day;
    /// Must be in the range 0-23.
    int hour;
    /// Must be in the range 0-59.
    int minute;
    /// Must be in the range 0-59.
    int second;
    //@}

  }; // class Time


/// Get processors ticks from switching on.
INTAPI_BASE void GetTicksNumber(INT64 *num);


//////////////////////////////////////////////////////////////////////////////
// Inline methods of Time class

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Initializes the object by current time.
Time::Time()
  {
  Set(Time::Get());
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// Initializes the object by the specified time.
/// @param[in] t Time, number of seconds passed since 00:00:00 1970.01.01.
Time::Time(const unsigned t)
  {
  Set(t);
  }

INTEGRA_NAMESPACE_END
#endif
