/****************************************************************
 ***                                                          ***
 ***     notebook.c                                           ***
 ***          NoteBook widget demo                            ***
 ***                                                          ***
 ***************************************************************/
#include "EZ.h"
#include <X11/bitmaps/flagup>
#include "dog3.xpm"

void my_exit();
/*************************************************************************/
static void make_button_box(EZ_Widget *pp);
static void make_check_button_box(EZ_Widget *pp);
static void make_a_text_widget(EZ_Widget *pp);
static void make_author_info_widget(EZ_Widget *pp);

/*************************************************************************/
EZ_Widget *notebook, *page[20];
main(int ac, char **av)
{
  EZ_Widget *frame, *tmp;
  int       i;
  char      str[160];

  EZ_Initialize(ac,av,1);
  
  /* create a toplevel frame */
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,NULL,
                          EZ_FILL_MODE, EZ_FILL_BOTH, 0);

  /* create a notebook widget */
  /* and create 4 pages in it */
  notebook = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK, frame,
                             EZ_FILL_MODE, EZ_FILL_BOTH,
                             EZ_BORDER_TYPE, EZ_BORDER_UP,
                             EZ_BORDER_WIDTH, 2,
                             EZ_PADX, 10, EZ_PADY, 20,
                             EZ_IPADX, 10, EZ_IPADY, 10, 0);
  page[0] = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK_PAGE, notebook, 
                            EZ_LABEL_STRING, "Buttons", 
                            EZ_EXPAND, 1, EZ_BORDER_WIDTH, 2, 0);

  page[1] = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK_PAGE, notebook, 
                            EZ_EXPAND, 1, EZ_BORDER_WIDTH, 4,
                            EZ_FOREGROUND, "Red", EZ_BACKGROUND, "#007f6f",
                            EZ_FONT_NAME,  "-Adobe-Times-Bold-R-Normal--*-120-*-*-*-*-*-*",
                            0);
  {
    EZ_Widget *tmp = EZ_GetNoteBookPageTab(page[1]);
    if(tmp)
      {
	EZ_Widget *a = EZ_CreateWidget(EZ_WIDGET_NW_LABEL, tmp,
                                       EZ_BITMAP_FILE, "earth.xpm",0);

	a = EZ_CreateWidget(EZ_WIDGET_NW_LABEL, tmp,
                            EZ_LABEL_STRING, "This is a test", 0);
      }
  }
  page[2] = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK_PAGE, notebook, 
                            EZ_LABEL_STRING, "TextWidget",
                            EZ_BACKGROUND, "bisque2",
                            EZ_FILL_MODE, EZ_FILL_BOTH,
                            EZ_PIXMAP_DATA, dog_xpm, 0); /* a pixmap label */

  page[3] = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK_PAGE, notebook, 
                            EZ_LABEL_STRING,  "Author", 
                            EZ_UNDERLINE,    5, 0);

  /*
  for(i = 4; i < 20; i++)
    {
      char tstr[32]; sprintf(tstr, "Page %d", i);
      page[i] = EZ_CreateWidget(EZ_WIDGET_NOTEBOOK_PAGE, notebook, 
                                EZ_LABEL_STRING,  tstr, 0);
    }
    */                            
  
  /* now create a few things in page[1-4] */

  make_button_box(page[0]);
  make_check_button_box(page[1]);
  make_a_text_widget(page[2]);
  make_author_info_widget(page[3]);

  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/**********************************************************************
 *
 *  Make a button box.
 */
static void get_out();

static void make_button_box(EZ_Widget *parent)
{
  EZ_Widget *frame, *tmp;

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, parent,     /* a frame to hold five widgets    */
                          EZ_LABEL_STRING, "buttons",
                          EZ_ORIENTATION, EZ_VERTICAL,  /* children are stacked vertically */
                          0);
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                        EZ_LABEL_STRING, "A Normal Button",0);       /* two buttons */
  tmp = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame,
                        EZ_LABEL_STRING, "Another Button",
                        EZ_FOREGROUND, "#ffff00",    /* bg and fg colors       */
                        EZ_BACKGROUND, "#008f8f", 0);
}		     

/**********************************************************************
 *
 *   Create a check button box.
 */
static void make_check_button_box(EZ_Widget *parent)
{
  EZ_Widget *check, *tmp;
  int       i;
  char      str[256];

  check = EZ_CreateWidget(EZ_WIDGET_FRAME, parent,
                          EZ_LABEL_STRING, "check buttons",
                          EZ_ORIENTATION, EZ_VERTICAL,
                          0);  /* a labeled frame    */
  for(i = 0; i < 5; i++)
    {
      sprintf(str, "Check Button %d",i);
      tmp = EZ_CreateWidget(EZ_WIDGET_CHECK_BUTTON, check,
                            EZ_LABEL_STRING, str,
                            EZ_CHECK_BUTTON_ON_VALUE, i+1,
                            EZ_CHECK_BUTTON_OFF_VALUE, i*i+10,
                            EZ_CHECK_BUTTON_ON_OFF, (i == 1)? 1: 0,
                            0);
    }
}

static void make_a_text_widget(EZ_Widget *parent)
{
  EZ_Widget *textW;

  textW = EZ_CreateWidget(EZ_WIDGET_TEXT, parent, 0);
  EZ_TextLoadFile(textW,"notebook.c");
}
/***********************************************************************
 *
 *  A frame displying info about the author
 */
static char author_info[] ="Maorong Zou\n\
Department of Mathematics\n\
The University of Texas at Austin\n\
Austin, TX 78712\n\n\
email:  mzou@math.utexas.edu";

static void make_author_info_widget(EZ_Widget *parent)
{
  EZ_Widget *author, *tmp;
  author = EZ_CreateWidget(EZ_WIDGET_FRAME, parent, 
                           EZ_LABEL_STRING, "Author info",
                           EZ_ORIENTATION, EZ_VERTICAL, 0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, author,
                        EZ_LABEL_STRING, "A Photo",
                        EZ_IMAGE_FILE, "author.gif", 0);
  tmp = EZ_CreateWidget(EZ_WIDGET_LABEL, author,
                        EZ_LABEL_STRING,author_info,
                        EZ_TEXT_LINE_LENGTH,50, 
                        0);
}
