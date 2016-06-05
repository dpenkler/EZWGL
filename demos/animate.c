/*******************************************************************************************
 *
 *   This file implements animated icon/label. Animation is achieved via a repeated
 *   timer which cycles through a set of provided pixmaps.  Given a widget,
 *
 *     setupAnimation   -- makes the widget ready for animation.
 *     startAnimation   -- initiates the animation.
 *     stopAnimation    -- terminates the animation.
 */

#include "EZ.h"

/*******************************************************************************************/
void setupAnimation   MY_ANSIARGS((EZ_Widget *widget, EZ_LabelPixmap **pixmaps, int n));
void startAnimation   MY_ANSIARGS((EZ_Widget *widget, long sec, long usec, int count));
void stopAnimation    MY_ANSIARGS((EZ_Widget *widget));
void setupAnimationF  MY_ANSIARGS((EZ_Widget *w, char *dir, char *base, int dgts, int start));
/********************************************************************************************
 *
 * Begin:  Animated Label/Icon
 */
#undef ANIMATION_MAGIC
#undef MAGIC  
#undef TIMER  
#undef NPXMPS  
#undef PXMPS  
#undef IDX

#define ANIMATION_MAGIC 0xaaedaec
#define MAGIC           0
#define TIMER           1
#define NPXMPS          2
#define PXMPS           3
#define IDX             4

static int isAnimationWidget(EZ_Widget *widget)
{
  if(widget)
    {
      EZ_UnknownDataType d;
      if(EZ_GetWidgetUnknownData(widget, MAGIC, &d) != 0)
	return(d.l == ANIMATION_MAGIC);
    }
  return(0);
}

/* Animation is done by a  repeating timer        */
static void animationTimerCallback(EZ_Timer *timer, void *data) 
{
  EZ_Widget *widget = (EZ_Widget *)data;
  if(widget && isAnimationWidget(widget))
    {
      EZ_UnknownDataType d;
      int                idx, npxmps, ti, cancel = 0;
      EZ_Timer           *ctimer;
      EZ_LabelPixmap     **pixmaps;

      ti = EZ_GetTimerIntData(timer);   /* number of cycles remaining */

      EZ_GetWidgetUnknownData(widget, NPXMPS, &d); /* total number of pmps */
      npxmps = d.i;

      EZ_GetWidgetUnknownData(widget, PXMPS, &d);  /* ptr to pmps */
      pixmaps = (EZ_LabelPixmap **)d.p;

      EZ_GetWidgetUnknownData(widget, IDX, &d);    /* current pmp index */
      idx = d.i;
      if(idx < 0 || idx >= npxmps) { ti--; idx = 0;} /* done one cycle */

      /* if startAnimation is called before 'timer' finishes, */
      /* cancel timer. */
      EZ_GetWidgetUnknownData(widget, TIMER, &d); /* the current timer */
      ctimer = (EZ_Timer *)d.p;
      if(timer != ctimer || ti <= 0) 
	{ idx = 0; cancel = 1;}

      if(EZ_WidgetIsViewable(widget))
	EZ_ConfigureWidget(widget, EZ_LABEL_PIXMAP, pixmaps[idx], NULL);
      idx++;
      d.i = idx;
      EZ_SetWidgetUnknownData(widget, IDX, d); /* increment idx */

      if(cancel) EZ_CancelTimer(timer);
      else EZ_SetTimerIntData(timer, ti);
    }
}
/*
 *  clean up procedure for animated widget.
 */
static void animationWidgetDestroyCallback(EZ_Widget *widget, void *data)
{
  if(widget && isAnimationWidget(widget))
    {
      EZ_UnknownDataType d;
      int                i, npmps = 0;
      EZ_Timer           *timer;
      EZ_LabelPixmap     **pmps;

      EZ_GetWidgetUnknownData(widget, TIMER, &d);
      EZ_CancelTimer((EZ_Timer *)d.p);

      EZ_GetWidgetUnknownData(widget, NPXMPS, &d);
      npmps = d.i;

      EZ_GetWidgetUnknownData(widget, PXMPS, &d);  
      pmps = (EZ_LabelPixmap **)d.p;

      for(i = 0; i < npmps; i++) EZ_FreeLabelPixmap(pmps[i]);
      (void)EZ_Free(pmps);
    }
}

