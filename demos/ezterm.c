/*----------------------------------------------------------------------
 * 
 *        A vt100 terminal emulator
 *
 */
#include "EZ.h"
#include <string.h>

/*----------------------------------------------------------------------
 *           command line options
 */
static XrmOptionDescRec commandLineOptions[] = {
  {"-bgImage",    "frame.bgImageFile",    XrmoptionSepArg,  NULL},
  {"-bgImageB",   "*bgImageFileB",        XrmoptionSepArg,  NULL},
  {"-tfg",        "*xterm.foreground",     XrmoptionSepArg,  NULL},  
  {"-tbg",        "*xterm.textBackground", XrmoptionSepArg,  NULL},  
  {"-fn",         "*xterm.fontName",       XrmoptionSepArg,  NULL},
  {"-sl",         "*termSaveLines",       XrmoptionSepArg,  NULL},
  {"-rv",         "*termReverseVideo",    XrmoptionNoArg,  "True"},
  {"-shadow",     "*xterm.labelShadow",    XrmoptionSepArg,  NULL},
  {"-ccolor",     "*termCursorColor",     XrmoptionSepArg,  NULL},
  {"-e",          ".command",             XrmoptionSepArg,  NULL},
};

/*----------------------------------------------------------------------
 *          fallback resource specfication 
 */
static char *fallbackResources = 
"\n\
*term.fontName:      9x15\n\
"
;

/*----------------------------------------------------------------------
 * 
 */
static char      *shell = NULL;
static char      *command[] = {NULL, NULL, NULL, NULL};
static EZ_Widget *toplevel = NULL;     /* toplevel frame */
static EZ_Widget *emulator = NULL;     /* vt emulator    */
static EZ_Widget *flistBox = NULL;     /* font lister    */
static EZ_Widget *fileSelector = NULL; /* image selector */

static EZ_Pixmap *theImage = NULL;     /* background image */
static int       scaleImage = 0;       /* flag to scale bg image */
static char      imageFile[256];       /* image file name */
static unsigned char *rgbdata = NULL;  /* image in RGB    */
static int      imW, imH;              /* current size of image */

static Atom      deleteWindowAtom;     
static Atom      WMProtocolsAtom;


#define    LARGE_FONT "-adobe-helvetica-bold-r-normal-*-*-180-*-*-*-*-*-*"
/*----------------------------------------------------------------------
 *  exit dialogue callbacks. 
 *  when 'command' exits, post a dialogue to confirm exit.
 */
static void restartCb(EZ_Widget *w, void *d)
{
  EZ_Widget *toplevel = (EZ_Widget *)d;
  EZ_TermStartCommand(emulator, command);
  EZ_DestroyWidget(toplevel);
}

static void quitCb(EZ_Widget *w, void *d)
{
  EZ_TermStopCommand(emulator, 15);
  EZ_Shutdown();
  exit(0);
}

void dialogue(EZ_Widget *widget, void *data)
{
  EZ_Widget  *toplevel, *frame, *label, *restart, *quit;	      
  char str[128];

  sprintf(str, "Command '%s' has finished", command[0]);
  toplevel = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                                "popFrame", "PopFrame",
                                EZ_LABEL_STRING,   " ",
                                EZ_TEXT_LINE_LENGTH, 30,
                                EZ_JUSTIFICATION,    EZ_CENTER,
                                EZ_TRANSIENT,       True,
                                EZ_IPADY,           10,
                                EZ_ORIENTATION,     EZ_VERTICAL,
                                EZ_FILL_MODE,       EZ_FILL_BOTH,
                                NULL);

  label  =           EZ_CreateWidget(EZ_WIDGET_LABEL,    toplevel,
				     EZ_LABEL_STRING,    str,
				     EZ_TEXT_LINE_LENGTH, 60,
				     EZ_JUSTIFICATION,    EZ_CENTER,
                                     EZ_FOREGROUND,         "red",
                                     EZ_LABEL_SHADOW,       1,1,
                                     EZ_FONT_NAME,         LARGE_FONT,
				     NULL);
  
  frame =            EZ_CreateWidget(EZ_WIDGET_FRAME,   toplevel,
				     EZ_HEIGHT,         0,
				     NULL);

  restart =          EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
				     EZ_LABEL_STRING,   "Restart",
				     EZ_UNDERLINE,       0,
				     EZ_CALLBACK,        restartCb, toplevel,
				     NULL );
  quit =             EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                     EZ_LABEL_STRING,   "Exit",
                                     EZ_UNDERLINE,       0,
				     EZ_CALLBACK,        quitCb, NULL,
				     NULL );

  EZ_DisplayWidgetUnderPointer(quit, -10, -10);
  EZ_SetFocusTo(quit);
}

