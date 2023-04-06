#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <math.h>       
#include <fenv.h>
#include <float.h>
#include <ucontext.h> 
#include <signal.h>
#include <setjmp.h>
#pragma STDC FENV_ACCESS on

#define _GNU_SOURCE /* Bring to GNU */

jmp_buf env;

void fp_exception_handler(int code, siginfo_t *info, void *arg) {
    
    unsigned int addr = (unsigned long)info->si_addr;
    
    
    // check SIGFPE information 
    switch (info->si_code) {
        
        case FPE_FLTDIV: // divide by zero
            signal(SIGFPE, SIG_DFL);
            break;
        case FPE_FLTOVF: //  overlfow
            fprintf ( stderr , "SIGFP is catched!\n");
            fprintf(stderr, "fp exception %x at address %x\n", code, addr);
            // disable error signal
            signal(SIGFPE, SIG_DFL);
            longjmp(env, 1);  
            break;
        case FPE_FLTUND: // ignore because of didn't feenableexcept
            fprintf ( stderr , "SIGFP is catched!\n");
            fprintf(stderr, "fp exception %x at address %x\n", code, addr);
            // disable error signal
            signal(SIGFPE, SIG_DFL);
            break;
        default:
            //printf("Unknown exception occurred.\n");
            break;
        
    }
    
    

}

int main() {
    // Enable floating-point exceptions
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW  );
    
    struct sigcontext *scp;
    
    struct sigaction action;
    action.sa_sigaction = fp_exception_handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGFPE);
    
    if (sigaction(SIGFPE, &action, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    
    double x;
    if (setjmp(env) == 0) { // setjmp turn 0
        
     
        /* cause an underflow exception (not reported) */ 
        double x = DBL_MIN; 
        
         printf("min_normal = %g \n", x); 
         x = x / 13.0; 
         printf("min_normal / 13.0 = %g \n", x);
      
        x = DBL_MAX;
        
      
      
      /* cause an overflow exception (reported) */ 
      
        printf("max_normal = %g \n", x); 
      
      
      
        x = x * x;
        printf("max_normal * max_normal = %g \n", x);
    }else{
        
        x = x = DBL_MAX;
        printf("max_normal * max_normal = %g \n", x);
    }    

    
    return 0;
}

