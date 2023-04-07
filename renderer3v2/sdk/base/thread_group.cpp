/// @file
///
/// @brief Methods of ThreadGroup class.
///
/// @author Rkv - Kostya Vostryakov, KIHB, '10.02.14.
///
/// Copyright &copy; INTEGRA, Inc., 2010.

#include <integra.hpp>

#include <base/marray.hpp>
#include <math/rnd.hpp>
#include <math/vect3.hpp>
#include <base/threads.hpp>

#include "thread_group.hpp"

INTEGRA_NAMESPACE_START

//////////////////////////////////////////////////////////////////////////////
/// Constructor
/// @param[in] tg Pointer to group
/// @param[in] ind Thread index
ThreadGroup::ThreadParams::ThreadParams(ThreadGroup *tg, int ind) 
: thread_grp(tg), thread_ind(ind) 
  {

  }

//////////////////////////////////////////////////////////////////////////////
/// Default constructor
ThreadGroupException::ThreadGroupException() : exception(NO_EXCEPTION)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Base constructor
/// @param[in] ex Exception type
/// @param[in] sr Exception source
ThreadGroupException::ThreadGroupException(int ex, const char *sr) : exception(ex), source(sr)
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Cast to int type
/// @return Exception type
ThreadGroupException::operator int () const
  {
  return exception;
  }

