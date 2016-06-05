/****************************************************************
 *
 *  To make multiple line selections in a FancyListbox.
 *  Hold the control key while press the left mouse buttn
 *  to make a multi-line selections.
 *
 */
#include "EZ.h"
EXTERN  void   EZ_WidgetDispatchEvent MY_ANSIARGS((XEvent *event));

typedef struct flbData_ {
  int   cnt;
  int   indices[100];
} flbData;

flbData testData;

void flbEHandler(EZ_Widget *flb, void *data, int et, XEvent *xev)
{
  if(xev->type == ButtonPress && xev->xbutton.button == Button1)
    {
      flbData *record = (flbData *)data;
      int modifiers = xev->xbutton.state;

      if(modifiers & ControlMask) /* control key is pressed */
	{
	  int row, col;
	  /* this is a bit tricky, we first dispatch
	   * the event to its default handler so the
	   * flistbox update its currnet selection
	   */
	  EZ_WidgetDispatchEvent(xev);
	  /* then we record the selection in testData
	   * and highlight the selected item
	   */
	  EZ_GetFancyListBoxSelectionIdx(flb, &row, &col);
	  if(row >= 0)
	    {
	      EZ_Item *item = EZ_GetFancyListBoxItemUsingIdx(flb, row, col);
	      if(item)
		{
		  int i;
		  for(i = 0; i < record->cnt; i++)
		    {
		      if(record->indices[i] == row) break;
		    }
		  if(i == record->cnt)
		    {
		      record->indices[record->cnt++] = row;
		      EZ_ConfigureItem(item, EZ_BACKGROUND, "yellow", NULL);
		    }
		}
	    }
	  /* have to remove this event, otherwise, it will be
	   * dispatched again to its default event handler !!!
	   */
	  xev->type = 0;   
	}
    }
}

void btnCallback(EZ_Widget *btn, void *data)
{
  EZ_Widget *flb = (EZ_Widget *)data;
  flbData   *sels = (flbData *) EZ_GetWidgetPtrData(flb);
  int       i;

  if( sels->cnt > 0)
    {
      for(i = 0; i < sels->cnt; i++)
	{
	  EZ_Item *item = EZ_GetFancyListBoxItemUsingIdx(flb, sels->indices[i], 0);
	  if(item)
	    {
	      char *label; int length;
	      EZ_GetLabelItemStringInfo(item, &label, &length);
	      if(length > 0)	printf("[%s]\n", label);
	      EZ_ConfigureItem(item, EZ_BACKGROUND, NULL,
			       NULL);
	    }
	}
      sels->cnt = 0;
    }
  else  /* just get the current selection */
    {
      EZ_Item **items = EZ_GetFancyListBoxSelection(flb);
      if(items)
	{
	  char *label; int length;
	  EZ_GetLabelItemStringInfo(items[0], &label, &length);
	  if(length > 0) printf("[%s]\n", label);
	}
    }
}

main(int ac, char **av)
{
  char       str[256];
  EZ_Item    *items[1];
  EZ_Widget  *frame, *flistbox, *btn;
  int i;
  
  EZ_Initialize(ac, av, 0);
  testData.cnt = 0;

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME,  NULL,
			  EZ_WIDTH,         200,
			  EZ_HEIGHT,        300,
			  EZ_FILL_MODE,     EZ_FILL_BOTH,
			  EZ_ORIENTATION,   EZ_VERTICAL,
			  NULL);
			  
  flistbox = EZ_CreateWidget(EZ_WIDGET_FANCY_LIST_BOX,  frame,
			     EZ_FANCY_LIST_BOX_COLUMNS, 1,
			     EZ_IPADY,                  2,
			     EZ_FANCY_LIST_BOX_TYPE,    0,  /* item mode */
			     EZ_CLIENT_PTR_DATA,        &testData,
			     EZ_EVENT_HANDLER,          flbEHandler, &testData,
			     EZ_SELECTION_BACKGROUND,   "yellow",
			     0);

  btn =  EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON,  frame,
			 EZ_LABEL_STRING,          "test",
			 EZ_HEIGHT,                0,
			 EZ_WIDTH,                 0,
			 EZ_CALLBACK,              btnCallback, flistbox,
			 NULL);
  
  for(i = 0; i < 30; i++)
    {
      sprintf(str, " test item %d ", i);
      items[0] = EZ_CreateLabelItem(str, NULL);
      EZ_FancyListBoxInsertRow(flistbox, items, 1, -1); /* -1: at the end */
    }
  EZ_DisplayWidget(flistbox);
  EZ_EventMainLoop();
}




