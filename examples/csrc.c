/****************************************************************
 ***                                                          ***
 ***     text.c                                               ***
 ***          simple EZ_Text widget demo                      ***
 ***                                                          ***
 ***************************************************************/
#include <stdio.h>
#include <ctype.h>

#include "EZ.h"

static EZ_Widget *tp_frame;    /* the master frame */
static EZ_Widget *textW;      /* the text widget  */
static void Load_CSRC_File();

/*************************************************************************/
static char *colors[] = {
  "red",
  "green",
  "blue",
  "yellow",
  "cyan",
  "magenta",
  "white",
  "gray",
  "black",
};
#define NCOLORS 9
/*************************************************************************/

static char *fonts[] = {
  "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Helvetica-Medium-R-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Times-Bold-I-Normal--*-120-*-*-*-*-*-*",
  "-Adobe-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*",
  "-*-Courier-Medium-R-Normal--*-180-*-*-*-*-*-*",
  "-Adobe-Helvetica-Bold-R-Normal--*-180-*-*-*-*-*-*",
  "-Adobe-Times-Bold-I-Normal--*-180-*-*-*-*-*-*",
  "-Adobe-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*",
  "-*-Courier-Medium-R-Normal--*-240-*-*-*-*-*-*",
  "-Adobe-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*",
  "-Adobe-Times-Bold-I-Normal--*-240-*-*-*-*-*-*",
};
#define NFONTS 12
#define COMMENT_FONT  fonts[0]
#define TYPE_FONT     fonts[1]
#define CONTROL_FONT  fonts[2]
#define CPP_FONT      fonts[3]

/**************************************************************************/
static void fcolorMCallback(menu, data)
     EZ_Widget *menu; void *data;
{
  int i = EZ_GetWidgetReturnedData(menu);
  if( i >= 0 && i < NCOLORS) 
    EZ_TextSetRegionForeground(textW, colors[i]);
}
/**************************************************************************/
static void bcolorMCallback(menu, data)
     EZ_Widget *menu; void *data;
{
  int i = EZ_GetWidgetReturnedData(menu);
  if( i >= 0 && i < NCOLORS) 
    EZ_TextSetRegionBackground(textW, colors[i]);
}
/**************************************************************************/
static void fontMCallback(menu, data) 
     EZ_Widget *menu; void *data;
{
  int i = EZ_GetWidgetReturnedData(menu);
  if( i >= 0 && i < NFONTS) 
    EZ_TextSetRegionFont(textW, fonts[i]);
}
static void underline_region(widget,data)
     EZ_Widget *widget; void *data;
{
  EZ_TextSetRegionUnderline(textW,1);
}
/**************************************************************************/
#define OPEN   101
#define OPENA  102
#define NEWF   103
#define SAVE   104
#define SAVEA  105
#define EXIT   106

static EZ_Widget *file_entry, *dialogue = NULL;

static void display_dialogue()
{
  if(dialogue == NULL)
    {
      dialogue = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                                 EZ_LABEL_STRING, "Please Enter a File Name",
                                 EZ_TRANSIENT,    True, 
                                 0);
      file_entry = EZ_CreateWidget(EZ_WIDGET_ENTRY, dialogue,
                                   0);
    }
  EZ_SetGrab(dialogue);
  EZ_DisplayWidget(dialogue);
  EZ_SetFocusTo(file_entry);
}
/***********************************************************************/
static void replace_region_by_bitmap(menu, d)
     EZ_Widget *menu, *d;
{
  char *fname = EZ_GetEntryString(file_entry);
  EZ_TextReplaceRegionWithPixmap(textW, fname);
  EZ_HideWidget(dialogue);
  EZ_ReleaseGrab();
}
static void replace_callback(menu, d)
     EZ_Widget *menu; void *d;
{
  display_dialogue();
  EZ_SetWidgetCallBack(file_entry, replace_region_by_bitmap, NULL);
}

