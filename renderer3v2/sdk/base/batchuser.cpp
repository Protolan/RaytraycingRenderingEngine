/// @internal
/// @file
///
/// @brief Definition of BatchUser class.
///
/// @author Pnd - Nickolay Derjabin, '00.12.18
///
/// Copyright &copy; INTEGRA, Inc., 2000-2004

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <stdio.h>
#include <sys/select.h>

static int lin_getch(bool echo)
  {
  char ch;
  struct termios old_attr, new_attr;
  tcgetattr(0, &old_attr);
  new_attr = old_attr;
  new_attr.c_lflag &= ~ICANON;
  new_attr.c_lflag &= echo ? ECHO : ~ECHO;
  tcsetattr(0, TCSANOW, &new_attr);
  ch = getchar();
  tcsetattr(0, TCSANOW, &old_attr);
  return ch;
  }

static int _getch()
  {
  return lin_getch(false);
  }

static int _getche()
  {
  return lin_getch(true);
  }

int _kbhit(void)
  {
  struct timeval tv;
  fd_set read_fd;

  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_ZERO(&read_fd);
  FD_SET(0,&read_fd);

  if(select(1, &read_fd, NULL, NULL, &tv) == -1)
    return 0;

  if(FD_ISSET(0,&read_fd))
    return 1;

  return 0;
  }
#endif

#include <integra.hpp>

START_C_DECLS
#if 0
// There is a problem with this method implementation
// it requires too many libraries
#include "ikbl.h"
#endif
END_C_DECLS

#include "batchuser.hpp"

INTEGRA_NAMESPACE_START

/**
@class BatchUser base/batchuser.cpp

User object for a batch mode environment.

This class implements the IUser interface for batch mode applications.
All messages are printed to the standard output.

@sa @ref base_mainpage
**/


