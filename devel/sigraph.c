/************************* Sigraph Widget ****************************/
#include "EZ.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

void ez_GetSigraphData (EZ_Widget *widget, int *numsamps, short **vals);
void ez_SetSigraphData (EZ_Widget *widget, int numsamps, short *vals);
void ez_GetSigraphSelection (EZ_Widget *widget, int *startSamp, int *endSamp);
void ez_SetSigraphSelection (EZ_Widget *widget, int  startSamp, int  endSamp);
/*********************************************************************/

#define Sigraph_TYPE    65555
static void SigraphSetup       (EZ_Widget *widget);
static void SigraphFreeData    (EZ_Widget *widget);
static void SigraphComputeSize (EZ_Widget *widget, int *w, int *h);
static void SigraphDraw        (EZ_Widget *widget);
static void SigraphEventHandle (EZ_Widget *widget, XEvent *event);
static void sliderCB     (EZ_Widget *, void *);
/*********************************************************************/

static void SigraphSetup(EZ_Widget *widget) {
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  unsigned long       mask  = 0L;  
  XGCValues           gcvalues;

#define NUMSAMPS 0
#define SAMPARRP 1
#define VSAMPBEG 2
#define VSAMPEND 3
#define SSAMPNUM 4
#define SSAMPVAL 5
#define ESAMPNUM 6
#define ESAMPVAL 7
#define GCONTEXT 9

  /* data is an array of 10 EZ_UnknownDataTypes */
  data[NUMSAMPS].i = 0;         /* no samples at first */
  data[SAMPARRP].p = 0;         /* really */
  data[VSAMPBEG].i = 0;         /* start sample number in view  */
  data[VSAMPEND].i = 100;       /* end sample number in view    */
  data[SSAMPNUM].i = 0;         /* selection start sample number*/
  data[SSAMPVAL].i = 0;         /* selection start sample val   */
  data[ESAMPNUM].i = 0;         /* selection end sample number  */
  data[ESAMPVAL].i = 0;         /* selection end sample val     */
  data[GCONTEXT].p = (void *)EZ_GetGC(mask, &gcvalues);
	printf("Width is %d\n",data[VSAMPEND].i);
}
/***************************************************************/

static void SigraphFreeData(EZ_Widget *widget) {
  /* this procedure is called when a Sigraph widget is
   * destroyed. We free the GC allocated for this widget 
   */
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  GC gc = (GC)data[GCONTEXT].p;
  /* for gc created by EZ_GetGC, you must use EZ_FreeGC to free it ! */
  EZ_FreeGC(gc); 
}
/***************************************************************/

static void SigraphComputeSize(EZ_Widget *widget, int *w, int *h) {
  /* this procedure is invoked by the geometry manager to
   * figure out the minimal size of Sigraph.
   */
  *w =1000; *h =100;  
}
/***************************************************************/