/*----------------------------------------------------------------------
 * handle  WM_DELETE_WINDOW message from the window manager.
 */
void clientMessageHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  XClientMessageEvent *ev = (XClientMessageEvent *)xev;      
      
  if(ev->message_type == WMProtocolsAtom)
    {
      Atom c = (ev->data.l)[0];
      if(c == deleteWindowAtom)
	{
          if(widget != toplevel)  EZ_HideWidget(widget);
	  else
            {
              EZ_Shutdown();
              exit(0);
            }
	}
    }
}


/*----------------------------------------------------------------------
 *  change fonts ...
 */
static void fontEntryCb(EZ_Widget *widget, void *data) 
{
  EZ_Widget *tp = (EZ_Widget *)data;
  EZ_Widget *entry = EZ_GetWidgetPtrData(tp);
  char *str = EZ_GetEntryString(entry);
  if(str && *str) EZ_ConfigureWidget(emulator, EZ_FONT_NAME, str, NULL);
  /* hide the dialogue */
  EZ_HideWidget(tp);
}

static void cancelCb(EZ_Widget *widget, void *data) 
{
  EZ_HideWidget((EZ_Widget *)data);
}

static void enterFontCb(EZ_Widget *widget, void *data) 
{
  static EZ_Widget *ftoplevel = NULL;
  if(ftoplevel == NULL)
    {
      EZ_Widget *entry, *frame, *confirm, *cancel;

      ftoplevel= EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                                 EZ_LABEL_STRING,    "Please Enter a Font Name",
                                 EZ_TEXT_LINE_LENGTH, 24,
                                 EZ_JUSTIFICATION,    EZ_CENTER,
                                 EZ_TRANSIENT,       True,
                                 EZ_IPADY,           10,
                                 EZ_ORIENTATION,     EZ_VERTICAL,
                                 EZ_FILL_MODE,       EZ_FILL_BOTH,
                                 EZ_FONT_NAME,       LARGE_FONT,
                                 EZ_FOREGROUND,         "red",
                                 EZ_LABEL_SHADOW,       1,1,
                                 0);

      entry =    EZ_CreateWidget(EZ_WIDGET_ENTRY,    ftoplevel,
                                 EZ_TEXT_LINE_LENGTH, 60,
                                 EZ_BACKGROUND,      "gray90",
                                 EZ_CALLBACK,         fontEntryCb, ftoplevel,
                                 EZ_ENTRY_STRING,     "10x20",
                                 0);
	      
      frame =            EZ_CreateWidget(EZ_WIDGET_FRAME,   ftoplevel,
                                         EZ_HEIGHT,         0,
                                         NULL);
      confirm =          EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                         EZ_LABEL_STRING,   "Ok",
                                         EZ_UNDERLINE,       0,
                                         EZ_CALLBACK,       fontEntryCb, ftoplevel,
                                         NULL );
      cancel =           EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                         EZ_LABEL_STRING,   "Cancel",
                                         EZ_UNDERLINE,       1,
                                         EZ_CALLBACK,        cancelCb, ftoplevel,
                                         NULL );
      EZ_SetWidgetPtrData(ftoplevel, entry);
    }
  EZ_DisplayWidget(ftoplevel);
  EZ_RaiseWidgetWindow(ftoplevel);
  EZ_SetFocusTo(EZ_GetWidgetPtrData(ftoplevel));
}

static void fontListerCb(EZ_Widget *widget, void *data)
{
  EZ_Item **selection = EZ_GetFancyListBoxSelection(widget);
  if(selection)
    {
      char *fname=NULL; int len;
      EZ_Item *item = selection[0];  /* */
      EZ_GetLabelItemStringInfo(item, &fname, &len);
      if(fname)  EZ_ConfigureWidget(emulator, EZ_FONT_NAME, fname, NULL);
    }
}