//////////////////////////////////////////////////////////////////////////////
/// Operator =
/// @param[in] ex Exception type
/// @return Exception type
ThreadGroupException & ThreadGroupException::operator = (int ex)
  {
  exception = ex;
  return *this;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor
/// @param[in] num Number of threads
/// @param[in] name Group name
/// @param[in] numa_node_id Index of NUMA node to create threads in
ThreadGroup::ThreadGroup(int num, const char *name, int numa_node_id)
  {
  if (name != NULL)
    this->name = name;
  threads_num = used_threads_num = num;
  this->numa_node_id = (numa_node_id >= 0 ? numa_node_id : -1);
  threads.Allocate(num);
  start_event.Allocate(num);
  stop_event.Allocate(num);
  thread_params.Allocate(num);

  threads_created = false;
  stop_flag = true;
  term_flag = true;
  exception = (int)ThreadGroupException::NO_EXCEPTION;
  exception_mask = (int)ThreadGroupException::NO_EXCEPTION;
  exception_source = NULL;
  IntInitializeCriticalSection(&bucket_cs);

  // the critical section preserve from early threads start
  // i.e. before the function ends.
  IntEnterCriticalSection(bucket_cs);		
  for (int i = 0; i < threads_num; ++i)
    {
    start_event[i] = IntCreateEvent(FALSE);
    stop_event[i] = IntCreateEvent(FALSE);
    }
  IntLeaveCriticalSection(bucket_cs);
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor
ThreadGroup::~ThreadGroup()
  {
  Terminate();

  for (int i = 0; i < threads_num; ++i)
    {
    IntCloseEvent(start_event[i]);
    IntCloseEvent(stop_event[i]);
    }

  IntDeleteCriticalSection(bucket_cs);
  }

//////////////////////////////////////////////////////////////////////////////
/// Create threads
void ThreadGroup::Create()
  {
  Assert(!threads_created);
  if (threads_created)
    return;

  //this->exec = exec;
  //this->next = next;

  threads_created = true;
  term_flag = false;
  exception = ThreadGroupException::NO_EXCEPTION;
  exception_mask = (int)ThreadGroupException::NO_EXCEPTION;
  exception_source = NULL;

  contexts.Allocate(threads_num);
  for (int i = 0; i < threads_num; ++i)
    {
    thread_params[i] = ThreadParams(this, i);
    contexts[i].data_ptr = &thread_params[i];
    contexts[i].RunThread = Execute;
    Str thr_name;
    thr_name.Printf("Group %s Thread %d", name.Data(), i);

    threads[i] = IntCreateThread(&contexts[i], thr_name, true, numa_node_id);
    }
  }

//////////////////////////////////////////////////////////////////////////////
/// Thread execution function
/// @return SUCCESS or FAILURE
OKAY ThreadGroup::Execute(void *param)
  {
  ThreadParams *thread_params = (ThreadParams *)param;
  ThreadGroup *thread_grp = thread_params->thread_grp;

  for( ; ; )
    {
    IntWaitForMultipleEvents(1, &thread_grp->start_event[thread_params->thread_ind],
                              TRUE, MAX_UINT);

    if (thread_grp->term_flag)
      {
      IntSetEvent(thread_grp->stop_event[thread_params->thread_ind]);
      break;
      }

    // simple processing
    if (thread_grp->next == NULL)
      {
      try
        {
        thread_grp->exec(thread_grp->shared_params, thread_grp->next_shared_params,
                         thread_params->thread_ind);
        }
      catch (ThreadGroupException ex)
        {
        IntEnterCriticalSection(thread_grp->bucket_cs);
        thread_grp->exception_mask |= (int)ex;
        thread_grp->exception_source = ex.source;
        IntLeaveCriticalSection(thread_grp->bucket_cs);
        }
      catch (...)
        {
        IntEnterCriticalSection(thread_grp->bucket_cs);
        thread_grp->exception_mask |= (int)ThreadGroupException::UNDEFINED_EXCEPTION;
        thread_grp->exception_source = NULL;
        IntLeaveCriticalSection(thread_grp->bucket_cs);
        }
      }
    // complex processing
    else
      {
      for( ; ; )
        {
        try
          {
          IntEnterCriticalSection(thread_grp->bucket_cs);

          if (!thread_grp->next(thread_grp->next_shared_params, 
                                &thread_params->indiv_param, thread_params->thread_ind) ||
              thread_grp->stop_flag || thread_grp->exception_mask != (int)ThreadGroupException::NO_EXCEPTION) 
            {
            IntLeaveCriticalSection(thread_grp->bucket_cs);

            break;
            }

          IntLeaveCriticalSection(thread_grp->bucket_cs);

          thread_grp->exec(thread_grp->shared_params, thread_params->indiv_param,
                           thread_params->thread_ind);
          }
        catch (ThreadGroupException ex)
          {
          IntEnterCriticalSection(thread_grp->bucket_cs);
          thread_grp->exception_mask |= (int)ex;
          thread_grp->exception_source = ex.source;
          IntLeaveCriticalSection(thread_grp->bucket_cs);
          break;
          }
        catch (...)
          {
          IntEnterCriticalSection(thread_grp->bucket_cs);
          thread_grp->exception_mask |= (int)ThreadGroupException::UNDEFINED_EXCEPTION;
          thread_grp->exception_source = NULL;
          IntLeaveCriticalSection(thread_grp->bucket_cs);
          break;
          }
        }
      }

    IntSetEvent(thread_grp->stop_event[thread_params->thread_ind]);
    }

  return SUCCESS;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start thread group anisochronously
/// @param[in] the_shared_params Shared parameters by all threads
/// @param[in] the_next_shared_params Parameters to create next job portion
/// @param[in] used_tr_num Number of threads used in the calculations
void ThreadGroup::Start(void *the_shared_params, void *the_next_shared_params,
                        int used_tr_num)
  {
  Assert(threads_created);
  Assert(stop_flag);

  if (!threads_created)
    return;

  shared_params = the_shared_params;
  next_shared_params = the_next_shared_params;
  stop_flag = false;
  exception = ThreadGroupException::NO_EXCEPTION;
  exception_mask = (int)ThreadGroupException::NO_EXCEPTION;
  exception_source = NULL;
  used_threads_num = used_tr_num <= 0 || used_tr_num >= threads_num ? threads_num : used_tr_num;

  for (int i = 0; i < used_threads_num; ++i)
    IntSetEvent(start_event[i]);
  }

//////////////////////////////////////////////////////////////////////////////
/// Start thread group anisochronously
/// @param[in] the_shared_params Shared parameters by all threads
/// @param[in] the_next_shared_params Parameters to create next job portion
/// @param[in] used_tr_num Number of threads used in the calculations
void ThreadGroup::Start(void *shared_params, ExecFuncType exec, 
                        NextFuncType next, int used_tr_num)
  {
  Start(shared_params, shared_params, exec, next, used_tr_num);
  }

//////////////////////////////////////////////////////////////////////////////
/// Start thread group anisochronously
/// @param[in] shared_params Shared parameters by all threads
/// @param[in] next_shared_params Parameters to create next job portion
/// @param[in] exec Function to execute job portion
/// @param[in] next Function to create next job portion
/// @param[in] used_tr_num Number of threads used in the calculations
void ThreadGroup::Start(void *shared_params, void *next_shared_params, 
                        ExecFuncType exec, NextFuncType next, int used_tr_num)
  {
  Assert(stop_flag);

  this->exec = exec;
  this->next = next;

  if (!threads_created)
    {
    Create();
    Start(shared_params, next_shared_params, used_tr_num);
    return;
    }

  Start(shared_params, next_shared_params, used_tr_num);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get next subdomain job
/// @param[in, out] shared_params Shared parameters by all threads
/// @param[in, out] indiv_param Parameters for the thread
/// @param[in] thread_id Thread index in group
bool Thread1DRangeNext(void *shared_param, void **indiv_param,
                       unsigned int thread_id)
  {
  Thread1DRange *thread1d_range = (Thread1DRange *)shared_param;

  if (!thread1d_range->GetNext(thread_id))
    return false;

  *indiv_param = thread1d_range->thread_ranges[thread_id];

  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get next subdomain job
/// @param[in, out] shared_params Shared parameters by all threads
/// @param[in, out] indiv_param Parameters for the thread
/// @param[in] thread_id Thread index in group
bool Thread2DRangeNext(void *shared_param, void **indiv_param,
                       unsigned int thread_id)
  {
  Thread2DRange *thread2d_range = (Thread2DRange *)shared_param;

  if (!thread2d_range->GetNext(thread_id))
    return false;

  *indiv_param = thread2d_range->thread_ranges[thread_id];

  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Start thread group anisochronously
/// @param[in] shared_params Shared parameters by all threads
/// @param[in] exec Function to execute job portion
/// @param[in] range Domain subdivision
/// @param[in] used_tr_num Number of threads used in the calculations
void ThreadGroup::Start(void *shared_params, ExecFuncType exec, 
                        Thread1DRange *range, int used_tr_num)
  {
  Start(shared_params, range, exec, Thread1DRangeNext, used_tr_num);
  }

//////////////////////////////////////////////////////////////////////////////
/// Start thread group anisochronously
/// @param[in] shared_params Shared parameters by all threads
/// @param[in] exec Function to execute job portion
/// @param[in] range Domain subdivision
/// @param[in] used_tr_num Number of threads used in the calculations
void ThreadGroup::Start(void *shared_params, ExecFuncType exec, 
                        Thread2DRange *range, int used_tr_num)
  {
  Start(shared_params, range, exec, Thread2DRangeNext, used_tr_num);
  }

//////////////////////////////////////////////////////////////////////////////
/// Stop thread group with stop-waiting 
void ThreadGroup::Stop()
  {
  if (stop_flag || !threads_created)
    return;

  IntEnterCriticalSection(bucket_cs);
  stop_flag = true;
  IntLeaveCriticalSection(bucket_cs);

  IntWaitForMultipleEvents(used_threads_num, &stop_event[0], TRUE, MAX_UINT);
  }

//////////////////////////////////////////////////////////////////////////////
/// Check - is thread group stopped. 
bool ThreadGroup::IsStopped() const
  {
  return (stop_flag || !threads_created);
  }

//////////////////////////////////////////////////////////////////////////////
/// Terminate threads
void ThreadGroup::Terminate()
  {
  if (!threads_created)
    return;
  Stop();
  term_flag = true;
  Start(NULL, NULL, NULL, NULL, 0);
  Stop();

  for (int i = 0; i < threads_num; ++i)
    IntCloseThread(threads[i]);

  threads_created = false;
  }

//////////////////////////////////////////////////////////////////////////////
/// Wait while thread group finished all job
/// @return 0 - SUCCESS, -1 - timeout, -2 - error, -3 - exception (memory error)
int ThreadGroup::Gathering(DWORD dwMilliseconds)
  {
  Assert(threads_created);
  Assert(!stop_flag);

  if (!threads_created)
    return -2;

  int ret = IntWaitForMultipleEvents(used_threads_num, &stop_event[0], TRUE, dwMilliseconds);

  if (ret == -1)
    return -1;

  stop_flag = true;
  if (ret == -2)
    return -2;

  if (exception_mask != ThreadGroupException::NO_EXCEPTION)
    {
    if ((exception_mask & (int)ThreadGroupException::UNDEFINED_EXCEPTION) == (int)ThreadGroupException::UNDEFINED_EXCEPTION)
      return -2;
    return -3;
    }

  return 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Wait while thread group finished all job
/// @return 0 - SUCCESS, -2 - error, -3 - exception (memory error)
int ThreadGroup::Gathering()
  {
  Assert(threads_created);
  Assert(!stop_flag);

  if (!threads_created)
    return -2;

  int ret = IntWaitForMultipleEvents(used_threads_num, &stop_event[0], TRUE, MAX_UINT);

  stop_flag = true;
  if (ret == -2 || ret == -1)
    return -2;

  if (exception_mask != ThreadGroupException::NO_EXCEPTION)
    {
    if ((exception_mask & (int)ThreadGroupException::UNDEFINED_EXCEPTION) == (int)ThreadGroupException::UNDEFINED_EXCEPTION)
      return -2;
    return -3;
    }
//    throw ThreadGroupException(exception);

  return 0;
  }

//////////////////////////////////////////////////////////////////////////////
/// Return exception source text
/// @return Exception source text
const char *ThreadGroup::ExceptionSource() const
  {
  return exception_source.Data();
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor
/// @param[in] threads_num Number of threads
Thread1DRange::Thread1DRange(int threads_num) 
  : threads_num(threads_num), used_threads_num(threads_num)
  {
  thread_ranges.Allocate(threads_num);
  for (int i = 0; i < threads_num; ++i)
    thread_ranges[i] = new Range;
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor
Thread1DRange::~Thread1DRange() 
  {
  for (int i = 0; i < threads_num; ++i)
    delete thread_ranges[i];
  }
    
//////////////////////////////////////////////////////////////////////////////
/// Set domain range
/// @param[in] begin Begin domain index
/// @param[in] end Past-the-end domain index
/// @param[in] stp Subdivision step
/// @param[in] used_tr_num Number of threads used in the calcultions
/// @param[in] t_ind Index of the block of threads
void Thread1DRange::Set(int begin, int end, int stp, int used_tr_num, int t_ind)
  {
  used_threads_num = used_tr_num <= 0 || used_tr_num >= threads_num ? threads_num : used_tr_num;
  g_begin = begin, g_end = end;
  t_index = t_ind;
  if (stp <= 0)
    {
    if (end - begin > 1024 * 1024)
      step = int((end - begin) / (used_threads_num * 1024.0));
    else if (end - begin > 1024 * 10)
      step = int((end - begin) / (used_threads_num * 4.0));
    else
      step = int((end - begin) / (used_threads_num * 1.0));
    step = Max(step, 1);
    }
  else 
    step = stp;
  num = int(ceil(double(end - begin) / step));
  Assert(num >= 0);
  cur = 0;

  for (int i = 0; i < used_threads_num; ++i)
    {
    thread_ranges[i]->res = g_end - g_begin;
    thread_ranges[i]->t_index = t_index;
    }
  }

//////////////////////////////////////////////////////////////////////////
/// Get completed percentage
/// @return Percent complete in (0, 1) range
double Thread1DRange::GetDone() const
  {
  return (double)(cur - used_threads_num <= 0 ? 0 : cur - used_threads_num) / num;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get next subdomain job
/// @param[in] thread_id Thread index
bool Thread1DRange::GetNext(int thread_id)
  {
  if (cur >= num)
    return false;

  Range* range = thread_ranges[thread_id];

  range->begin = g_begin + cur * step;
  range->end = Min(range->begin + step, g_end);

  cur++;
  return true;
  }

//////////////////////////////////////////////////////////////////////////////
/// Constructor
/// @param[in] threads_num Number of threads
Thread2DRange::Thread2DRange(int threads_num) 
    : threads_num(threads_num), used_threads_num(threads_num)
  {
  thread_ranges.Allocate(threads_num);
  for (int i = 0; i < threads_num; ++i)
    thread_ranges[i] = new Range;
  }
  
//////////////////////////////////////////////////////////////////////////////
/// Destructor
Thread2DRange::~Thread2DRange() 
  {
  for (int i = 0; i < threads_num; ++i)
    delete thread_ranges[i];
  }

//////////////////////////////////////////////////////////////////////////////
/// Set domain range
/// @param[in] x_begin Begin domain index in X
/// @param[in] x_end Past-the-end domain index in X
/// @param[in] y_begin, Begin domain index in Y
/// @param[in] y_end Past-the-end domain index in Y
/// @param[in] stepx Subdivision step for X (or common for X and Y if stepy == 0)
/// @param[in] stepy Subdivision step for Y
/// @param[in] used_tr_num Number of threads used in the calcultions
/// @param[in] t_ind Index of the block of threads
void Thread2DRange::Set(int x_begin, int x_end, 
                        int y_begin, int y_end, int stepx, int stepy, int used_tr_num, int t_ind)
  {
  used_threads_num = used_tr_num <= 0 || used_tr_num >= threads_num ? threads_num : used_tr_num;
  g_x_begin = x_begin, g_x_end = x_end;
  g_y_begin = y_begin, g_y_end = y_end;
  t_index= t_ind;

  int x_len = x_end - x_begin;
  int y_len = y_end - y_begin;

  if (stepy == 0)
    stepy = stepx;
  if (stepx <= 0)
    {
    if (x_len > 1024 * 1024)
      x_step = int(x_len / (used_threads_num * 1024.0));
    else if (x_len > 1024 * 10)
      x_step = int(x_len / (used_threads_num * 4.0));
    else
      x_step = int(x_len / (used_threads_num * 1.0));
    x_step = Max(x_step, 1);
    }
  else
    x_step = stepx;
  x_num = int(ceil(double(x_len) / x_step));

  if (stepy <= 0)
    {
    if (y_len > 1024 * 1024)
      y_step = int(y_len/ (used_threads_num * 1024.0));
    else if (y_len > 1024 * 10)
      y_step = int(y_len / (used_threads_num * 4.0));
    else
      y_step = int(y_len / (used_threads_num * 1.0));
    y_step = Max(y_step, 1);
    }
  else
    y_step = stepy;
  int y_num = int(ceil(double(y_len) / y_step));

  num = x_num * y_num;
  Assert(num > 0);
  cur = 0;
  x_cur = 0;
  y_cur = 0;

  for (int i = 0; i < used_threads_num; ++i)
    {
    thread_ranges[i]->x_res = x_len;
    thread_ranges[i]->y_res = y_len;
    thread_ranges[i]->t_index = t_index;
    }
  }

//////////////////////////////////////////////////////////////////////////
/// Get completed percentage
/// @return Percent complete in (0, 1) range
double Thread2DRange::GetDone() const
  {
  return (double)(cur - used_threads_num <= 0 ? 0 : cur - used_threads_num) / num;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get next subdomain job
/// @param[in] thread_id Thread index
bool Thread2DRange::GetNext(int thread_id)
  {
  if (cur >= num)
    return false;

  Range* range = thread_ranges[thread_id];

  range->x_begin = g_x_begin + x_cur * x_step;
  range->x_end = Min(range->x_begin + x_step, g_x_end);

  range->y_begin = g_y_begin + y_cur * y_step;
  range->y_end = Min(range->y_begin + y_step, g_y_end);

  if (x_cur >= x_num - 1)
    {
    x_cur = 0;
    y_cur++;
    }
  else
    x_cur++;

  cur++;
  return true;
  }


    
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static const int N = 12000;
static const int M = 1200;
static const double a = 1.1;
static const double b = 10;

static const double c = -1;
static const double d = 3;

double TestF(double x)
  {
  return Sin(x) + x;
  }

void ExecTestIntegration(void *shared_param, void *indiv_param, 
                         unsigned int thread_id)
{
  double *integ2_threads = (double *)shared_param;
  Thread1DRange::Range *range = (Thread1DRange::Range *)indiv_param;
   
  double integ1 = 0;
  for (int i = range->begin; i < range->end; i++)
    {
    integ1 += TestF((b - a) * (i / (N - 1.0)) + a);
    }
  integ2_threads[thread_id] += integ1;
}

double TestF(double x, double y)
  {
  return Sin(x + y) + x + y;
  }

void ExecTest2Integration(void *shared_param, void *indiv_param, 
                          unsigned int thread_id)
{
  double *integ2_threads = (double *)shared_param;
  Thread2DRange::Range *range = (Thread2DRange::Range *)indiv_param;
   
  double integ1 = 0;
  for (int i = range->x_begin; i < range->x_end; i++)
    {
    for (int j = range->y_begin; j < range->y_end; j++)
      {
      integ1 += TestF((b - a) * (i / (N - 1.0)) + a, 
        (d - c) * (j / (M - 1.0)) + c);
      }
    }
  integ2_threads[thread_id] += integ1;
}

INTEGRA_NAMESPACE_END

#if 0
bool test1ThreadGroup()
  {
  Ref<ThreadGroup> thread_group = new ThreadGroup(4, "test1ThreadGroup");
  Ref<Thread1DRange> thread1d_range = new Thread1DRange(4);

  double integ1 = 0;
  double integ2 = 0;

  for (int i = 0; i < N; i++)
    integ1 += TestF((b - a) * (i / (N - 1.0)) + a);
  integ1 *= (b - a) / N;

  thread1d_range->Set(0, N);
  double integ2_threads[4] = {0, 0, 0, 0};
  thread_group->Start(&integ2_threads, ExecTestIntegration, thread1d_range);
  thread_group->Gathering();
  integ2 = integ2_threads[0] + integ2_threads[1] + integ2_threads[2] + integ2_threads[3];

  integ2 *= (b - a) / N;

  return MathD::AboutEqual(integ1, integ2);
  }

bool test2ThreadGroup()
  {
  Ref<ThreadGroup> thread_group = new ThreadGroup(4, "test2ThreadGroup");
  Ref<Thread2DRange> thread2d_range = new Thread2DRange(4);

  double integ1 = 0;
  double integ2 = 0;

  for (int i = 0; i < N; i++)
    {
    for (int j = 0; j < M; j++)
      integ1 += TestF((b - a) * (i / (N - 1.0)) + a, 
      (d - c) * (j / (M - 1.0)) + c);
    }
  integ1 *= (b - a) * (d - c) / (N * M);

  thread2d_range->Set(0, N, 0, M);
  double integ2_threads[4] = {0, 0, 0, 0};
  thread_group->Start(&integ2_threads, ExecTest2Integration, thread2d_range);
  thread_group->Gathering();
  integ2 = integ2_threads[0] + integ2_threads[1] + integ2_threads[2] + integ2_threads[3];

  integ2 *= (b - a) * (d - c) / (N * M);

  return MathD::AboutEqual(integ1, integ2);
  }
#endif