static void SigraphDraw(EZ_Widget *widget) {
  int focus_pad = 0;   /* kbd focus highlight padding */
  int border    = 0;   /* border style */
  int bwidth    = 0;   /* border width */
  int padx, pady;      /* XY padding, not used */
  int width, height;   /* widget window width and height */
  Display  *dpy;       /* the display */
  Window   win;        /* widget window */
  int      depth;      /* depth of visual */
  int      tile;       /* is bg tiled ? */
  int      x, y, tmp, tmp2;
  int      fv,lv;
  Pixmap   pixmap;
  GC       gc;
  unsigned long pv;
  double amp;
  short base;
  XPoint mypoints[2048]; /* max window width i wish */
  int i,npoints,lim;
  unsigned short *drawPtr,val;
  short *samp,tval;
  double inc,ind; 
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);

  EZ_GetWidgetPaddings(widget, &padx, &pady, &focus_pad); /* get all paddings       */
  EZ_GetWidgetBorder(widget, &bwidth, &border);           /* border characteristics */
  EZ_GetWidgetDimension(widget, &width, &height);         /* widget size            */
  dpy = EZ_GetDisplay();              /* display   */
  depth = EZ_GetDepth();             /*   visual    */
  win = EZ_GetWidgetWindow(widget); /* widget window */
  tmp = focus_pad + bwidth;        
  tmp2 = tmp + tmp;

  /* draw into a tmp pixmap, copy to the widget window when done */
  pixmap = XCreatePixmap(dpy, win, width, height, depth); 

  /* fill the background */
  EZ_GetBackgroundGC(widget, &gc, &pv,0,0); 
  XFillRectangle(dpy, pixmap, gc, 0, 0, width, height);

  /* fix kbd focus highlight border */
  EZ_FixFocusHighlightBorder(widget, pixmap, width, height, focus_pad);
  
  gc = (GC)data[GCONTEXT].p;
  pv = EZ_GetForegroundPV(widget);
  XSetForeground(dpy, gc, 0);	
  XFillRectangle(dpy,pixmap,gc,tmp,tmp,width-tmp2,height-tmp2);
  XSetForeground(dpy,gc,pv);
  if (data[NUMSAMPS].i > 0) {
  amp = (height-tmp2)/2;
  base = amp + tmp;
  npoints = width-tmp2;
  fv = data[VSAMPBEG].i;
  lv = data[VSAMPEND].i;
  samp = data[SAMPARRP].p;
  lim = lv - fv;
  inc = (double)lim / (double) npoints;
  ind = fv;
  for (i=0;i<npoints;i++) {
    mypoints[i].x = i+tmp;
    val = samp[fv];
    tval = ((val >> 8) | (val << 8));
    mypoints[i].y = base - (amp*((float)tval)/32780.0) ;
    ind += inc;
    fv = ind; 
  }
  
  XDrawLines(dpy, pixmap, gc, mypoints, npoints,CoordModeOrigin); 
  } 
  /* render the widget border */
  EZ_DrawRectBorder(widget, pixmap);

  /* and finally  ... */
  XCopyArea(dpy,pixmap,win, gc,0,0,width,height,0,0); 
  XFreePixmap(dpy, pixmap); 
}
/***************************************************************/