static void listFonts(EZ_Widget *notused1, void *notused2)
{
  if(flistBox == NULL)
    {
      EZ_Item     *items[1024];
      Display     *dpy = EZ_GetDisplay();
      int         i, nnames, count;
      char        **fontNames;
      
      flistBox = EZ_CreateWidget(EZ_WIDGET_FANCY_LIST_BOX,  NULL,
                                 EZ_FANCY_LISTBOX_TYPE,     1, /* a row a time */
                                 EZ_FANCY_LISTBOX_COLUMNS,  1, /* only one column */
                                 EZ_BORDER_WIDTH,           1,
                                 EZ_TEXT_BACKGROUND,        "white",
                                 EZ_ROW_BG,                 1, "white", "bisque",
                                 EZ_SELECTION_FOREGROUND,   "red",
                                 EZ_SELECTION_BACKGROUND,   "yellow",
                                 EZ_IPADX,                  0,
                                 EZ_IPADY,                  2,
                                 EZ_CALLBACK,               fontListerCb, NULL,
                                 EZ_SIZE,                   500, 500,
                                 0);
      fontNames = XListFonts(dpy, "????*", 1020, &nnames);
      if(fontNames && nnames > 0)
        {
          for(i = 0; i < nnames; i++)
            {
              items[i] = EZ_CreateItem(EZ_LABEL_ITEM,
                                       EZ_LABEL_STRING, fontNames[i],
                                       /* EZ_FONT_NAME,    fontNames[i],*/
                                       EZ_TEXT_LINE_LENGTH, 200,
                                       0);
            }
          items[i] = NULL;
          XFreeFontNames(fontNames);
          EZ_SetFancyListBoxData(flistBox, items, i, 1);
        }
    }
  EZ_DisplayWidget(flistBox);
  EZ_RaiseWidgetWindow(flistBox);
}

/*----------------------------------------------------------------------
 *  change colors ...
 */

static char *colors[] =
{"red2", "green2", "blue2", "yellow2", "magenta2",  "cyan2", "gray90", "gray10", "gray76", "white", NULL};

static void setFgColor(EZ_Widget *widget, void *data)
{
  if(data)
    {
      EZ_TermTouchScreen(emulator);
      EZ_ConfigureWidget(emulator, EZ_FOREGROUND, (char *)data, 0);
    }
}

static void setBgColor(EZ_Widget *widget, void *data)
{
  if(data)
    {
      EZ_TermTouchScreen(emulator);
      EZ_ConfigureWidget(emulator, EZ_TEXT_BACKGROUND, (char *)data, 0);
    }
}

static void fgcCb(EZ_Widget *widget, void *data) 
{
  EZ_Widget *tp = (EZ_Widget *)data;
  EZ_Widget *entry = EZ_GetWidgetPtrData(tp);
  char *str = EZ_GetEntryString(entry);
  if(str && *str) EZ_ConfigureWidget(emulator, EZ_FOREGROUND, str, NULL);
  /* hide the dialogue */
  EZ_HideWidget(tp);
}

static void bgcCb(EZ_Widget *widget, void *data) 
{
  EZ_Widget *tp = (EZ_Widget *)data;
  EZ_Widget *entry = EZ_GetWidgetPtrData(tp);
  char *str = EZ_GetEntryString(entry);
  if(str && *str) { EZ_ConfigureWidget(emulator, EZ_TEXT_BACKGROUND, str, NULL); scaleImage = 0;}
  /* hide the dialogue */
  EZ_HideWidget(tp);
}


