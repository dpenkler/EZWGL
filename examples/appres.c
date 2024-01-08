/*********************************************************************
 *
 *  Demonstrate  EZ_RegisterApplicationResources
 */
#include "EZ.h"

static  int   int1 = 0,   int2 = 0; /* var set by resources */
static  float float2 = 0;

static  EZ_ResourceSpec exampleResources[] =
{
   { 1234, "aPairOfInt",  "APairOfInt", "int,int"   },
   { 1235, "intFloat",    "IntFloat",   "int,float" },
};


int configure(void *widget, int option, EZ_Value *values)
{
  switch(option)
   {
     case 1234:  int1 = values[0].value.i; int2 = values[1].value.i; break;
     case 1235:  int1 = values[0].value.i; float2 = values[1].value.f; break;
     default:  break;
   }
}

int main(int ac, char **av)
{
  EZ_Initialize(ac, av, 1);
  printf("Run with one of the following options\n\t %s -xrm \"*aPairOfInt: int1 int2\" -xrm \"*intFloat: int float\"\n\n", av[0]);
  printf("Initial values are: int1=%d, int2=%d, float2=%f\n\n", int1, int2, float2);         
  EZ_RegisterApplicationResources(2,exampleResources, configure);  
  printf("After using resources: int1=%d, int2=%d, float2=%f\n", int1, int2, float2);         
  /* more stuff ... */
}