static void SigraphEventHandle(EZ_Widget *widget, XEvent *event) {
  if(event->type == Expose) SigraphDraw(widget);
  else if(event->type == ButtonPress || event->type == KeyPress) {
      int padx, pady, focus_pad=0;                /* paddings  */
      int border = 0, bwidth = 0, bwpad, bwpad2;  /* border .. */
      int x,y, width, height;                     /* dimension */
      Display *dpy = EZ_GetDisplay();             /* display   */
      XEvent xevent;
      EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);

      if(event->type == ButtonPress) {
          if(event->xbutton.button == EZ_Btn1) {
              /* get paddings and  dimension */
              EZ_GetWidgetPaddings(widget, &padx, &pady, &focus_pad);
              EZ_GetWidgetBorder(widget, &bwidth, &border);
              EZ_GetWidgetDimension(widget, &width, &height);
              bwpad = focus_pad + bwidth;
              bwpad2 = bwpad + bwpad;
              x = event->xbutton.x;
              y = event->xbutton.y;
              data[SSAMPNUM].i = (x-bwpad);
              data[SSAMPVAL].i = (float)(y-bwpad)/(float)(height - bwpad2);
              if(data[SSAMPNUM].i < 0) data[SSAMPNUM].i = 0; 
              else if(data[SSAMPNUM].i > data[NUMSAMPS].i) data[SSAMPNUM].i=data[NUMSAMPS].i;
              SigraphDraw(widget);
          } else if(event->xbutton.button == EZ_Btn3) {
              EZ_HandleDnDEvents(widget, event);
            }
        }
      else /* key press */ {
          char              tmpbuffer[8];
          int               buffersize = 8, xmove_unit, ymove_unit, count;
          KeySym            keysym;
          XComposeStatus    compose; 

          count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
          tmpbuffer[count] = '\0';
          xmove_unit = 0;
          ymove_unit = 0;

          switch(keysym)
            {
            case XK_Left: case XK_b:  case XK_h: case XK_B:  case XK_H:  
#ifdef XK_KP_Left
            case XK_KP_Left:
#endif
              if(event->xkey.state & ShiftMask)        xmove_unit = -4;
              else if(event->xkey.state & Mod1Mask)    xmove_unit = -10 ;
              else if(event->xkey.state & ControlMask) xmove_unit = -20;
              else xmove_unit = -1;
              break;
            case XK_Right: case XK_f: case XK_l: case XK_F: case XK_L:  
#ifdef XK_KP_Right
            case XK_KP_Right:
#endif
              if(event->xkey.state & ShiftMask)         xmove_unit = 4;
              else if(event->xkey.state & Mod1Mask)     xmove_unit = 10 ;
              else if(event->xkey.state & ControlMask)  xmove_unit = 20;
              else xmove_unit = 1;
              break;
            case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P: 
#ifdef XK_KP_Up
            case XK_KP_Up:
#endif
              if(event->xkey.state & ShiftMask)         ymove_unit = -4;
              else if(event->xkey.state & Mod1Mask)     ymove_unit = -10 ;
              else if(event->xkey.state & ControlMask)  ymove_unit = -20;
              else ymove_unit = -1;
              break;
              break;
            case XK_Down: case XK_n: case XK_j: case XK_N: case XK_J:  
#ifdef XK_KP_Down
            case XK_KP_Down:
#endif
              if(event->xkey.state & ShiftMask)         ymove_unit = 4;
              else if(event->xkey.state & Mod1Mask)     ymove_unit = 10 ;
              else if(event->xkey.state & ControlMask)  ymove_unit = 20;
              else ymove_unit = 1;
              break;
            default:
              break;
            }
              
          if(xmove_unit | ymove_unit)             {
/*                data[0].f += (xmove_unit) * 0.01; */
/*                data[1].f += (ymove_unit) * 0.01; */
/*                if(data[0].f < 0.0) data[0].f = 0.0; else if(data[0].f > 1.0) data[0].f=1.0; */
/*                if(data[1].f < 0.0) data[1].f = 0.0; else if(data[1].f > 1.0) data[1].f=1.0; */
              SigraphDraw(widget);
              EZ_CallWidgetMotionCallbacks(widget);
            }
          /* normally there are too many KeyPress events ... */
          while(XCheckTypedEvent(dpy, KeyPress, &xevent));
        }
    }
}
/**********************************************************************/

void ez_GetSigraphData(EZ_Widget *widget, int *numsamps, short **vals) {
  /* return the sample data associated with the graph */
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  *numsamps = data[NUMSAMPS].i;  *vals = data[SAMPARRP].p; 
}

void ez_SetSigraphData(EZ_Widget *widget,int numsamps, short *vals) {
  /* set the Sigraph sample buffer  */
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  data[NUMSAMPS].i = numsamps;
  data[SAMPARRP].p = vals;
  data[VSAMPEND].i = numsamps; /* debug */
  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);
  EZ_CallWidgetMotionCallbacks(widget);
}

void ez_GetSigraphView(EZ_Widget *widget, int *begsamp, int *endsamp) {
  /* set the Sigraph sample buffer  */
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  *begsamp = data[VSAMPBEG].i;
  *endsamp = data[VSAMPEND].i; 

}

void ez_SetSigraphView(EZ_Widget *widget, int begsamp, int endsamp) {
  /* set the Sigraph sample buffer  */
  EZ_UnknownDataType  *data = EZ_GetRawXWinPrivateData(widget);
  if ((begsamp > 0) && 
      (begsamp < data[NUMSAMPS].i)) data[VSAMPBEG].i = begsamp;
  if ((endsamp < data[NUMSAMPS].i) && 
      (endsamp > data[VSAMPBEG].i))  data[VSAMPEND].i = endsamp; 
  if(EZ_WidgetIsViewable(widget)) EZ_DrawWidget(widget);
  EZ_CallWidgetMotionCallbacks(widget);
}

/*********************************************************************
 *
 *  Testing .....
 *
 *********************************************************************/
/*#ifdef TEST_Sigraph*/