static void enterColorCb(EZ_Widget *widget, void *data)
{
  static EZ_Widget *ctoplevel = NULL;
  if(ctoplevel == NULL)
    {
      EZ_Widget *entry, *frame, *fgc, *bgc, *cancel;

      ctoplevel= EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                                 EZ_LABEL_STRING,    "Please Enter a Color Name",
                                 EZ_TEXT_LINE_LENGTH, 24,
                                 EZ_JUSTIFICATION,    EZ_CENTER,
                                 EZ_TRANSIENT,       True,
                                 EZ_IPADY,           10,
                                 EZ_ORIENTATION,     EZ_VERTICAL,
                                 EZ_FILL_MODE,       EZ_FILL_BOTH,
                                 EZ_FONT_NAME,       LARGE_FONT,
                                 EZ_FOREGROUND,         "red",
                                 EZ_LABEL_SHADOW,       1,1,
                                 0);

      entry =    EZ_CreateWidget(EZ_WIDGET_ENTRY,    ctoplevel,
                                 EZ_TEXT_LINE_LENGTH, 60,
                                 EZ_BACKGROUND,      "gray90",
                                 EZ_ENTRY_STRING,     "black",
                                 0);
	      
      frame =            EZ_CreateWidget(EZ_WIDGET_FRAME,   ctoplevel,
                                         EZ_HEIGHT,         0,
                                         NULL);
      fgc =              EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                         EZ_LABEL_STRING,   "Foreground",
                                         EZ_UNDERLINE,       0,
                                         EZ_CALLBACK,       fgcCb, ctoplevel,
                                         NULL );
      bgc =              EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                         EZ_LABEL_STRING,   "Background",
                                         EZ_UNDERLINE,       0,
                                         EZ_CALLBACK,       bgcCb, ctoplevel,
                                         NULL );
      cancel =           EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                                         EZ_LABEL_STRING,   "Cancel",
                                         EZ_UNDERLINE,       1,
                                         EZ_CALLBACK,        cancelCb, ctoplevel,
                                         NULL );
      EZ_SetWidgetPtrData(ctoplevel, entry);
    }
  EZ_DisplayWidget(ctoplevel);
  EZ_RaiseWidgetWindow(ctoplevel);
  EZ_SetFocusTo(EZ_GetWidgetPtrData(ctoplevel));
}


/*----------------------------------------------------------------------
 *  find and set a background image
 */
static void scaleSetBgImage(int flag)
{
  unsigned int sW, sH, bw, depth;
  static int lw, lh;
  Window window, root;
  int x, y, w=0, h,  iwidth, iheight;

  if(!EZ_WidgetIsViewable(toplevel)) return;
  XGetGeometry(EZ_GetDisplay(), EZ_GetWidgetWindow(toplevel),
               &root, &x,&y,&sW,&sH, &bw, &depth);

  if(flag == 1 && theImage != NULL) /* rescale */
    {
      int ww, hh; Pixmap jnk;
      EZ_GetLabelPixmapInfo(theImage, &jnk, &jnk, &ww, &hh);
      if(abs(ww - sW) + abs(hh - sH) < 10) return;
    }

  if(flag && rgbdata != NULL)
    {
      unsigned char *rgb = rgbdata;
      unsigned char *newrgb = NULL;

      scaleImage = 1;
      if(sW && sH)
        {
          newrgb = EZ_ScaleRGB(rgbdata, imW, imH, sW, sH, 80, 0);
          if(newrgb)
            {
              rgb = newrgb;
              iwidth = sW;
              iheight = sH;
            }
          else scaleImage = 0;
          theImage = EZ_CreateLabelPixmapFromRawRGB(rgb, iwidth, iheight);
          if(newrgb) (void)free(newrgb);
          EZ_ConfigureWidget(toplevel, EZ_BG_PIXMAP, theImage,  EZ_MARK_CLEAN, 1, 0);
          EZ_TermTouchScreen(emulator);      
          EZ_ConfigureWidget(emulator, EZ_TEXT_BACKGROUND, 0,  EZ_MARK_CLEAN, 1, 0);
        }
    }
  else
    {
      if(rgbdata) free(rgbdata);
      rgbdata = NULL;
      if(EZ_ReadImageFile(imageFile, &iwidth, &iheight, &rgbdata))
        {
          unsigned char *rgb = rgbdata;
          unsigned char *newrgb = NULL;
          Pixmap jnk; int ww,hh;
          imW = iwidth; imH = iheight;
          scaleImage = 1;
          if(sW && sH)
            {
              newrgb = EZ_ScaleRGB(rgbdata, iwidth, iheight, sW, sH, 80,0);
              if(newrgb)
                {
                  rgb = newrgb;
                  iwidth = sW;
                  iheight = sH;
                }
              else scaleImage = 0;
            }
          theImage = EZ_CreateLabelPixmapFromRawRGB(rgb, iwidth, iheight);
          if(newrgb) (void)free(newrgb);
          EZ_ConfigureWidget(toplevel, EZ_BG_PIXMAP, theImage,  0);
          EZ_TermTouchScreen(emulator);      
          EZ_ConfigureWidget(emulator, EZ_TEXT_BACKGROUND, 0,  0);
        }          
      else scaleImage = 0;
    }
}

/*--------------------------------------------------------------------------
 * Registered to toplevel widgets. When the window size changes, 
 * rescale the bgImage.
 */
static void eventHandler(EZ_Widget *widget, void *data, int etype, XEvent *xev)
{
  if(xev->type == ConfigureNotify && scaleImage == 1)
    {
      scaleSetBgImage(1);
    }
}


