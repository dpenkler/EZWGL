#include "EZ.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

void EZ_SSheetWriteFile(EZ_Widget *widget, char *fname)
{
  FILE *fp = fopen(fname, "w");
  if(fp==NULL) { fprintf(stderr, "Cannot open %s\n", fname);}
  else
    {
      int nrows, ncols=0, bytes; 
      char *str;
      EZ_SSheetGetSizes(widget, &nrows, &ncols);
      str= EZ_SSheetGetRegionString(widget,0,0, nrows,ncols);
      if(str) { bytes = strlen(str); fwrite(str, 1, bytes, fp); (void)free(str);}
      fclose(fp);
    }
}

void EZ_SSheetReadFile(EZ_Widget *widget, char *fname)
{
  FILE *fp = fopen(fname, "r");
  if(fp ==NULL) { fprintf(stderr, "Cannot open %s\n", fname);}
  else
    {
      char *s;   int bytes;
      (void)fseek(fp, 0L, SEEK_END);
      bytes = ftell(fp);
      (void)fseek(fp, 0L, SEEK_SET);
      s= (char *)malloc(bytes+10);
      if(s)
        {
          fread(s, 1,  bytes, fp); s[bytes] = '\0';
          fclose(fp);
          EZ_SSheetClearRegion(widget, 0, 0, 32767, 32767);
          EZ_SSheetInsertRegionString(widget, 0, 0, s);
          free(s);
        }
      else { fprintf(stderr, "Out of memory\n");}
    }
}


static EZ_Widget *statusLabel,  *ssheet;

static void showStatus(char *str)
{ EZ_ConfigureWidget(statusLabel, EZ_LABEL_STRING, str, NULL);}
static void makeChart(EZ_Widget *w, void *d);
static void ssort(EZ_Widget *w, void *d);

/* registered to spread sheet, disply the content of the active cell 
 * the next two callbacks form a infinite loop !!
 */
static mutex=0;
static void motionCB(EZ_Widget *w, void *d)
{
  char *str=NULL; int row, col;
  if(mutex ==0)
    {
      mutex = 1;
      EZ_SSheetGetActiveCell(w, &row, &col, &str);
      EZ_SetEntryString((EZ_Widget *)d, str);
      mutex = 0;
    }
}
/* registered to entry, search for cell with ... */
static void emotionCB(EZ_Widget *w, void *d)
{
  char *str= EZ_GetEntryString(w);
  int row, col;
  if(mutex == 0)
    {
      mutex = 1;
      if(EZ_SSheetFindCell(ssheet, str, &row, &col))
        {
          int orow, ocol;
          EZ_SSheetGetActiveCell(w, &orow, &ocol, NULL);
          if(orow != row || ocol != col)
            EZ_SSheetSetActiveCell(ssheet,row,col);
        }
      mutex = 0;
    }
}


static void destroyCB(EZ_Widget *w, void *d)
{
  EZ_SSheetWriteFile(w, "a");
}

static dojust(EZ_Widget *w, void *d)
{
  int r1=-1,c1,r2,c2;
  EZ_SSheetGetHighlightedRegion(ssheet,  &r1, &c1, &r2, &c2);
  if(r1 < 0)
    { EZ_SSheetGetActiveCell(ssheet, &r1, &c1, NULL); r2=r1; c2=c1;}
  if(r1 >= 0)
    {
      int just = (int) d;
      EZ_SSheetSetRegionJustification(ssheet, r1,c1,r2,c2,just);
      showStatus(NULL);
    }
  else showStatus("Please selection a region first");
}

static dohiderow(EZ_Widget *w, void *d)
{
  static int hidden=0, start, end;
  if(hidden)
    {
      EZ_SSheetUnhideRows(ssheet, start,end);
      EZ_ConfigureWidget(w, EZ_LABEL_STRING, "hide row", 0);
      hidden =0;
    }
  else
    {
      int r1=-1,c1,r2,c2;
      EZ_SSheetGetHighlightedRegion(ssheet,  &r1, &c1, &r2, &c2);
      if(r1 >= 0)
        {
          EZ_SSheetHideRows(ssheet, r1,r2);
          EZ_ConfigureWidget(w, EZ_LABEL_STRING, "unhide row", 0);
          hidden =1;
          start =r1; end=r2;
          showStatus(NULL);
        }
      else showStatus("Please selection a region first");
    }
}

