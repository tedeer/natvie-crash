#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <android/legacy_signal_inlines.h>
// 只捕获会造成进程终止的几种异常
const int SIGNALS_LEN = 7;
const int signal_array[] = {SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSEGV, SIGSTKFLT, SIGSYS};
// 储存系统默认的异常处理
struct sigaction old_signal_handlers[7];

void signal_handler(int signal, siginfo_t *info, void *context)
{
    // 自己做一些处理工作
    const int code = info->si_code;
    printf("handler signal %d, code: %d, pid: %d, uid: %d, tid: %d\n",
          signal,
          code,
          info->si_pid,
          info->si_uid,
          info->si_tid
    );

    // 找到异常对应的默认处理函数
    int index = -1,i = 0;
    for (i = 0; i < SIGNALS_LEN; ++i) {
        if (signal_array[i] == signal) {
            index = i;
            break;
        }
    }
    if(index == -1) {
        printf("Not found match handler\n");
        exit(code);
    }
    struct sigaction old = old_signal_handlers[index];
    // 调用默认的异常处理函数
    old.sa_sigaction(signal, info, context);
}

void init()
{
    struct sigaction handler = {0};
    memset(&handler, 0, sizeof(struct sigaction));

    handler.sa_sigaction = signal_handler;
    //SA_ONSTACK is spare stack,when current stack is overflow,prog could use spare stack
    handler.sa_flags = SA_SIGINFO | SA_ONSTACK;
    int i = 0;
    for (i = 0; i < SIGNALS_LEN; ++i) {
        sigaction(signal_array[i], &handler, &old_signal_handlers[i]);
    }
}

int divide_by_zero()
{
  int a = 1;
  int b = 0; 
  return a / b;
}
 
void cause_segfault()
{
  int * p = (int*)0x12345678;
  *p = 0;
}
 
void stack_overflow()
{
  int foo[1000]; //allocate something big on the stack
  (void)foo;
  stack_overflow();
}
 
/* break out with ctrl+c to test SIGINT handling */
void infinite_loop()
{
  while(1) {};
}
 
void illegal_instruction()
{
  /* I couldn't find an easy way to cause this one, so I'm cheating */
  raise(SIGILL);
}

static void* test_thread_func(void *arg)
{
    stack_overflow();
    return NULL;
}
   
void thread_err()
{
    pthread_t test_thread_t;
    pthread_create(&test_thread_t, NULL, test_thread_func, NULL);
    pthread_detach(test_thread_t);
}

void cause_calamity()
{
  /* uncomment one of the following error conditions to cause a calamity of 
   your choosing! */
 
//   (void)divide_by_zero();
//   cause_segfault();
//   assert(false);
//   infinite_loop();
//   illegal_instruction();
//   stack_overflow();
  thread_err();
}

int main(int argc, char * argv[])
{
  init();
 
  cause_calamity();
 
  puts("OMG! Nothing bad happend!");
  while(1){
      sleep(1);
  }
  return 0;
}