static void pEHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  int update = 0;
  switch(xev->type) {
    case ButtonPress:
      if(xev->xbutton.button == Button1) update = 1;
      break;
    case MotionNotify:
    case KeyPress:
      update = 1;
    default:
      break;
    }
  if(update) {
      float       x,y;
      char        str[128];
      EZ_Widget   *label = data;
  /*      ez_GetSigraphXY(widget, &x, &y); */
/*        ez_SetSigraphXY(copy, x,y); */
/*        sprintf(str, " %f %f ", x, y); */
/*        EZ_ConfigureWidget(label, EZ_LABEL_STRING, str, 0); */
    }
}

static void motionCb(EZ_Widget *widget, void *data) {
  float x,y;
/*    ez_GetSigraphXY(widget, &x, &y); */
/*    ez_SetSigraphXY(data, x, y); */
}

static void getout(EZ_Widget *widget, void *dat)
{ EZ_Shutdown();  exit(0); }

#ifdef DODND
static int encodeLoc(EZ_Widget *widget, void *data, char **message, int *length, int *needFree)
{
  static char str[64];
  float x, y;
  ez_GetSigraphXY(widget, &x, &y);
  sprintf(str, " %f %f ", x, y);
  *length = strlen(str);
  *message = str;
  *needFree = 0;
  return(EZ_DND_SUCCESS);  
}
static int setDragIconEncoder(EZ_Widget *widget, void *data, char **message, int *len, int *nFree)
{
  EZ_SetDragIcon((EZ_Widget *)data, 1);
  return(EZ_DND_SUCCESS);
}
int decodeLoc(EZ_Widget *widget, void *data,  char *message, int length)
{
  printf("[%s]\n", message);
  return(EZ_DND_SUCCESS);
}

#endif

fileInit(EZ_Widget *w1, EZ_Widget *w2, char *editFile) {
struct stat stat_buf;
static int editfd,i;
int len,nsamp;
short *marr1,*marr2,*lex1,*lex2;
short buf[32768];
if ((editfd = open (editFile, O_RDONLY, 0)) > 0) {
   fstat(editfd,&stat_buf);
   len = stat_buf.st_size;
   nsamp = len / 4;
   marr1 = malloc(len/2);
   marr2 = malloc(len/2);
   lex1 = marr1;
   lex2 = marr2;
   while (len = read(editfd,buf,65536)) {
	for (i=0;i<len/2;) {
	  *lex1++ = buf[i++];
          *lex2++ = buf[i++];
	}
   }
   close(editfd);
   ez_SetSigraphData(w1,nsamp,marr1);
   ez_SetSigraphData(w2,nsamp,marr2);
 } else {
    fprintf(stderr,"Edit File Open error on %s\n %s\n",
            editFile,strerror(errno));
    exit(1);
 }
}

static EZ_Widget *zoomW,  *panW;

static void zoomCB(EZ_Widget *slider, void *chans) {
  int nsamp,bsamp,esamp,tmp;
  short *vals; 
  EZ_Widget **w = chans;
  float f =  EZ_GetSliderValue(slider);
  ez_GetSigraphData(*w, &nsamp, &vals);
  ez_GetSigraphView(*w, &bsamp, &esamp);
  tmp = nsamp * f;
  if ((bsamp + tmp) > nsamp) {
    bsamp = nsamp - tmp;
    esamp = nsamp;
    EZ_SetSliderValue(panW,(float)bsamp/(float)nsamp);
  } else esamp = bsamp + tmp;
  while (*w) ez_SetSigraphView(*w++, bsamp, esamp);
}

static void panCB(EZ_Widget *slider, void *chans) {
  int nsamp,bsamp,esamp,tmp;
  short *vals;
  EZ_Widget **w = chans;
  float f =  EZ_GetSliderValue(slider);
  ez_GetSigraphData(*w, &nsamp, &vals);
  ez_GetSigraphView(*w, &bsamp, &esamp);
  tmp = esamp - bsamp;
  bsamp = f * nsamp;
  if ((bsamp + tmp) > nsamp) {
    esamp = nsamp;
    EZ_SetSliderValue(zoomW,(float)(nsamp-bsamp)/(float)nsamp);
  } else   esamp = bsamp + tmp;
  while (*w) {
  ez_SetSigraphView(*w++, bsamp, esamp);
  }
}

