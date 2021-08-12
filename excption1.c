#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <android/legacy_signal_inlines.h>

void almost_c99_signal_handler(int sig)
{
  switch(sig)
  {
    case SIGABRT:
    //   fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
      printf("Caught SIGABRT: usually caused by an abort() or assert(), error:%s\n", strerror(errno));
      break;
    case SIGFPE:
      printf("Caught SIGFPE: arithmetic exception, such as divide by zero, error:%s\n",
            strerror(errno));
      break;
    case SIGILL:
      printf("Caught SIGILL: illegal instruction, error:%s\n", strerror(errno));
      break;
    case SIGINT:
      printf("Caught SIGINT: interactive attention signal, probably a ctrl+c, error:%s\n",
            strerror(errno));
      break;
    case SIGSEGV:
      printf("Caught SIGSEGV: segfault, error:%s\n", strerror(errno));
      break;
    case SIGTERM:
    default:
      printf("Caught SIGTERM: a termination request was sent to the program, error:%s\n",
            strerror(errno));
      break;
  }
  exit(1);
}
 
void set_signal_handler()
{
  bsd_signal(SIGABRT, almost_c99_signal_handler);
  bsd_signal(SIGFPE,  almost_c99_signal_handler);
  bsd_signal(SIGILL,  almost_c99_signal_handler);
  bsd_signal(SIGINT,  almost_c99_signal_handler);
  bsd_signal(SIGSEGV, almost_c99_signal_handler);
  bsd_signal(SIGTERM, almost_c99_signal_handler);
}
 
int  divide_by_zero();
void cause_segfault();
void stack_overflow();
void infinite_loop();
void illegal_instruction();
void cause_calamity();
 
static char const * icky_global_program_name;
 
int main(int argc, char * argv[])
{
  (void)argc;
 
  /* store off program path so we can use it later */
  icky_global_program_name = argv[0];
 
  set_signal_handler();
 
  cause_calamity();
 
  puts("OMG! Nothing bad happend!");
  return 0;
}
 
void cause_calamity()
{
  /* uncomment one of the following error conditions to cause a calamity of 
   your choosing! */
 
//   (void)divide_by_zero();
  // cause_segfault();
  // assert(false);
  // infinite_loop();
  // illegal_instruction();
  stack_overflow();
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
 
void stack_overflow();
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