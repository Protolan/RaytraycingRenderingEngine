/// @file
///
/// @brief ThreadGroup class declarations.
///
/// @author Rkv - Kostya Vostryakov, KIHB, '10.02.14.
///
/// Copyright &copy; INTEGRA, Inc., 2010.

#ifndef _KIHB_THREAD_GROUP_HPP_
#define _KIHB_THREAD_GROUP_HPP_

#include <base/threads.hpp>
#include <base/arrays.hpp>
#include "export.h"

INTEGRA_NAMESPACE_START
class Thread1DRange;
class Thread2DRange;

#ifdef NO_EXCEPTION
#undef NO_EXCEPTION
#endif

#ifdef UNDEFINED_EXCEPTION
#undef UNDEFINED_EXCEPTION
#endif

#ifdef MEMORY_EXCEPTION
#undef MEMORY_EXCEPTION
#endif

/// Exceptions delivered through threads
struct ThreadGroupException
  {
  /// View controls of image
  enum ViewControls
    {
    /// No exception
    NO_EXCEPTION = 0x00000000,
    /// Undefined exception
    UNDEFINED_EXCEPTION = 0x00000001,
    /// Memory allocation exception
    MEMORY_EXCEPTION = 0x00000002
    };
  /// Default constructor
  ThreadGroupException();
  /// Base constructor
  ThreadGroupException(int ex, const char *sr = NULL);
  /// Cast to int type
  operator int () const;
  /// Operator =
  ThreadGroupException & operator = (int ex);
  /// Exception type
  int exception;
  /// Source of the error
  Str source;
  };

/// Thread group manager
class ThreadGroup
  {
  public:
    /// Pointer to function to create next job portion
    typedef bool (*NextFuncType) (void *shared_param, void **indiv_param,
      unsigned int thread_id);
    /// Pointer to function to execute job portion
    typedef void (*ExecFuncType) (void *shared_param, void *indiv_param, 
      unsigned int thread_id);

  public:

    /// Constructor                                                             
    ThreadGroup(int num, const char *name = NULL, int numa_node_id = -1);
    /// Destructor
    ~ThreadGroup();
    /// Start thread group anisochronously
    void Start(void *shared_params, void *next_shared_params, int used_tr_num = 0);
    /// Start thread group anisochronously
    void Start(void *shared_params, ExecFuncType exec, NextFuncType next = NULL,
                             int used_tr_num = 0);
    /// Start thread group anisochronously
    void Start(void *shared_params, void *next_shared_params, 
                             ExecFuncType exec, NextFuncType next = NULL, int used_tr_num = 0);
    /// Start thread group anisochronously
    void Start(void *shared_params, ExecFuncType exec, Thread1DRange *range,
                             int used_tr_num = 0);
    /// Start thread group anisochronously
    void Start(void *shared_params, ExecFuncType exec, Thread2DRange *range,
                             int used_tr_num = 0);
    /// Stop thread group
    void Stop();
    /// Check - is thread group stopped. 
    bool IsStopped() const;
    /// Wait while thread group finished all job for specified time
    int Gathering(DWORD dwMilliseconds);
    /// Wait while thread group finished all job
    int Gathering();
    /// Return exception source text
    const char *ExceptionSource() const;

  private:
    /// Create threads
    void Create();
    /// Terminate threads
    void Terminate();
    /// Pointer to shared parameters by all threads
    void *shared_params;
    /// Pointer to shared parameters to create next job portion
    void *next_shared_params;
    /// Pointer to function to create next job portion
    NextFuncType next; 
    /// Pointer to function to execute job portion
    ExecFuncType exec;

  private:

    /// Thread group name
    Str name;

    /// Number of threads in a group
    int threads_num;
    /// Numner of used threads in a group
    int used_threads_num;
    /// NUMA node id to lock threads to
    int numa_node_id;
    /// Thread handles
    TArray<HANDLE> threads;
    /// Thread start events
    TArray<HANDLE> start_event;
    /// Thread stop events
    TArray<HANDLE> stop_event;
    /// Thread stop events
    HANDLE bucket_cs;

    /// Thread parameters
    struct ThreadParams
      {
      /// Pointer to thread
      ThreadGroup *thread_grp;
      /// Thread index
      int thread_ind;
      /// Parameters for the thread
      void *indiv_param;
      /// Constructor
      ThreadParams() {}
      /// Constructor
      ThreadParams(ThreadGroup *tg, int ind);
      };

    /// Parameters for each thread in group
    TArray<ThreadParams> thread_params;
    /// Thread context array.
    TArray<ThreadContext> contexts;
    /// Are threads created
    bool threads_created;
    /// Are threads stopped
    bool stop_flag;
    /// Are threads terminated
    bool term_flag;
    /// Exception type
    ThreadGroupException exception;
    /// Mask of allowed exceptions
    int exception_mask;
    /// Source of allowed exceptions
    Str exception_source;
    /// Thread execution function
    static OKAY Execute(void *param);
  };

