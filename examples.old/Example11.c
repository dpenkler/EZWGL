/************************** Example 11 ***********************/
#include "EZ.h"

static char *xpms[] = { /* these are from /usr/include/X11/pixmaps */
"NeXT.xpm",   "doso.xpm",  "file_tar.xpm",  "question.xpm",
"xfm_file_small.xpm",  "Plaid.xpm",  "doss.xpm","file_text.xpm",
"rbomb.xpm",  "xfm_files.xpm",  "app.xpm",  "tank.xpm",
"drafting.xpm",  "fils.xpm",  "mini-exp.xpm",  "rcalc.xpm","xfm_font.xpm",
"app_compress.xpm",  "dragon.xpm",  "find.xpm",  "mini-eyes.xpm",  "recycle.xpm",
"xfm_gif.xpm",  "app_find.xpm",  "drawing.xpm",  "flight_sim.xpm",
"rlogin.xpm",  "xfm_h.xpm",  "app_grep.xpm",  "edit.xpm",  "floppy.xpm",  
"rterm.xpm",  "xfm_icon.xpm",  "app_make.xpm",  "editor.xpm",  "floppy3.xpm",
"xfm_info.xpm",  "app_tar.xpm",  "editres.xpm",  "flow_chart.xpm",
"stopsign.xpm",  "xfm_make.xpm",  "app_taz.xpm",  "emacs.xpm",
"folder2.xpm",   NULL,
};

static EZ_Widget *labelWidget;

static void itemCallBack(EZ_Item *item, void *data)
{
  char str[123], *ptr = xpms[EZ_GetItemIntData(item)];
  sprintf(str, "You've double clicked on '%s'", ptr);
  EZ_ConfigureWidget(labelWidget, EZ_LABEL_STRING, str, 0);
}

int main(int argc, char **argv)
{
  EZ_Widget *wa, *listbox;
  EZ_Item  *items[40];
  int i;

  EZ_Initialize(argc,argv,0); 

  
  for(i = 0; i < 32; i++)
    {
      items[i] = EZ_CreateLabelItem(xpms[i], NULL);
      EZ_ConfigureItem(items[i], EZ_PIXMAP_FILE, xpms[i],
		       EZ_ORIENTATION, EZ_VERTICAL,
		       EZ_CALLBACK, itemCallBack, NULL,
		       EZ_BORDER_TYPE,
		       (i < 8 ? EZ_BORDER_RAISED: (i < 16 ? EZ_BORDER_SUNKEN :
						   (i < 24? EZ_BORDER_GROOVE :
						    EZ_BORDER_RIDGE))),
		       EZ_BORDER_WIDTH, 3,
		       EZ_CLIENT_INT_DATA, i,  0);
    }

  wa = EZ_CreateWorkArea(NULL, 1, 0);
  EZ_ConfigureWidget(wa, EZ_WIDTH, 500, EZ_HEIGHT, 600,
		     EZ_SELECTION_BACKGROUND, "yellow",
		     EZ_SELECTION_FOREGROUND, "blue",
		     EZ_PADX, 10, EZ_PADY, 10,0);
  labelWidget = EZ_CreateLabel(wa, "No");
  EZ_ConfigureWidget(labelWidget, EZ_HEIGHT, 0, EZ_PROPAGATE, False, 		     
		     EZ_EXPAND, True,
		     EZ_BACKGROUND, "bisque2", EZ_FOREGROUND, "red",
		     EZ_LABEL_POSITION, EZ_LEFT, 
		     EZ_TEXT_LINE_LENGTH, 80,
		     EZ_BORDER_TYPE, EZ_BORDER_SUNKEN,
		     EZ_BORDER_WIDTH, 3,
		     0);

  listbox = EZ_CreateListBox(NULL, 1, 1);
  EZ_ConfigureWidget(listbox, EZ_HEIGHT, 300, EZ_WIDTH, 200, 0);
  EZ_SetListBoxItems(listbox, xpms, 40);
  items[32] = EZ_CreateWidgetItem(listbox);

  for(i = 0; i < 33; i++)
    EZ_WorkAreaInsertItem(wa, items[i]);

  EZ_DisplayWidget(wa);
  EZ_EventMainLoop();
}
/************************** Example 11 ***********************/