static dohidecol(EZ_Widget *w, void *d)
{
  static int hidden=0, start, end;
  if(hidden)
    {
      EZ_SSheetUnhideColumns(ssheet, start,end);
      EZ_ConfigureWidget(w, EZ_LABEL_STRING, "hide column", 0);
      hidden =0;
    }
  else
    {
      int r1=-1,c1,r2,c2;
      EZ_SSheetGetHighlightedRegion(ssheet,  &r1, &c1, &r2, &c2);
      if(r1 >= 0)
        {
          EZ_SSheetHideColumns(ssheet, c1,c2);
          EZ_ConfigureWidget(w, EZ_LABEL_STRING, "unhide column", 0);
          hidden =1;
          start =c1; end=c2;
          showStatus(NULL);
        }
      else showStatus("Please selection a region first");
    }
}


static dofont(EZ_Widget *w, void *d)
{
  /* really should remember the current text property */
  EZ_TextProp *prop = NULL;
  char *fname;
  int id = (int) d;
  switch(id) {
  case 1: fname = "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*";     
    prop = EZ_GetTextProperty(EZ_FONT_NAME, fname, EZ_FOREGROUND, "BLUE", 0);
    break;
  case 2: fname = "-Adobe-Times-Bold-I-Normal--*-120-*-*-*-*-*-*"; 
    prop = EZ_GetTextProperty(EZ_FONT_NAME, fname, 0);
    break;
  case 3:
    prop = EZ_GetTextProperty(EZ_UNDERLINE, 1, 0);
    break; 
  default: 
    break;
  }
  {
    int r1=-1,c1,r2,c2;
    EZ_SSheetGetHighlightedRegion(ssheet,  &r1, &c1, &r2, &c2);
    if(r1 >= 0)
      {
        EZ_SSheetSetRegionProperty(ssheet, r1,c1,r2,c2, prop);
      }
    else
      { 
        EZ_SSheetGetActiveCell(ssheet, &r1, &c1, NULL); r2=r1; c2=c1;
        EZ_SSheetSetRegionProperty(ssheet, r1,c1,r2,c2, prop);
        EZ_SSheetSetTextProperty(ssheet, prop);
      }
  }
}