static void clear_region_tprop(w,d) EZ_Widget *w; void *d;
{
  EZ_TextSetRegionProperty(textW, NULL);
}
/***********************************************************************/

static void Load_Regular_File()
{
  int length;
  char *fname = EZ_GetEntryString(file_entry);
  length = strlen(fname);
  if(length > 2 && fname[length-2] == '.' && fname[length-1] == 'c')
    { 
      /* a C src file */
      EZ_FreezeWidget(textW);
      Load_CSRC_File(fname);
      EZ_TextBeginningOfBuffer(textW);
      EZ_UnFreezeWidget(textW);
    }
  else  EZ_TextLoadFile(textW,fname);
  EZ_HideWidget(dialogue);
  EZ_ReleaseGrab();
}

static void Save_Regular_File()
{
  int length;
  char *fname = EZ_GetEntryString(file_entry);
  EZ_TextSaveText(textW, fname);
  EZ_HideWidget(dialogue);
  EZ_ReleaseGrab();
}
static void open_regular_file()
{
  display_dialogue();
  EZ_SetWidgetCallBack(file_entry,Load_Regular_File, NULL);
}
static void save_regular_file()
{
  display_dialogue();
  EZ_SetWidgetCallBack(file_entry,Save_Regular_File, NULL);
}
/***********************************************************************/
static void Load_Annotated_File()
{
  char *fname = EZ_GetEntryString(file_entry);  
  EZ_TextLoadAnnotatedFile(textW, fname);
  EZ_HideWidget(dialogue);
}

static void Save_Annotated_File()
{
  char *fname = EZ_GetEntryString(file_entry);  
  EZ_TextSaveAnnotatedText(textW, fname);
  EZ_HideWidget(dialogue);
}
/***********************************************************************/
static void fileMCallback(menu,data) 
     EZ_Widget *menu; void *data;
{
  char *fname;
  int i = EZ_GetWidgetReturnedData(menu);
  switch(i)
    {
    case OPEN:
      open_regular_file();
      break;
    case OPENA:
      display_dialogue();
      EZ_SetWidgetCallBack(file_entry,Load_Annotated_File, NULL);
      break;
    case NEWF:
      EZ_TextClear(textW);
      break;
    case SAVE:
      open_regular_file();
      break;
    case SAVEA:
      display_dialogue();
      EZ_SetWidgetCallBack(file_entry, Save_Annotated_File, NULL);
      break;
    case EXIT:
      EZ_Shutdown();
      exit(0);
      break;
    default:
      break;
    }
}
/**************************************************************************/