main(int ac, char **av) {
  EZ_Widget *frame, *label, *sigraph1, *sigraph2, *btn, *icon;
  EZ_Widget *chans[16];
  Atom TEST_ATOM;
  double zoom,pan;
  EZ_Initialize(ac,av,0);
  TEST_ATOM = EZ_GetAtom("TEST_ATOM");
  EZ_RegisterPrimitiveWidget(Sigraph_TYPE, "Sigraph", "Sigraph",
                             SigraphSetup, SigraphComputeSize,
                             SigraphDraw, SigraphFreeData, SigraphEventHandle);
  
  frame    = EZ_CreateWidget(EZ_WIDGET_FRAME,   NULL,
                             EZ_FILL_MODE,      EZ_FILL_BOTH,
                             EZ_LABEL_STRING,   "Sigraph", 
                             EZ_ORIENTATION,    EZ_VERTICAL,
                             NULL);
  sigraph1  = EZ_CreateWidgetXrm(Sigraph_TYPE,    frame,
                                "sigraph1",         "Sigraph1",
                                EZ_BORDER_TYPE,  EZ_BORDER_DOWN,
                                EZ_PADB,         2,
                                EZ_BORDER_WIDTH, 4,
                                EZ_CURSOR,       EZ_GetCursor(XC_crosshair),
				EZ_FOREGROUND,   "green",
                                0);
  sigraph2  = EZ_CreateWidgetXrm(Sigraph_TYPE,    frame,
                                "sigraph2",         "Sigraph2",
                                EZ_BORDER_TYPE,  EZ_BORDER_DOWN,
                                EZ_PADB,         2,
                                EZ_BORDER_WIDTH, 4,
                                EZ_CURSOR,       EZ_GetCursor(XC_crosshair),
				EZ_FOREGROUND,   "green",
                                0);
  chans[0] = sigraph1;
  chans[1] = sigraph2;
  chans[2] = 0;
  zoom = 1.0;
  zoomW = EZ_CreateSlider(frame,"zoom", 0.0, 1.0, 1.0, EZ_HORIZONTAL_SLIDER);
  EZ_AddWidgetCallBack(zoomW, EZ_CALLBACK, zoomCB, chans, 0);
  
  pan = 1.0;
  panW = EZ_CreateSlider(frame,"pan", 0.0, 1.0, 0.0, EZ_HORIZONTAL_SLIDER);
  EZ_AddWidgetCallBack(panW, EZ_CALLBACK, panCB, chans, 0);

  label =    EZ_CreateWidget(EZ_WIDGET_LABEL,  frame,
                             EZ_HEIGHT,        30,
                             EZ_LABEL_STRING,  "Location",
                             EZ_PROPAGATE,     False,
                             0);

  btn =      EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                             EZ_LABEL_STRING,         "Exit",
                             EZ_SIZE,                 0,0,
                             EZ_CALLBACK,             getout, NULL,
                             0);

  icon = EZ_CreateWidget(EZ_WIDGET_FREE_LABEL, NULL,
                         EZ_LABEL_STRING, "Drag Icon",
                         EZ_BACKGROUND,   "#00cccc",
                         EZ_FOREGROUND,   "red",
                         NULL);
  EZ_AddEventHandler(sigraph1,pEHandler, label, 0);

#ifdef DODND
  EZ_WidgetAddDnDDataEncoder(sigraph, TEST_ATOM, 0, encodeLoc, NULL, NULL, NULL);
  EZ_WidgetAddDnDDataEncoder(sigraph, EZ_DND_DRAG_INIT_ATOM, 0, 
                             setDragIconEncoder, icon, NULL, NULL);

  EZ_WidgetAddDnDDataDecoder(btn, TEST_ATOM, 0, decodeLoc, NULL, NULL, NULL);
#endif

  EZ_DisplayWidget(frame);
  printf("opening\n");
  fileInit(sigraph1,sigraph2,"test2.cdr");
  EZ_EventMainLoop();
}
/*#endif*/
/*********************************************************************/