main(int ac, char **av)
{
  EZ_Widget  *frame, *frame1, *entry, *btn, *hscr, *vscr, *ttt;
  
  EZ_Initialize(ac, av, 0);
  
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                          EZ_GRID_CONSTRAINS, EZ_ROW,    0,  0,   0, 0,
                          EZ_GRID_CONSTRAINS, EZ_ROW,    1, 100,  100, 0,
                          EZ_GRID_CONSTRAINS, EZ_ROW,    2, 0,   0, 0,
                          EZ_GRID_CONSTRAINS, EZ_ROW,    3, 0,   0, 0,
                          EZ_GRID_CONSTRAINS, EZ_COLUMN, 0, 100, 100, 0,
                          EZ_GRID_CONSTRAINS, EZ_COLUMN, 1, 0, 0, 0,
                          EZ_FILL_MODE, EZ_FILL_BOTH,
                          EZ_SIZE, 800, 500,
                          0);

  frame1 = EZ_CreateWidget(EZ_WIDGET_FRAME, frame,
                           EZ_ORIENTATION,            EZ_HORIZONTAL_LEFT,
                           EZ_GRID_CELL_GEOMETRY,     0, 0, 2, 1,
                           EZ_GRID_CELL_PLACEMENT,    EZ_FILL_BOTH, EZ_CENTER, 
                           0);
  entry  = EZ_CreateWidgetXrm(EZ_WIDGET_ENTRY,           frame1,
			      "entry",                   NULL,
                              EZ_TEXT_BACKGROUND,        "white",
                              EZ_TEXT_LINE_LENGTH,      20,
                              EZ_MOTION_CALLBACK,       emotionCB, NULL,
			      0);

  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "chart",                    NULL,
                              EZ_LABEL_STRING,           "Chart",
                              EZ_CALLBACK,                makeChart, NULL,
                              0);

  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "sss",                    NULL,
                              EZ_LABEL_STRING,           "Sort",
                              EZ_CALLBACK,                ssort, NULL,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "Ljust",                    NULL,
                              EZ_LABEL_STRING,           "Just L",
                              EZ_CALLBACK,                dojust, (void *)EZ_LEFT,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "Rjust",                    NULL,
                              EZ_LABEL_STRING,           "Just R",
                              EZ_CALLBACK,                dojust, (void *)EZ_RIGHT,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "Cjust",                    NULL,
                              EZ_LABEL_STRING,           "Just C",
                              EZ_CALLBACK,                dojust, (void *)EZ_CENTER,
                              0);

  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "B",                    NULL,
                              EZ_LABEL_STRING,           "B",
                              EZ_CALLBACK,                dofont, (void *)0x1,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "B",                    NULL,
                              EZ_LABEL_STRING,           "I",
                              EZ_CALLBACK,                dofont, (void *)0x2,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "B",                    NULL,
                              EZ_LABEL_STRING,           "U",
                              EZ_UNDERLINE,               0,
                              EZ_CALLBACK,                dofont, (void *)0x3,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "h",                    NULL,
                              EZ_LABEL_STRING,           "Hide Rows",
                              EZ_UNDERLINE,               0,
                              EZ_CALLBACK,                dohiderow, NULL,
                              0);
  ttt  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame1,
                              "h",                    NULL,
                              EZ_LABEL_STRING,           "Hide Cols",
                              EZ_UNDERLINE,               0,
                              EZ_CALLBACK,                dohidecol, NULL,
                              0);


  ssheet  = EZ_CreateWidgetXrm(EZ_WIDGET_SPREAD_SHEET,    frame,
			       "spreadSheet",             NULL,
			       EZ_TEXT_BACKGROUND,        "white",
                               EZ_GRID_CELL_GEOMETRY,     0, 1, 1, 1,
                               EZ_GRID_CELL_PLACEMENT,    EZ_FILL_BOTH, EZ_CENTER, 
			       EZ_MOTION_CALLBACK,        motionCB, entry,
                               EZ_DESTROY_CALLBACK,       destroyCB, NULL,
			       0);
  statusLabel = EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
                                EZ_ORIENTATION,            EZ_HORIZONTAL_LEFT,
                                EZ_GRID_CELL_GEOMETRY,     0, 3, 2, 1,
                                EZ_GRID_CELL_PLACEMENT,    EZ_FILL_BOTH, EZ_CENTER, 
                                EZ_LABEL_POSITION,         EZ_LEFT,
                                EZ_TEXT_LINE_LENGTH,       80,
                                EZ_FONT_NAME,              "r14",
                                EZ_LABEL_STRING,           "Status: ",
                                EZ_HEIGHT, 16,
                                EZ_PADY, 0,
                                EZ_PROPAGATE, 0,
                                0);

  hscr = EZ_CreateWidget(EZ_WIDGET_HORIZONTAL_SCROLLBAR,  frame,
                         EZ_EXPAND,                       1,
                         EZ_GRID_CELL_GEOMETRY,           0, 2, 1, 1,
                         0);
  vscr = EZ_CreateWidget(EZ_WIDGET_VERTICAL_SCROLLBAR,    frame,
                         EZ_EXPAND,                       1,
                         EZ_GRID_CELL_GEOMETRY,           1, 1, 1, 1,
                         0);
  EZ_SSheetSetHScrollbar(ssheet, hscr);
  EZ_SSheetSetVScrollbar(ssheet, vscr);
  EZ_SSheetSetTabChar(ssheet, ' ', '\n');
  EZ_SSheetReadFile(ssheet, "a");
  EZ_DisplayWidget(ssheet);
  EZ_EventMainLoop();
}

/************************************************************
 *
 * a simple  bar chart
 */
static  GC redgc, greengc;

#define BAR_CHART   1
#define PIE_CHART   2

typedef struct {
  int         kind;
  int         total;
  int         r1, c1, r2, c2; /* region */
  int         xsize, ysize;
  float       maxv, minv;
  EZ_FigPiece **pieces;
  float       *values;
} barChart, pieChart;


