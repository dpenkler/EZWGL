
/*********************************************************************/
#include "EZ.h"

static float values[4];

void timer_callback(EZ_Timer *timer, void *data)
{
  /*
   * /proc/stat   kernel/system statistics
   *
   * cpu  3357 0 4313 1362393
   * The number of jiffies (1/100ths of a second)
   * that the system spent in user mode, user
   * mode with low priority (nice), system mode,
   * and the idle task, respectively.  The last
   * value should be 100 times the second entry
   * in the uptime pseudo-file.
   */
  static float cpustat[4];   /* remember the statistics read last time */
  FILE  *fp;
  char  str[32];   int i;
  float info[4], ftmp;

  if( (fp = fopen("/proc/stat", "r")) != NULL)
    {
      fscanf(fp, "%s %f %f %f %f", str, info, info+1, info+2, info+3);

      if(cpustat[0] != 0)
	{
	  float fields[4], total = 0.0;
	  for(i = 0; i < 4; i++)
	    { fields[i] = info[i] - cpustat[i]; total += fields[i]; }
	  if(total > 0)
	    {
	      ftmp = (fields[0] + fields[1] + fields[2])*100.0/total;
              values[0] = ftmp;
	    }
	}
      for(i=0; i < 4; i++) cpustat[i] = info[i];
      fclose(fp);
    }
  /*
   * /proc/loadavg
   * The load average numbers give the number of jobs in
   * the run queue averaged over 1, 5 and 15 minutes.
   * They are the same as the load average numbers given
   * by uptime(1) and other programs.
   */
  if( (fp = fopen("/proc/loadavg", "r")) != NULL)
    {
      static float oldv = 0.0;

      fscanf(fp, "%f", &ftmp);
      values[1] = ftmp * 100.0;
      fclose(fp);
    }
  /*
   * /proc/meminfo
   * This is used by free(1) to report the amount of
   * free and used memory (both physical and swap) on
   * the system as well as the shared memory and buffers
   * used by the kernel.
   */
  if( (fp = fopen("/proc/meminfo", "r")) != NULL)
    {
      static float stotal=0.0, suser=0.0, sshared=0.0, sbuffers=0.0;
      char junk[512];
      float total, used, freeM, shared, buffers, cached;
      float user, ftmp;

      fgets(junk, 500, fp);
      fscanf(fp, "%s %f %f %f %f %f %f", junk, &total, &used,
	     &freeM, &shared, &buffers, &cached);
      values[2] = 100.0*(total - freeM)/total;
      fclose(fp);
    }  
  EZ_SetHistogramValues(data, values, 3);
}
/*********************************************************************/
main(int ac, char **av)
{
  EZ_Widget *frame, *hgram;
  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                           EZ_FILL_MODE,       EZ_FILL_BOTH,
                           EZ_ORIENTATION,     EZ_VERTICAL,
                           EZ_PADX, 0,
                           EZ_PADY, 0,
                           0);

  hgram = EZ_CreateWidget(EZ_WIDGET_HISTOGRAM,    frame,
                          EZ_BORDER_WIDTH, 2,
                          EZ_BAR_COLOR_N, 0, "green",
                          EZ_BAR_COLOR_N, 1, "yellow",
                          EZ_BAR_ORIENTATION, EZ_HORIZONTAL,
                          EZ_IPADY, 2, EZ_IPADX, 2,
                          0);
  EZ_SetHistogramValues(hgram, values, 3);
  EZ_DisplayWidget(frame);
  EZ_CreateTimer(0, 100000, -1, timer_callback, hgram, 0);
  EZ_EventMainLoop();
}
/*********************************************************************/