/// Thread group 1D domain subdivision
class Thread1DRange
  {
  public:
    /// Discrete subdomain range
    struct Range
      {
      /// Begin index
      int begin;
      /// Past-the-end element index
      int end;
      /// Number of elements in whole domain
      int res;
      /// Index of the block of threads
      int t_index;
      };
    /// Constructor
    Thread1DRange(int threads_num);
    /// Destructor
    ~Thread1DRange();
    /// Set domain range
    void Set(int begin, int end, int stp = 0, int used_tr_num = 0, int t_ind = 0);
    /// Get completed percentage
    double GetDone() const;
    /// Get next subdomain job
    bool GetNext(int thread_id);

    /// Domain subdivision
    TArray<Range*> thread_ranges;

  private:
    /// Number of threads in a group
    int threads_num;
    /// Numner of used threads in a group
    int used_threads_num;
    /// Begin domain index
    int g_begin;
    /// Past-the-end domain index
    int g_end;
    /// Subdivision step
    int step;
    /// Current subdomain index
    int cur;
    /// Number subdomains
    int num;
    /// Index of the block of threads
    int t_index;
  };

/// Thread group 2D domain subdivision
class Thread2DRange
  {
  public:
   
    /// Discrete subdomain range
    struct Range
      {
      /// X begin index
      int x_begin;
      /// X past-the-end element index
      int x_end;
      /// Y begin index
      int y_begin;
      /// Y past-the-end element index
      int y_end;
      /// Number of elements in whole domain in X
      int x_res;
      /// Number of elements in whole domain in Y
      int y_res;
      /// Index of the block of threads
      int t_index;
      };

    /// Constructor
    Thread2DRange(int threads_num);
    /// Destructor
    ~Thread2DRange();
    /// Set domain range
    void Set(int x_begin, int x_end, 
                           int y_begin, int y_end, int stepx = 0, int stepy = 0,
                           int used_tr_num = 0, int t_ind = 0);
    /// Get completed percentage
    double GetDone() const;
    /// Get next subdomain job
    bool GetNext(int thread_id);
    /// Domain subdivision
    TArray<Range*> thread_ranges;

  private:
    /// Number of threads in a group
    int threads_num;
    /// Numner of used threads in a group
    int used_threads_num;
    /// Begin domain index in X
    int g_x_begin;
    /// Past-the-end domain index in X
    int g_x_end;
    /// Begin domain index in Y
    int g_y_begin;
    /// Past-the-end domain index in Y
    int g_y_end;
    /// Subdivision step in X
    int x_step;
    /// Subdivision step in Y
    int y_step;
    /// Current subdomain index
    int cur;
    /// Number subdomains
    int num;
    /// Current subdomain index in X
    int x_cur;
    /// Current subdomain index in Y
    int y_cur;
    /// Number subdomains in X
    int x_num;
    /// Index of the block of threads
    int t_index;
  };


INTEGRA_NAMESPACE_END

#endif