void chartHandle(EZ_Widget *sheet, EZ_Item *chart, 
                 int r1, int c1, int r2, int c2,  /* region to chart */
                 int kind, void *data)
{
  barChart    *barc = (barChart *)data;
  int         i, j, k, nbars, allbad=0;
  float       maxv = -10000.0, minv= 0.0;
  float       *fptr, *ftmp, xfactor, yfactor;
  EZ_FigPiece **pieces;

  nbars = (r2-r1+1)*(c2-c1+1);

  ftmp = (float *)malloc(nbars*sizeof(float));
  k = 0;
  for(j=c1; j<=c2; j++) for(i = r1; i <= r2; i++)
    {
      float  fv=0.0;
      char *str = EZ_SSheetGetCell(sheet, i, j);
      if(str) fv = (float)atof(str);
      if(fv>maxv)maxv=fv; if(fv<minv)minv=fv;
      ftmp[k++]=fv;
    }
  if(maxv == minv) maxv = minv + 1.0;
  if(barc->maxv != maxv ||   barc->minv != minv) allbad = 1;
  barc->maxv = maxv;
  barc->minv = minv;

  k=0;
  fptr   = barc->values;
  pieces = barc->pieces;
  xfactor= (float)barc->xsize/(float)nbars;
  yfactor = (barc->ysize)/(maxv-minv);
  EZ_FreezeFigItem(chart);
  if(allbad) EZ_FigItemDeleteAllPieces(chart,0);
  for(j=c1; j<=c2; j++) for(i = r1; i <= r2; i++)
    {
      if(allbad || ftmp[k] != fptr[k])
        {
          GC gc = (j&1)? redgc: greengc;
          int   ii, jj;
          float ff = (ftmp[k] - minv) * yfactor;
          fptr[k]=ftmp[k];
          ii = (int)ff;
          jj = (int)((float)((j-c1) +(i-r1)*(c2-c1+1)) * xfactor);
          if(!allbad)EZ_FigItemDeletePiece(chart, pieces[k], 0);
          pieces[k] = EZ_FigItemAddFilledRectangle(chart, gc, jj, 0, 5, ii);
        }
      k++;
    }
  if(allbad) EZ_FigItemAddString(chart, redgc, "simple chart", 12, 0, 100, 200);
  EZ_UnFreezeFigItem(chart);
  free(ftmp);
}

static void freeBarChart(EZ_Item *chart, void *d) 
{ barChart *c = (barChart *)d; free(c->values); free(c->pieces); free(c);}

static void testCb(EZ_Item *chart, void *d) 
{
  EZ_Widget *ss = (EZ_Widget *)d;
  EZ_SSheetSwapRow(ss, 1,4);
}
static void makeChart(EZ_Widget *widget, void *data)
{
  int r1=-1,c1,r2,c2;
  EZ_Widget *d = ssheet;

  if(redgc == None)
    {
      XGCValues  gcvalues;
      unsigned long red, green;
      red     = EZ_AllocateColorFromName("red");
      green   = EZ_AllocateColorFromName("green");
      gcvalues.foreground = red;
      redgc = EZ_GetGC(GCForeground, &gcvalues);
      gcvalues.foreground = green;
      greengc = EZ_GetGC( GCForeground, &gcvalues);
    }  
  EZ_SSheetGetHighlightedRegion(d,  &r1, &c1, &r2, &c2);

  if(r1 >= 0 && (r2 != r1 || c2 != c1))
    {
      EZ_Item *item;
      int          nbars = (r2-r1+1)*(c2-c1+1) + 1;
      barChart     *barchart = (barChart *)malloc(sizeof(barChart));
      EZ_FigPiece  **pieces = (EZ_FigPiece **) malloc(nbars * sizeof(EZ_FigPiece *));      
      float        *values = (float *)malloc(nbars * sizeof(float));
      
      memset(pieces, 0, nbars * sizeof(EZ_FigPiece *));
      memset(values, 0, nbars * sizeof(float));
      barchart->r1 = r1; barchart->c1 = c1;
      barchart->r2 = r2; barchart->c2 = c2;
      barchart->xsize=200; barchart->ysize=200;
      
      barchart->pieces = pieces;
      barchart->values = values;
      item = EZ_SSheetMakeChart(d, r1, c1, r2, c2, (EZ_SChartHandle)chartHandle, 0, barchart, 1);
      EZ_ConfigureItem(item, EZ_DESTROY_CALLBACK, freeBarChart, barchart , 
                       EZ_CALLBACK, testCb, d, 0);
      showStatus(NULL);
    }
  else showStatus("Please selection a region first");
}

static void ssort(EZ_Widget *widget, void *data)
{
  int r1=-1,c1,r2,c2;
  EZ_Widget *d = ssheet;

  EZ_SSheetGetHighlightedRegion(d,  &r1, &c1, &r2, &c2);

  if(r1 >= 0 && (r2 != r1 || c2 != c1))
    {
      EZ_FreezeWidget(d);
      EZ_SSheetSortRegion(d, r1,c1, r2, c2, c1, NULL);
      EZ_SSheetValidateCharts(d,r1,c1, r2, c2);
      EZ_UnFreezeWidget(d);
      showStatus(NULL);
    }
  else showStatus("Please selection a region first");
}
 