main(ac, av) int ac; char **av;
{
  EZ_Widget *tmp, *tmpa, *mbar;
  EZ_Widget *fileM, *editM, *fcolorM, *bcolorM, *fontM;
  int       i;
  char      str[32];

  EZ_Initialize(ac,av,1);
  EZ_DisableSliderDepression();

  /* Menu File */
  fileM = EZ_CreateSimpleMenu("Open Regular File %x101 %u 0|Open Annotated File %x102 %u1|New %x103 %u0 |Save Text %x104 %u0|Save Annotated Text %x105 %u5|Exit %u0 %x106");
  EZ_ConfigureWidget(fileM,
		     EZ_CALLBACK, fileMCallback, NULL,
		     0);

  /* Menu F_color, B_color, font. They are used as submenus */
  fcolorM = EZ_CreateWidget(EZ_WIDGET_POPUP_MENU,   NULL,
			    EZ_LABEL_STRING,        "Set Region foreground",
			    EZ_CALLBACK,            fcolorMCallback, NULL,
			    NULL);
  for(i = 0; i < NCOLORS; i++)
    {
      sprintf(str, "Ctrl-%c", *colors[i]);
      tmp = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, fcolorM, 
			    EZ_LABEL_STRING, colors[i], 
			    EZ_RETURN_VALUE, i,
			    EZ_SHORTCUT_KEY, str,
			    0);
    }
  bcolorM = EZ_CreateWidget(EZ_WIDGET_POPUP_MENU,   NULL,
			    EZ_LABEL_STRING,        "Set Region background",
			    EZ_CALLBACK,            bcolorMCallback, NULL,
			    NULL);
  for(i = 0; i < NCOLORS; i++)
    {
      sprintf(str, "Ctrl-%c", *colors[i]);
      tmp = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, bcolorM, 
			    EZ_LABEL_STRING, colors[i], 
			    EZ_RETURN_VALUE, i,
			    EZ_SHORTCUT_KEY, str,
			    0);
    }

  /* menu font */
  fontM = EZ_CreateWidget(EZ_WIDGET_POPUP_MENU,   NULL,
			  EZ_LABEL_STRING,        "Set Region Font",
			  EZ_CALLBACK,            fontMCallback, NULL,
			  NULL);
  for(i = 0; i < NFONTS; i++)
    {
      tmp = EZ_CreateWidget(EZ_WIDGET_MENU_RADIO_BUTTON, fontM, 
			    EZ_LABEL_STRING, fonts[i],
			    EZ_RETURN_VALUE, i,
			    EZ_RADIO_BUTTON_VALUE, i,
			    EZ_RADIO_BUTTON_GROUP, 2,
			    EZ_TEXT_LINE_LENGTH, 80,
			    0);
    }

  /* menu Edit */
  editM = EZ_CreateWidget(EZ_WIDGET_POPUP_MENU, NULL,
			  EZ_LABEL_STRING, "Edit",
			  0);
  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_SUBMENU, editM,
			EZ_LABEL_STRING, "Set Region Foreground",
			EZ_UNDERLINE, 11,
			0);
  EZ_SetSubMenuMenu(tmp, fcolorM);

  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_SUBMENU, editM,
			EZ_LABEL_STRING, "Set Region Background",
			EZ_UNDERLINE, 11,
			0);
  EZ_SetSubMenuMenu(tmp, bcolorM);

  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_SUBMENU, editM,
			EZ_LABEL_STRING, "Set Region Font",
			EZ_UNDERLINE, 14,
			0);
  EZ_SetSubMenuMenu(tmp, fontM);

  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, editM,
			EZ_LABEL_STRING, "Underline Region",
			EZ_UNDERLINE, 0,
			EZ_CALLBACK, underline_region, NULL,
			0);


  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, editM,
			EZ_LABEL_STRING, "Replace Region By Image",
			EZ_UNDERLINE, 0,
			EZ_CALLBACK, replace_callback, NULL,
			0);


  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_NORMAL_BUTTON, editM,
			EZ_LABEL_STRING, "Clear Region Property",
			EZ_UNDERLINE, 0,
			EZ_CALLBACK, clear_region_tprop, NULL,
			0);





  tp_frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,          /* toplevel widget */
                             EZ_IPADX, 0, EZ_IPADY, 0,
                             EZ_PADX, 4, EZ_PADY, 0,
                             EZ_BG_IMAGE_FILE,"bg1.gif",      /* background tile */ 
                             EZ_STACKING,  EZ_VERTICAL,
                             EZ_WIDTH,     650, EZ_HEIGHT, 500,
                             EZ_FILL_MODE, EZ_FILL_BOTH,
                             0);
  
  mbar = EZ_CreateWidget(EZ_WIDGET_FRAME, tp_frame,
                         EZ_PADX,         0,
                         EZ_PADY,         0,
                         EZ_EXPAND,       True, 
                         EZ_HEIGHT,       0,
                         EZ_STACKING,     EZ_HORIZONTAL_LEFT,
                         EZ_BORDER_WIDTH, 2,
                         EZ_BORDER_TYPE,  EZ_BORDER_RAISED,
                         0);  


  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON, mbar,
                        EZ_LABEL_STRING,  "File", 
                        EZ_PADY, 2, 
                        0);
  EZ_SetMenuButtonMenu(tmp, fileM);
  tmp = EZ_CreateWidget(EZ_WIDGET_MENU_BUTTON, mbar,
                        EZ_LABEL_STRING,  "Edit", 
                        EZ_PADY, 2, 
                        0);
  EZ_SetMenuButtonMenu(tmp, editM);

 
  textW = EZ_CreateWidget(EZ_WIDGET_TEXT, tp_frame,
                          EZ_IMAGE_FILE,  "cloth.gif",
                          EZ_BG_IMAGE_FILE_B,"bg.gif",    /* background tile */
                          EZ_PROPAGATE, 0,
                          EZ_EMBEDER, 1,
			  EZ_FONT_NAME, "-*-lucidatypewriter-medium-r-*-*-12-*-*-*-*-*-*-*",
                          EZ_SELECTION_BACKGROUND, "#00afaf",		     
                          0);

  EZ_DisplayWidget(tp_frame);                              /* display the toplevel widget */ 
  EZ_FreezeWidget(textW);
  Load_CSRC_File("csrc.c"); 
  EZ_TextBeginningOfBuffer(textW);
  EZ_UnFreezeWidget(textW);
  while(1) EZ_WaitAndServiceNextEvent();

  EZ_Shutdown();
  exit(0);
}

