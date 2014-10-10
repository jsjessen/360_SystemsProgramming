/************ t.c file **************/

/* James Jessen            Group #1
 * 10918967                 Seq. #4
 *
 * CptS 360
 * Lab #2
 */

//#include < stdio.h>
#include "print.h"

int savedPC, savedFP, savedSP;  // globals

int save()
{
 /*  Write C code to save (this functions's) return PC to savedPC; */
 /*                  save caller's FP to savedFP; */

 /*  HINT: look at the diagram:                */
 /*  ------------------------------- */
 /*        |retPC|oldFP|        ========> save these values into [savedPC,savedFP] */
 /* ----------------|--------------- */
 /*              CPU.ebp */

  int *ebp = (int*)get_ebp();
  savedFP = *ebp;
  savedPC = *(ebp + 1);

  // An attempt to fix a,b,c values but doesn't work
  int *esp = (int*)get_esp();
  savedSP = *esp;

 /*  get CPU's ebp; print it in HEX; etc. */
 /*  print in HEX retPC old FP, etc. */

  myprintf("CPU.ebp = %x\n", *ebp);
  myprintf("oldFP   = %x\n", *ebp);
  myprintf("retPC   = %x\n", *(ebp + 1));
  myprintf("savedSP = %x\n", *esp);

  return 0;
}

int longjump(int v)
{
 /*  Write C code to replace this function's [retPC, oldFP] with [savedPC,savedFP] */

 /*  HINT: look at the diagram:                */
 /*  --------------------------------- */
 /*        |retPC|oldFP|    <========= Replace these values with [savePC,savedFP]  */
 /* ----------------|----------------- */
 /*              CPU.ebp */

  int *ebp = (int*)get_ebp();
  *ebp = savedFP;
  *(ebp + 1) = savedPC;

  // An attempt to fix a,b,c values but doesn't work
  int *esp = (int*)get_esp();
  *esp = savedSP;

 /*  get CPU's ebp register; print it in HEX */
 /*  print in HEX savedPC, savedFP, etc. */

  myprintf("CPU.ebp = %x\n", *ebp);
  myprintf("savedFP = %x\n", savedFP);
  myprintf("savedPC = %x\n", savedPC);
  myprintf("savedSP = %x\n", savedSP);

  return v;
}


int C()
{
  myprintf("enter C\n");
  myprintf("long jump? (y|n) ");

  if (getchar()=='y')
    longjump(12345);

  myprintf("exit C\n");
}

int B()
{
  myprintf("enter B\n");
  C();
  myprintf("exit B\n");
}

int A()
{
  myprintf("enter A\n");
  B();
  myprintf("exit A\n");
}

int main()
{
  int r, a,b,c;
  myprintf("enter main\n");
  a = 1; b = 2; c = 3;

  int *ebp = (int*)get_ebp();
  int *esp = (int*)get_esp();
  myprintf("ebp    = %x\n", *ebp);
  myprintf("retPC  = %x\n", *(ebp + 1));
  myprintf("SP     = %x\n", *esp);
  myprintf("ebp-8  = %x\n", (ebp - 8));
  myprintf("ebp-9  = %x\n", (ebp - 9));
  myprintf("ebp-10 = %x\n", (ebp - 10)); 
  myprintf("&a=%x &b=%x &c=%x\n", &a, &b, &c);

  myprintf("myprintf test: %u, %c, %d, %x, %s\n", 3, 'z', 42, 12, "Hello");

  r = save();
  if (r==0){
    A();
    myprintf("normal return\n");
  }
  else{
    myprintf("back to main via long jump r=%d\n", r);

    int *ebp = (int*)get_ebp();
    int *esp = (int*)get_esp();
    myprintf("ebp   = %x\n", *ebp);
    myprintf("retPC = %x\n", *(ebp + 1));
    myprintf("SP    = %x\n", *esp);

    myprintf("a=%d b=%d c=%d\n", a,b,c);
  }
  myprintf("exit main\n");
}
