/*********************************************************************/
#include "EZ.h"

struct meters_ {
  EZ_Widget *cpulabel,  *cpumeter;
  EZ_Widget *loadlabel, *loadmeter;
  EZ_Widget *memlabel,  *memmeter;
} Meters;

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
	  EZ_Widget *meter = Meters.cpumeter;
	  EZ_Widget *label = Meters.cpulabel;
	  float fields[4], total = 0.0;
	  for(i = 0; i < 4; i++)
	    { fields[i] = info[i] - cpustat[i]; total += fields[i]; }
	  if(total > 0)
	    {
	      EZ_StopStatusMeter(meter);
	      EZ_SetStatusMeterRange(meter,  0.0, total );
	      EZ_SetStatusMeterValueN(meter, 0, fields[0]);
	      EZ_SetStatusMeterValueN(meter, 1, fields[1]);
	      EZ_SetStatusMeterValueN(meter, 2, fields[2]);
	      EZ_StartStatusMeter(meter);
	      ftmp = (fields[0] + fields[1] + fields[2])*100.0/total;
	      sprintf(str, "CPU  %-3.0f%%", ftmp);
	      EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, NULL);
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
      EZ_Widget *meter = Meters.loadmeter;
      EZ_Widget *label = Meters.loadlabel;

      fscanf(fp, "%f", &ftmp);
      if(oldv != ftmp)
	{
	  EZ_SetStatusMeterValue(meter, ftmp);
	  sprintf(str, "Load %-1.2f", ftmp);
	  EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, NULL);
	  oldv = ftmp;	  
	}
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
      EZ_Widget *meter = Meters.memmeter;
      EZ_Widget *label = Meters.memlabel;
      char junk[512];
      float total, used, freeM, shared, buffers, cached;
      float user, ftmp;

      fgets(junk, 500, fp);
      fscanf(fp, "%s %f %f %f %f %f %f", junk, &total, &used,
	     &freeM, &shared, &buffers, &cached);
      user = total - freeM - shared - buffers;
      if(user < 0.0)
	{
	  user = 0.0;
	  buffers = total - freeM - shared;
	}
      if(stotal != total || suser != user || sshared != shared || sbuffers != buffers)
	{
	  EZ_StopStatusMeter(meter);
	  EZ_SetStatusMeterRange(meter, 0.0, total);
	  EZ_SetStatusMeterValueN(meter, 0, user);
	  EZ_SetStatusMeterValueN(meter, 1, shared);
	  EZ_SetStatusMeterValueN(meter, 2, buffers);
	  EZ_StartStatusMeter(meter);
	  stotal = total; suser = user; sshared = shared; sbuffers = buffers;
	  EZ_ConfigureWidget(label, EZ_LABEL_STRING, "Memory", NULL);
	}
      fclose(fp);
    }  
}
/*********************************************************************/
main(int ac, char **av)
{
  EZ_Widget *frame, *tmp;
  
  EZ_Initialize(ac,av,0);
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
			  EZ_FILL_MODE, EZ_FILL_BOTH,
			  EZ_IPADY, 4,
			                           /*idx  min weight pad */
			  EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 80,  0,  0,  
			  EZ_GRID_CONSTRAINS, EZ_COLUMN, 1, 160, 10, 0,  
			  NULL);
  /* CPU meter */
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
      			                       /* X, Y, W, H */
			EZ_GRID_CELL_GEOMETRY,    0, 0, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_LEFT, 
			EZ_LABEL_STRING, "CPU ",
			EZ_PADX, 0, EZ_PADY, 0,
			EZ_PROPAGATE, False,
			EZ_LABEL_POSITION, EZ_LEFT,
			NULL);
  Meters.cpulabel = tmp;
  tmp = EZ_CreateWidget(EZ_WIDGET_STATUS_METER, frame,
			EZ_BORDER_WIDTH, 1,
			EZ_SMETER_STYLE, 5, 2,	
			EZ_SMETER_BACKGROUND, "green",
			EZ_GRID_CELL_GEOMETRY,    1, 0, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_CENTER, 
			NULL);
  Meters.cpumeter = tmp;

  /* mem usage meter */
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
      			                       /* X, Y, W, H */
			EZ_GRID_CELL_GEOMETRY,    0, 1, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_LEFT, 
			EZ_LABEL_STRING, "Memory ",
			EZ_PADX, 0, EZ_PADY, 0,
			EZ_PROPAGATE, False,
			EZ_LABEL_POSITION, EZ_LEFT,
			NULL);
  Meters.memlabel = tmp;
  tmp = EZ_CreateWidget(EZ_WIDGET_STATUS_METER, frame,
			EZ_SMETER_STYLE, 0, 0,		
			EZ_SMETER_BACKGROUND, "green",
			EZ_BORDER_WIDTH, 1,
			EZ_GRID_CELL_GEOMETRY,    1, 1, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_CENTER, 
			NULL);
  Meters.memmeter = tmp;


  /* load meter */
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
      			                       /* X, Y, W, H */
			EZ_GRID_CELL_GEOMETRY,    0, 2, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_LEFT, 
			EZ_LABEL_STRING, "Load ",
			EZ_PADX, 0, EZ_PADY, 0,
			EZ_PROPAGATE, False,
			EZ_LABEL_POSITION, EZ_LEFT,
			NULL);
  Meters.loadlabel = tmp;
  tmp = EZ_CreateWidget(EZ_WIDGET_STATUS_METER, frame,
			EZ_BORDER_WIDTH, 1,
			EZ_SMETER_STYLE, 0, 0,		
			EZ_GRID_CELL_GEOMETRY,    1, 2, 1, 1, 
			EZ_GRID_CELL_PLACEMENT,   EZ_FILL_BOTH, EZ_CENTER, 
			EZ_SMETER_RANGE,    0.0, 4.0,
			EZ_SMETER_BACKGROUND, "green",
			NULL);
  Meters.loadmeter = tmp;

  EZ_CreateTimer(0, 500000, -1, timer_callback, NULL, 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*********************************************************************/