/*
 * start animation
 */
void startAnimation(EZ_Widget *widget, long sec, long usec, int repeat)
{
  if(widget && isAnimationWidget(widget))
    {
      EZ_UnknownDataType d;
      d.p = (void *) EZ_CreateTimer(sec, usec, -1,
				    animationTimerCallback, widget, repeat);
      EZ_SetWidgetUnknownData(widget,TIMER, d);
    }
}

/*
 * stop animation
 */
void stopAnimation(EZ_Widget *widget)
{
  if(widget && isAnimationWidget(widget))
    {
      EZ_UnknownDataType d;
      d.p = (void *) NULL;
      EZ_SetWidgetUnknownData(widget,TIMER, d);
    }
}

/*
 * setup an animated label/icon.
 */
void setupAnimation(EZ_Widget *widget, EZ_LabelPixmap **pixmaps, int npixmaps)
{
  if(widget)
    {
      int                i;
      EZ_LabelPixmap     **pmps;
      EZ_UnknownDataType d;
      
      pmps = (EZ_LabelPixmap **)EZ_Malloc((npixmaps+1) * sizeof(EZ_LabelPixmap *));
      for(i = 0; i < npixmaps; i++) pmps[i] = pixmaps[i];

      EZ_ConfigureWidget(widget, EZ_LABEL_PIXMAP, pmps[0], NULL);

      d.i = 0;
      EZ_SetWidgetUnknownData(widget,IDX,d);    /* pixmap index        */
      
      d.l = ANIMATION_MAGIC;
      EZ_SetWidgetUnknownData(widget,MAGIC,d);  /* magic               */

      d.p = (void *)pmps;
      EZ_SetWidgetUnknownData(widget,PXMPS,d);  /* pointer to pixmaps */

      d.i = npixmaps;
      EZ_SetWidgetUnknownData(widget,NPXMPS,d);  /* number of pixmaps  */

      d.p = NULL;
      EZ_SetWidgetUnknownData(widget,TIMER,d);  /* timer              */


      EZ_AddWidgetCallBack(widget, EZ_DESTROY_CALLBACK, 
			   animationWidgetDestroyCallback, NULL, 0);
    }
}
/* convenience interface to setupAnimation */
void setupAnimationF(EZ_Widget *widget, char *dir, char *base, int digits, int start)
{
  if(widget)
    {
      char format[256], fname[256];
      EZ_LabelPixmap *pmps[100];
      int  i, end = start+100;
      sprintf(format, "%s/%s%%0%dd.xpm", dir, base, digits);
      for(i = start; i < end; i++)
	{
	  sprintf(fname, format, i);
	  pmps[i] = EZ_CreateLabelPixmapFromXpmFile(fname);
	  if(pmps[i] == NULL) break;
	}
      i -= start;
      if(i > 0) setupAnimation(widget, pmps, i);
    }
}
#undef ANIMATION_MAGIC
#undef MAGIC  
#undef TIMER  
#undef NPXMPS  
#undef PXMPS  
/*
 * End:  Animated Label/Icon
 *
 ********************************************************************************************/
/* TEST */
static void animate(EZ_Widget *widget, void *data)
{
  startAnimation(widget,0, 50000, 1);
}


main(int ac, char **av)
{
  EZ_Widget *frame, *btn;

  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                           EZ_LABEL_STRING,    "Test Animation",
                           0);
  btn    = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,    frame,
                           EZ_LABEL_STRING,    "A Button",
                           EZ_FOREGROUND,      "red",
                           EZ_CALLBACK,        animate, NULL,
                           0);  
  setupAnimationF(btn, "burning", "borrar-", 1, 0);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