//////////////////////////////////////////////////////////////////////////////
/// Default constructor.
BatchUser::BatchUser()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
BatchUser::~BatchUser()
  {
  SwitchLogFileOff();
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue an error message to the user and wait for confirmation.
///
/// The method prints "ERROR:", the message and newline to the stdout.
/// @param[in] msg - A message to print.
void BatchUser::ErrorMsg(const CHAR *msg)
  {
  Assert(msg != NULL);
  if (ProgrCount() > 0)
    printf("\n");
  printf("ERROR: %s\n", (char *)msg);
  fflush(stdout);
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue a warning message to the user and wait for confirmation.
///
/// The method prints "WARNING:", the message and newline to the stdout.
/// @param[in] msg - A message to print.
void BatchUser::WarningMsg(const CHAR *msg)
  {
  Assert(msg != NULL);
  if (ProgrCount() > 0)
    printf("\n");
  printf("WARNING: %s\n", (char *)msg);
  fflush(stdout);
  }

//////////////////////////////////////////////////////////////////////////////
/// Issue a status information.
///
/// The method prints the message and newline to the stdout.
/// @param[in] msg - A message to print.
void BatchUser::StatusMsg(const CHAR *msg)
  {
  if (GetProgressQuietMode())
    return;
  Assert(msg != NULL);
  if (msg[0] == '\0')
    return;
  if (ProgrCount() > 0)
    printf("\n");
  printf("%s\n", (char *)msg);
  fflush(stdout);
  }  // StatusMsg()

//////////////////////////////////////////////////////////////////////////////
/// Issue a message to the stdout.
/// @param msg Message for the output.
void BatchUser::PrintMsg(const CHAR *msg)
  {
  printf("%s", msg);
  }

#if 0
//////////////////////////////////////////////////////////////////////////////
/// Issue a message that requires reply.
///
/// The method prints a message and a reply defined with the parameter.
/// Default reply means that all pressed keys treated as default reply
/// except the keys corresponds to another reply.
/// @param[in] msg - A message to print.
/// @param[in] rs - A reply style.
/// @param[in] def - Accept default reply.
/// @return true if positive reply was obtained.
bool BatchUser::Ask(const CHAR *msg, ReplyStyle rs, bool def)
  {
  Assert(msg != NULL);
  printf("%s\n", (char *)msg);
  int c;
  switch(rs)
    {
    case RS_OKCANCEL: // OK or CANCEL
      if (def)
        {
        // OK is default
        printf("OK(Any Key)/Cancel(C)\n");
        do
          {
          printf(">");
          c = getchar();
          }
        while (c == 10); // Pressed ENTER with empty line
        while (getchar() != 10) // Empty buffer
          {
          }
        if ((c == 'C') || (c == 'c'))
          return false;
        return true;
        }
      // CANCEL is default
      printf("OK(O)/Cancel(Any Key)\n");
      do
        {
        printf(">");
        c = getchar();
        }
      while (c == 10); // Pressed ENTER with empty line
      while (getchar() != 10) // Empty buffer
        {
        }
      if ((c == 'O') || (c == 'o'))
        return true;
      return false;

    case RS_YESNO: // YES or NO
      if (def)
        {
        // YES is default
        printf("Yes(Any Key)/No(N)\n");
        do
          {
          printf(">");
          c = getchar();
          }
        while (c == 10); // Pressed ENTER with empty line
        while (getchar() != 10) // Empty buffer
          {
          }
        if (c == 'N' || c == 'n') // Pressed 'N' or 'n'
          return false;
        return true;
        }
      // NO is default
      printf("Yes(Y)/No(Any Key)\n");
      do
        {
        printf(">");
        c = getchar();
        }
      while (c == 10); // Pressed ENTER with empty line
      while (getchar() != 10) // Empty buffer
        {
        }
      if ((c == 'Y') || (c == 'y')) // Pressed 'Y' or 'y'
        return true;
      return false;

    case RS_OK:
    default: // Just confirmation
      printf("OK(Any Key)\n>");
      while (getchar() != 10) // Wait for entered line and empty buffer
        {
        }
      return true;
    }
  }  // Ask()
#endif

//////////////////////////////////////////////////////////////////////////////
/// Issue a message that requires reply.
///
/// The method prints a message and a reply defined with the parameter.
/// Default reply means that all pressed keys treated as default reply
/// except the keys corresponds to another reply.
/// @param[in] msg - A message to print.
/// @param[in] rs - A reply style.
/// @param[in] def - Accept default reply.
/// @return true if positive reply was obtained.
bool BatchUser::Ask(const CHAR *msg, ReplyStyle rs, bool def)
  {
  Assert(msg != NULL);
  printf("%s\n", (char *)msg);
  int c;
  switch(rs)
    {
    case RS_OKCANCEL: // OK or CANCEL
      if (def)
        {
        // OK is default
        printf("OK(Any Key)/Cancel(C)\n");

        printf(">");
        c = _getche();
        printf("\n");

        if ((c == 'C') || (c == 'c'))
          return false;
        return true;
        }

      // CANCEL is default
      printf("OK(O)/Cancel(Any Key)\n");

      printf(">");
      c = _getche();
      printf("\n");

      if ((c == 'O') || (c == 'o'))
        return true;
      return false;

    case RS_YESNO: // YES or NO
      if (def)
        {
        // YES is default
        printf("Yes(Any Key)/No(N)\n");

        printf(">");
        c = _getche();
        printf("\n");

        if (c == 'N' || c == 'n') // Pressed 'N' or 'n'
          return false;
        return true;
        }
      // NO is default
      printf("Yes(Y)/No(Any Key)\n");

      printf(">");
      c = _getche();
      printf("\n");

      if ((c == 'Y') || (c == 'y')) // Pressed 'Y' or 'y'
        return true;
      return false;

    case RS_OK:
    default: // Just confirmation
      printf("OK(Any Key)\n>");
      _getche(); // Wait for entered key
      printf("\n");
      return true;
    }
  }  // Ask()

//////////////////////////////////////////////////////////////////////////////
/// Issue information message that requires reply, with 3 variants of answer.
///
/// @param msg - the message;
/// @param button0text - Text of the first button.
/// @param button1text - Text of the second button.
/// @param button2text - Text of the third button.
/// @param def_button - Default button number, when Return or Enter is pressed.
/// @param esc_button - Escape button number, when Esc is pressed.
/// @return  index of the pressed button
int BatchUser::Information(const CHAR *msg, const CHAR *button0text,
                 const CHAR *button1text, const CHAR *button2text,
                 int def_button, int esc_button)
  {
  Assert(msg != NULL);
  printf("%s\n", (char *)msg);

  if (button0text != NULL)
    printf("%s (0)\n", button0text);
  if (button1text != NULL)
    printf("%s (1)\n", button1text);
  if (button2text != NULL)
    printf("%s (2)\n", button2text);

  int c;
  do
    {
    printf(">");
    c = _getche();
    printf("\n");
    }
  while (!(def_button >= 0 && def_button <= 2 && c == 0x0d) &&
         !(esc_button >= 0 && esc_button <= 2 && c == 0x1b) &&
         !(button0text != NULL && c =='0') &&
         !(button1text != NULL && c =='1') &&
         !(button2text != NULL && c =='2'));

  if (def_button >= 0 && def_button <= 2 && c == 0x0d) // Enter
    c = '0' + def_button;
  if (esc_button >= 0 && esc_button <= 2 && c == 0x1b) // Esc
    c = '0' + esc_button;

  return  c - '0';
  }

//////////////////////////////////////////////////////////////////////////////
/// Show progress indication.
///
/// @param[in] done A value showing overall current progress.
///   For ordinary progress, the value is in percentages (0 <= done <= 100).
///   For busy mode, successive negative integral values are passed (-1, -2, etc.)
/// @param[in] msg A progress message to issue (not NULL).
///
void BatchUser::ShowProgr(double done, const CHAR *msg)
  {
  if (done >= 0.0)
    printf("%s %5.1f%% done\r", msg, done);
  else
    printf("%s\r", msg);
  fflush(stdout);
  }  // ShowProgr()

//////////////////////////////////////////////////////////////////////////////
/// Handle start of the progress indication.
///
/// @param[in] level Current progress level - 0 (the top most call), 1, 2, etc.
///
void BatchUser::InitProgr(int level)
  {
  }  // InitProgr()

//////////////////////////////////////////////////////////////////////////////
/// Handle end of the progress indication.
///
/// @param[in] level Current progress level - 0 (the top most call), 1, 2, etc.
///
void BatchUser::TermProgr(int level)
  {
  if (level != 0)
    return;
  printf("\n");
  fflush(stdout);
  }  // TermProgr()

//////////////////////////////////////////////////////////////////////////////
/// Check for user break.
/// @return True if break was requested.
bool BatchUser::Break()
  {
  while (_kbhit())
    {
    if (_getch() == 0x1b)
      {
      if (Ask(Tr("\nDo you want to break current procedure?"), RS_YESNO, false))
        SetBreak();
      }
    }

  return IUser::Break();
  }

INTEGRA_NAMESPACE_END

