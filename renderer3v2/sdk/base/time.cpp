/// @internal
/// @file
///
/// @brief Definitions of Time and Timer classes.
///
/// @author Fgk - Gennadij Kirejko, '99.04.01
/// @author Fvg - Vjacheslav Glukhov, '99.04.01
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004

// WIN32
#ifdef _WIN32
#include <windows.h>
#else
#include <utime.h>
#include <sys/time.h>
#include <sys/wait.h>
#endif

// LINUX
#if (OS_MARK == 'L')
#include <utime.h>
#include <sys/time.h>
#include <sys/wait.h>
#endif

// IRIX (+ rest)
#if (OS_MARK == 'U' || OS_MARK == 'N' || OS_MARK == 'R')
#include <sys/time.h>
#include <sys/wait.h>
#include <utime.h>
#include <unistd.h>
#endif

//  For all platforms (ANSI)
#include <time.h>

// COMPANY INCLUDES
#include <integra.hpp>

#include "time.hpp"
#include "serializer.hpp"

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
// Static functions

/// Pause for specified time period.
static void TimeDelay(int millisec);

/**
@class Timer base/time.hpp

Timers.

The class Timer is used for calculation in milliseconds of the
period between two concrete moments.

@sa @ref base_mainpage
**/

//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
///
/// Starts the timer.
Timer::Timer()
  {
  GetTime(sec, msec);
  }

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
///
/// The copied timer keeps the same start time as a source one.
/// @param[in] sour - A source object.
Timer::Timer(const Timer &sour)
  {
  sec = sour.sec;
  msec = sour.msec;
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
Timer::~Timer()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Reset timer.
///
/// Restarts the timer.
void Timer::Reset()
  {
  GetTime(sec, msec);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get elapsed time in milliseconds.
/// @return Time passed since creation or the last restart of the timer,
/// in milliseconds.
unsigned int Timer::Elapsed() const
  {
  int loc_sec = 0;
  int loc_msec = 0;

  GetTime(loc_sec, loc_msec);

  // Calculate a differences of times in milliseconds
  return (unsigned int)((loc_sec - sec) * 1000 + (loc_msec - msec));
  }

//////////////////////////////////////////////////////////////////////////////
/// Wait for the specified time.
///
/// The method waits for the specified time since creation or the last restart
/// of the timer.
/// If this time is already over, the method returns immediately.
/// @param[in] tm - Time to wait, in milliseconds.
/// @return Difference between the specified and elapsed time.
/// It will be positive, if time is already over, and 0 otherwise.
unsigned Timer::Wait(unsigned int tm) const
  {
  unsigned int time_beg = Elapsed();

  // Check if time is not already over
  if (time_beg < tm)
    {
    TimeDelay(tm - time_beg);
    return 0;
    }

  // Otherwise: time is already over
  return (time_beg - tm);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get time in a user friendly form.
/// @param[in] show_ms - Specifies a format of time;
/// - @b true - time is printed in the form "hh:mm:ss.mmm";
/// - @b false - time is printed in the form "hh:mm:ss".
/// @return A string with time elapsed since creation or the last
/// restart of the timer.
UStr Timer::TimeStamp(bool show_ms) const
  {
  UStr s;

  // Get current time
  unsigned elapsed = Elapsed();

  // Calculate hours and minutes
  unsigned second = elapsed / 1000;

  unsigned hour = second / 3600;
  second %= 3600;

  unsigned minute = second / 60;
  second %= 60;

  if (show_ms)
    {
    unsigned ms = elapsed % 1000;
    s.Printf(INTEGRA_T("%2.2d:%2.2d:%2.2d.%3.3d"), hour, minute, second, ms);
    }
  else
    s.Printf(INTEGRA_T("%2.2d:%2.2d:%2.2d"), hour, minute, second);

  return s;
  }  // TimeStamp()

//////////////////////////////////////////////////////////////////////////////
/// Get time period.
///
/// The method returns time (seconds and milliseconds)
/// elapsed since creation or the last restart of the timer.
/// @param[out] out_sec - Elapsed seconds.
/// @param[out] out_msec - Elapsed milliseconds.
void Timer::GetTime(int &out_sec, int &out_msec)
  {

// WIN32
#if (OS_MARK == 'I')
  static int last_time = -1;
  static int w_day = -1;
  time_t curtime;
  SYSTEMTIME systime;

  GetLocalTime(&systime);
  if ((last_time == -1) || (w_day != systime.wDay))
    {
    time(&curtime);
    last_time = (int)(curtime - systime.wHour * 3600 -
                                systime.wMinute * 60 -
                                systime.wSecond);
    w_day = systime.wDay;
    }
  else
    {
    curtime = last_time + systime.wHour * 3600 +
                          systime.wMinute * 60 +
                          systime.wSecond;
    }

  out_sec = (int)curtime;
  out_msec = systime.wMilliseconds;
#endif

//  UNIX
#if (OS_MARK == 'H' || OS_MARK == 'U' || OS_MARK == 'N' || \
     OS_MARK == 'R' || OS_MARK == 'L')

  struct timeval tmval;
  struct timezone tmzone;

  // Receive current time
  gettimeofday(&tmval, &tmzone);

  out_sec = (int)(tmval.tv_sec);
  out_msec = (int)((long)tmval.tv_usec / 1000L);
#endif
  }  // GetTime()


/**
@class Time base/time.hpp

Date, time in the habitual form.

The class Time is used for obtaining the time in seconds passed
since 00:00:00 1970.01.01, and to present local time in the form
of year, month, day, hour, minute and second.

@sa @ref base_mainpage
**/

/// Days in the month in non leap-year.
static int month_size[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//////////////////////////////////////////////////////////////////////////////
/// Constructor.
///
/// Initializes the object by the specified date and time.
/// @param[in] year0 - Year.
/// @param[in] month0 - Month.
/// @param[in] day0 - Day.
/// @param[in] hour0 - Hour.
/// @param[in] min0 - Minute.
/// @param[in] sec0 - Second.
Time::Time(const int year0, const int month0, const int day0,
           const int hour0, const int min0, const int sec0)
  {
  year   = year0;
  month  = month0;
  day    = day0;
  hour   = hour0;
  minute = min0;
  second = sec0;
  }
  
//////////////////////////////////////////////////////////////////////////////
/// Check - if the time is correct.
bool Time::IsCorrect() const
  {
  if (month <= 0 || month > 12)
    return false;
  bool leap_year = year % 4 == 0;
  int days = month_size[month - 1];
  if (month == 2 && leap_year)
    days += 1;
    
  if (day <= 0 || day > days)
    return false;
  return (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 &&  
          second >= 0 && second <= 59);  
  }
  
//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @param[in] sour - A source object.
Time::Time(const Time &sour)
  {
  year   = sour.year;
  month  = sour.month;
  day  = sour.day;
  hour   = sour.hour;
  minute = sour.minute;
  second = sour.second;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get current time.
/// @return Current time, in seconds passed since 00:00:00 1970.01.01.
unsigned Time::Get()
  {
  time_t tmp_time;
  time(&tmp_time);
  return (unsigned)tmp_time;
  }

//////////////////////////////////////////////////////////////////////////////
/// Set time from a string with particular format.
///
/// The method sets data and time from a string of chars in the format
/// MMDDhhmm.
/// @param[in] str - Source time, in the format MMDDhhmm.
/// @return @b true if the format is acceptable; @b false otherwise.
bool Time::SetFromSekisuiString(const Str &str)
  {
  if (str.Length() != 8)
    return false;

  Set(Get());  // is necessary to set year

  Str str_month  = str.SubStr(0, 2);
  Str str_day    = str.SubStr(2, 2);
  Str str_hour   = str.SubStr(4, 2);
  Str str_minute = str.SubStr(6, 2);

  month = atoi(str_month);

  if (month < 1 || month > 12)
    return false;

  int m = month - 1;
  int days_in_month = month_size[m];

  if (year % 4 == 0 && m == 1) // February
    {
    days_in_month++;
    if (year % 100 == 0)
      {
      days_in_month--;
      if (year % 400 == 0)
        days_in_month++;
      }
    }

  day    = atoi(str_day);
  hour   = atoi(str_hour);
  minute = atoi(str_minute);

  if (day < 1 || day > days_in_month ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59)
    return false;

  second = 0;

  return true;
  }  // SetFromSekisuiString()

/// Array of month names.
static const char *months[] = {"Jan", "Feb", "Mar", "Apr",
                               "May", "Jun", "Jul", "Aug",
                               "Sep", "Oct", "Nov", "Dec"};

//////////////////////////////////////////////////////////////////////////////
/// Get date in a user friendly form.
/// @return A string with date in the format "Month dd, yyyy",
/// month is represented by 3 letters.
UStr Time::DateStamp() const
  {
  UStr s;
  Assert((1 <= month) && (month <= 12));
  s.Printf(INTEGRA_T("%s %02d, %04d"), months[month - 1], day, year);
  return s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get date in a user friendly form.
/// @return A string with date in the "yyyy/mm/dd" format,
/// month is represented by number.
UStr Time::DateStampD() const
  {
  UStr s;
  Assert((1 <= month) && (month <= 12));
  s.Printf(INTEGRA_T("%4d/%02d/%02d"), year, month, day);
  return s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get time in a user friendly form.
/// @return A string with time in the format "hh:mm:ss".
UStr Time::TimeStamp() const
  {
  UStr s;
  s.Printf(INTEGRA_T("%02d:%02d:%02d"), hour, minute, second);
  return s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Create a string from date and time, which can be used as file name.
/// @return A string with time in the format "hh:mm:ss".
UStr Time::DateTimeString() const
  {
  UStr s;
  s.Printf(INTEGRA_T("%04d_%02d_%02d_%02d_%02d_%02d"),
    year, month, day, hour, minute, second);
  return s;
  }

//////////////////////////////////////////////////////////////////////////////
/// Convert time to the unsigned int.
/// @return Time, in seconds passed since 00:00:00 1970.01.01.
Time::operator unsigned() const
  {
  struct tm this_time;

  this_time.tm_sec   = second;
  this_time.tm_min   = minute;
  this_time.tm_hour  = hour;
  this_time.tm_mday  = day;
  this_time.tm_mon   = month - 1;
  this_time.tm_year  = year - 1900;
  this_time.tm_isdst = -1; // Standart or DAYLIGHT time   '97.11.15 :Esg

  time_t t = mktime(&this_time);
  return ((unsigned)t);
  }

//////////////////////////////////////////////////////////////////////////////
/// Serialize the object.
///
/// Method serializes this Time object as a chunk. There is no default.
/// @param[in, out] inout - Serializer.
/// @param[in] tag - A chunk name.
void Time::Serialize(Serializer &inout, const char *tag)
  {
  inout.BegChunk(tag);
  Value(inout);
  inout.EndChunk();
  }

//////////////////////////////////////////////////////////////////////////////
/// Low level serialization.
/// @param[in, out] inout - A Serializer.
void Time::Value(Serializer &inout)
  {
  inout.Value(year);
  inout.Value(month);
  inout.Value(day);
  inout.Value(hour);
  inout.Value(minute);
  inout.Value(second);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set the specified time.
///
/// The method reinitializes the object by the specified time.
/// @param[in] t - Required time, in seconds passed since 00:00:00 1970.01.01.
void Time::Set(const unsigned t)
  {
  // Convert Time to tm Structure
  time_t tmp_time = (time_t)t;
  struct tm *ansi_result = localtime(&tmp_time);

  // Save Result till Next Call
  hour   = ansi_result->tm_hour;
  minute = ansi_result->tm_min;
  second = ansi_result->tm_sec;
  year   = ansi_result->tm_year + 1900;
  month  = ansi_result->tm_mon + 1;  // tm_mon 0:11
  day  = ansi_result->tm_mday;
  }


//////////////////////////////////////////////////////////////////////////////
/// Get processors ticks from switching on.
/// @param[out] num Processor ticks number from switching on.
/// @note It appeared that under Linux number of ticks is periodically
/// DECREASED in comparison with previous call of "rdtsc".
/// Because method is used for time measurement, under Linux
/// semantic of method is changed to return number of microseconds
/// passed since 1 Jan 1970.
void GetTicksNumber(INT64 *num)
  {
#if defined _WIN32 && !defined _WIN64
  _asm
    {
    push ebx
    mov ebx, num
    rdtsc
    mov dword ptr [ebx], eax
    mov dword ptr [ebx + 4], edx
    pop ebx
    }
#elif defined _WIN64
  *num = __rdtsc();
#elif defined __linux
#if 0
  int low, high;
  __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));
  *num = ((INT64)high << 32) + low;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  *num = ((INT64)tv.tv_sec * 1000000) + tv.tv_usec;
#endif
#else
#error Unknown platform.
#endif
  }  // GetTicksNumber()


//////////////////////////////////////////////////////////////////////////////
// Static functions

//////////////////////////////////////////////////////////////////////////////
/// Pause for specified time period.
/// @param[in] millisec Required length of pause, in milliseconds.
static void TimeDelay(int millisec)
  {
// ----------------- IRIS, LINUX --------------------
#if (OS_MARK == 'U'|| OS_MARK == 'L')

  struct timespec tm, oldtm;

  tm.tv_sec = millisec / 1000L;                // seconds
  tm.tv_nsec = (millisec % 1000L) * 1000000L;  // and nanoseconds
  nanosleep(&tm, &oldtm);

#endif

// ------------------- HP ------------------------
#if (OS_MARK == 'H')

  struct timeval delay;

  delay.tv_sec = millisec / 1000;               // seconds
  delay.tv_usec = (millisec % 1000) * 1000;     // microseconds
  select(0, (int *)0, (int *)0, (int *)0, &delay);

#endif

// --------------- Win_NT/Win_95 -------------------
#if (OS_MARK == 'I')

  // Use WINDOWS SDK Sleep()
  Sleep(millisec);

#endif

// ------------------ SUN, IBM -------------------------
#if (OS_MARK == 'N' || OS_MARK == 'R')

  //  Usage of "high resolution sleep" is not yet implemented
  long sec, msec;
  struct timeval start_time, end_time;
  struct timezone OS_tmp_timezone;

  // Receive Current Time
  gettimeofday(&start_time, &OS_tmp_timezone);

  // Calculate Time in Seconds to Sleep
  sec = millisec / 1000L;

  // Go to Sleep if Necessary
  if (sec > 0L)
    sleep(sec);

  // Loop of Delay
  do
    {
    gettimeofday(&end_time, &OS_tmp_timezone);
    msec = ((long)end_time.tv_sec - (long)start_time.tv_sec) * 1000L +
     ((long)end_time.tv_usec - (long)start_time.tv_usec) / 1000L;
    } while (msec < millisec);

#endif
  }  // TimeDelay()

//////////////////////////////////////////////////////////////////////////////
/// Equality operator.
/// @param[in] tc - Time to compare.
/// @return True if the Times are equal.
bool Time::operator == (const Time &tc) const
  {
  if (year != tc.year)
    return false;
  if (month != tc.month)
    return false;
  if (day != tc.day)
    return false;
  if (hour != tc.hour)
    return false;
  if (minute != tc.minute)
    return false;
  if (second != tc.second)
    return false;
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Inequality operator.
/// @param[in] tc - Time to compare.
/// @return True if the Times are not equal.
bool Time::operator != (const Time &tc) const
  {
  return !(*this == tc);
  }

INTEGRA_NAMESPACE_END