static void setBgImage(EZ_Widget *widget, void *data)
{
  char *file = NULL;
  if(data) file = EZ_GetFileSelectorSelection((EZ_Widget *)data);
  else file = EZ_GetFileSelectorSelection(widget);

  if(file)
    {
      if(data == NULL)
        {
          EZ_ConfigureWidget(toplevel, EZ_BG_IMAGE_FILE, file, 0);
          EZ_TermTouchScreen(emulator);      
          EZ_ConfigureWidget(emulator, EZ_TEXT_BACKGROUND, 0, 0);
          scaleImage = 0;
        }
      else  /* scale */
        {
          strcpy(imageFile, file);
          scaleSetBgImage(0);
        }
    }
}

static void imageSelector(EZ_Widget *notused, void *notused2)
{
  if(fileSelector == NULL)
    {
      EZ_Widget *jnk, *ok, *filter, *cancel, *scaleBtn;
      fileSelector = EZ_CreateWidget(EZ_WIDGET_FILE_SELECTOR, NULL,
                                     EZ_CALLBACK, setBgImage, NULL,
                                     EZ_SIZE,     400, 400,
                                     0);
      EZ_GetFileSelectorWidgetComponents(fileSelector, &jnk, &jnk, &jnk, &jnk,
                                         &ok, &filter, &cancel);
      EZ_ConfigureWidget(ok, EZ_LABEL_STRING, "set", NULL);
      scaleBtn = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, NULL,
                                 EZ_LABEL_STRING, "Scale & Set",
                                 EZ_CALLBACK, setBgImage, fileSelector,
                                 NULL);
      EZ_InsertWidgetBefore(scaleBtn, filter);
    }
  EZ_DisplayWidget(fileSelector);
  EZ_RaiseWidgetWindow(fileSelector);
}


/*-----------------------------------------------------------------------
 * setup the menubar ...
 */

static void setupMenu(EZ_Widget *mbar)
{
  EZ_Widget *menu, *tmp, *mitem, *fgmenu, *bgmenu;
  int i = 0;

  /* font menu */
  menu =     EZ_CreateWidgetXrm(EZ_WIDGET_MENU,      NULL,
				"FontMenu", "fontMenu",
				EZ_MENU_TEAR_OFF,    1,
				NULL);
  
  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  menu,
			     EZ_LABEL_STRING,               "List all fonts",
			     EZ_UNDERLINE,                  0,
			     EZ_CALLBACK,                   listFonts, NULL,
			     NULL);

  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_SEPARATOR, menu, 0);

  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  menu,
			     EZ_LABEL_STRING,               "Enter a font",
			     EZ_UNDERLINE,                  0,
			     EZ_CALLBACK,                   enterFontCb, NULL,
			     NULL);

  mitem = EZ_MenuBarAddItemAndMenu(mbar, "Font", 0, menu);



  /* foreground color menu */
  fgmenu =     EZ_CreateWidgetXrm(EZ_WIDGET_MENU,      NULL,
                                  "FgMenu", "fgMenu",
                                  EZ_MENU_TEAR_OFF,    1,
                                  NULL);
  while(colors[i] != NULL)
    {
      tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  fgmenu,
                                 EZ_LABEL_STRING,               colors[i],
                                 EZ_UNDERLINE,                  0,
                                 EZ_CALLBACK,                   setFgColor, colors[i],
                                 EZ_FOREGROUND,                 colors[i],
                                 NULL);
      i++;
    }
  /* background color menu */
  bgmenu =     EZ_CreateWidgetXrm(EZ_WIDGET_MENU,      NULL,
                                  "BgMenu", "bgMenu",
                                  EZ_MENU_TEAR_OFF,    1,
                                  NULL);
  i = 0;
  while(colors[i] != NULL)
    {
      tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  bgmenu,
                                 EZ_LABEL_STRING,               colors[i],
                                 EZ_UNDERLINE,                  0,
                                 EZ_CALLBACK,                   setBgColor, colors[i],
                                 EZ_BACKGROUND,                 colors[i],
                                 NULL);
      i++;
    }
  
  menu =     EZ_CreateWidgetXrm(EZ_WIDGET_MENU,      NULL,
				"ColorMenu", "colorMenu",
				EZ_MENU_TEAR_OFF,    1,
				NULL);
  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_SUBMENU,  menu,  
                             EZ_LABEL_STRING,         "Foreground",
                             0);
  EZ_SetSubMenuMenu(tmp, fgmenu);

  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_SUBMENU,  menu,  
                             EZ_LABEL_STRING,         "Background",
                             0);
  EZ_SetSubMenuMenu(tmp, bgmenu);

  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_SEPARATOR, menu, 0);

  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  menu,  
                             EZ_LABEL_STRING,         "Enter a color",
                             EZ_CALLBACK,             enterColorCb, NULL,
                             0);

  mitem = EZ_MenuBarAddItemAndMenu(mbar, "Color", 0, menu);


  /* pixmap menu */
  menu =     EZ_CreateWidgetXrm(EZ_WIDGET_MENU,      NULL,
				"pixmapMenu", "pixmapMenu",
				EZ_MENU_TEAR_OFF,    1,
				NULL);
  
  tmp  =     EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON,  menu,
			     EZ_LABEL_STRING,               "select an image",
			     EZ_UNDERLINE,                  0,
			     EZ_CALLBACK,                   imageSelector, NULL,
			     NULL);

  mitem = EZ_MenuBarAddItemAndMenu(mbar, "Image", 0, menu);

}