/*********************************************************************************************/
#define IS_WHITE_SPACE(c)  (c == ' ' || c == '\t' || c == '\n')

static EZ_TextProperty *comment = NULL, *types = NULL;
static EZ_TextProperty *control = NULL, *cpp = NULL, *ez_func = NULL;

static void Load_CSRC_File(file)
     char *file;
{
  FILE *fp, *fopen();
  char str[1024], *tmp, *tmp1, c;
  int  in_comment = 0, i, count, length;

  EZ_TextClear(textW);
  fp = fopen(file, "r");
  if(fp == (FILE *)NULL)
    {
      sprintf(str, "Cannot Open %s",file);
      EZ_TextInsertString(textW, str);
      return;
    }

  comment = EZ_GetTextProperty(EZ_FONT_NAME, COMMENT_FONT,
			       EZ_FOREGROUND, "gray42", 0);
  types   = EZ_GetTextProperty(EZ_FONT_NAME, TYPE_FONT, 
			       EZ_FOREGROUND, "#ff0000", 0);
  control = EZ_GetTextProperty(EZ_FONT_NAME, CONTROL_FONT,
			       EZ_FOREGROUND, "#ffff00", 0);
  cpp     = EZ_GetTextProperty(EZ_FONT_NAME, CPP_FONT,
			       EZ_FOREGROUND, "#ff00ff", 0);
  ez_func = EZ_GetTextProperty(EZ_FONT_NAME, CPP_FONT,
			       EZ_FOREGROUND,"#0000ff",0);      

  while(fgets(str, 1023, fp))
    {
      tmp = str;
      length = strlen(str);
      i = 0;
      count = 0;
      while(count < length)
	{
	  if(in_comment)
	    {
	      while(tmp[i])
		{
		  if(tmp[i] == '*' && tmp[i+1] == '/')
		    {
		      c = tmp[i+2]; tmp[i+2] = 0;
		      EZ_TextInsertStringWithProperty(textW,tmp, comment);
		      in_comment = 0;
		      count += 2; 
		      tmp[i+2] = c;
		      tmp = tmp + i+2;
		      i = 0;
		      break;
		    }
		  i++;
		  count++;
		}
	      if(tmp[i] == 0)
		EZ_TextInsertStringWithProperty(textW,tmp, comment);
	      continue;
	    }

	  while(IS_WHITE_SPACE(tmp[i])) { count++; i++;}
	  if(i)
	    {
	      c = tmp[i]; tmp[i] = 0;
	      EZ_TextInsertStringWithProperty(textW,tmp, NULL);
	      tmp[i] = c;
	      tmp = tmp + i;
	      i = 0;
	    }
	  
	  if(!strncmp(tmp, "/*", 2))
	    {
	      in_comment = 1;
	      continue;
	      i = 0;
	    }
	  else if(tmp[0] == '#')
	    {
	      int iscpp = 0;
	      if(!strncmp(tmp, "#define", 7) ||
		 !strncmp(tmp, "#include", 8) ||
		 !strncmp(tmp, "#undef", 6) ||
		 !strncmp(tmp, "#ifdef", 6) ||
		 !strncmp(tmp, "#else", 5) ||
		 !strncmp(tmp, "#endif", 6) ||
		 !strncmp(tmp, "#if", 3))
		iscpp = 1;

	      i = 0;
	      while( !IS_WHITE_SPACE(tmp[i])) { i++, count++;}
	      c = tmp[i]; tmp[i] = 0;
	      EZ_TextInsertStringWithProperty(textW,tmp, iscpp == 0? NULL : cpp);
	      tmp[i] = c;
	      tmp = tmp + i;
	      i = 0;
	      continue;
	    }
	  else 
	    {
	      i = 0;
	      if(isalpha(tmp[i]))
		{
		  while(isalpha(tmp[i]) || tmp[i] == '_') { i++; count++;}
		  if(i)
		    {
		      c = tmp[i]; tmp[i] = 0;
		      if(!strncmp(tmp, "EZ_", 3))
			EZ_TextInsertStringWithProperty(textW,tmp, ez_func);	     
		      else if(!strncmp(tmp, "int", 3) ||
			      !strncmp(tmp, "float", 5) ||
			      !strncmp(tmp, "char", 4) ||
			      !strncmp(tmp, "unsigned", 8) ||
			      !strncmp(tmp, "long", 4) ||
			      !strncmp(tmp, "short", 5) ||
			      !strncmp(tmp, "typedef", 7) ||
			      !strncmp(tmp, "struct", 5) ||
			      !strncmp(tmp, "union", 5) ||
			      !strncmp(tmp, "double", 5) ||
			      !strncmp(tmp, "register", 8) ||
			      !strncmp(tmp, "enum", 4) ||
			      !strncmp(tmp, "extern", 6) ||
			      !strncmp(tmp, "static", 6) ||
			      !strncmp(tmp, "void", 4))
			EZ_TextInsertStringWithProperty(textW,tmp, types); 
		      else if(!strncmp(tmp, "if", 2) ||
			      !strncmp(tmp, "else", 4) ||
			      !strncmp(tmp, "do", 2) ||
			      !strncmp(tmp, "while", 5) ||
			      !strncmp(tmp, "for", 3) ||
			      !strncmp(tmp, "return", 6) ||
			      !strncmp(tmp, "case", 4) ||
			      !strncmp(tmp, "switch", 6) ||
			      !strncmp(tmp, "break", 5) ||
			      !strncmp(tmp, "continue", 8) ||
			      !strncmp(tmp, "exit", 4) ||
			      !strncmp(tmp, "default", 7) ||
			      !strncmp(tmp, "goto", 4))
			EZ_TextInsertStringWithProperty(textW,tmp, control); 
		      else
			EZ_TextInsertStringWithProperty(textW,tmp, NULL); 

		      tmp[i] = c;
		      tmp = tmp+i;
		      i = 0;
		      continue;
		    }
		  continue;
		}
	      else
		{
		  while(! IS_WHITE_SPACE(tmp[i])) { i++; count++;}
		  if(i)
		    {
		      c = tmp[i]; tmp[i] = 0;
		      EZ_TextInsertStringWithProperty(textW,tmp, NULL);
		      tmp[i] = c;
		      tmp = tmp + i;
		      i = 0;
		    }
		  continue;
		}
	    }
	}
    }
}
/***************************************************************************************/		    
		  

      
      