/*----------------------------------------------------------------------
 *  So the story begins ...
 */
int main(int ac, char **av)
{
  EZ_Widget *frame, *mbar;
  char      *value;

  EZ_InitializeXrm("Ezterm",                           /* class name            */
		   commandLineOptions,                 /* cmd line options      */
		   EZ_ArraySize(commandLineOptions),   /* # of cmd line options */
		   &ac,                                /* arg count, in and out */
		   av,                                 /* cmd args, in and out  */
		   fallbackResources,                  /* fallback resources    */
		   0);                                 /* useless junk          */

  if(ac > 1) 
    {
      fprintf(stderr, "\nUsage: %s \n", av[0]);
      fprintf(stderr, "\t-e           command\n");
      fprintf(stderr, "\t-bgImage     bg_tile_image\n");
      fprintf(stderr, "\t-bgImageB    bg_tile_image_secondary\n");
      fprintf(stderr, "\t-tbg         text_background\n");
      fprintf(stderr, "\t-tfg         text_foreground_color\n");
      fprintf(stderr, "\t-ccolor      cursor_color\n");
      fprintf(stderr, "\t-fn          fontName\n");
      fprintf(stderr, "\t-rv          reverse_video\n");
      fprintf(stderr, "\t-sl          number_of_lines_to_buffer\n");
      fprintf(stderr, "\t-shadow      shadow_offset ((xoff<<4)|yoff)\n");
      EZ_PrintStandardOptions();
      EZ_Shutdown();
      exit(0);
    }

  if(EZ_GetApplicationResource("command", &value))  shell = value;
  else if((shell = getenv("SHELL")) == NULL)
    shell = "/bin/sh";
  command[0] = shell;
  WMProtocolsAtom  = EZ_GetAtom("WM_PROTOCOLS");
  deleteWindowAtom = EZ_GetAtom("WM_DELETE_WINDOW");
  EZ_SetClientMessageHandler(clientMessageHandler,   NULL);

  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,   NULL,
                              "Toplevel",       "toplevel",
                              EZ_SIZE,           800, 400,
                              EZ_PADX,           0,
                              EZ_PADY,           0,
                              EZ_FILL_MODE,      EZ_FILL_BOTH,
                              EZ_ORIENTATION,    EZ_VERTICAL,
                              EZ_EVENT_HANDLER,  eventHandler, NULL,
                              0);
  toplevel = frame;
  mbar =  EZ_CreateWidgetXrm(EZ_WIDGET_MENU_BAR, frame,
                             "MenuBar",           "menubar",
                             EZ_HEIGHT,              0,
                             EZ_IPADX,               6,
                             EZ_PADX,                0,
                             EZ_PADY,                0,
                             EZ_DOCKABLE,            1,
                             NULL);
  setupMenu(mbar);
  
  emulator  = EZ_CreateWidgetXrm(EZ_WIDGET_TERM,   frame,
                                 "XTerm", "xterm",
                                 0);
  EZ_TermSetExitHandler(emulator, dialogue, emulator);
  EZ_DisplayWidget(frame);
  EZ_TermStartCommand(emulator, command);
  EZ_EventMainLoop();
}
/*------------------ that's all ------------------*/
