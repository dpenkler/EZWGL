/***********************************************************************
 *
 * This file implements a simple button box. The button box is a (labled)
 * frame with a bunch of frame-button paires in it. 
 *
 *  !! One should not add callbacks to the buttons, instead add callbacks
 *  !! to the button box, use 'getBboxCurrentButton' to figure out
 *  !! the current active button and do things from there.
 *
 *  !! the geometry layout of the bbox can be configured just as any primitive
 *  !! widget.  When using the GridBag geom manager, set the grid geometry
 *  !! for the parent of the button returned by 'newBboxButton'.
 *  
 */

#include "EZ.h"

/***********************************************************************/
EZ_Widget *createButtonBox MY_ANSIARGS((EZ_Widget *par, char *lab));
EZ_Widget *getBboxCurrentButton MY_ANSIARGS((EZ_Widget *btnBox));
EZ_Widget *newBboxButton  MY_ANSIARGS(( EZ_Widget *bbox, int active));
/***********************************************************************/
static void  bboxBtnEHandler MY_ANSIARGS((EZ_Widget *w, void *data, int et, XEvent *xevent));
static void  bboxBtnCallback MY_ANSIARGS((EZ_Widget *widget, void *data));
/***********************************************************************
 *
 *  return a bbox widget. A frame
 */
EZ_Widget *createButtonBox(EZ_Widget *parent,char *label)
{
  EZ_Widget *bbox;

  bbox = EZ_CreateWidget(EZ_WIDGET_FRAME,  parent, NULL);
  return(bbox);
}
/***********************************************************************
 *
 * Add a frame-button pair to bbox and return the button. One has to
 * call EZ_ConfigureWidget on the returned widget to set the attributes
 * for the button.  If GridBag geom manager is used on 'bbox', you
 * need to set the grid constrains on the parent of the returned value.
 */
EZ_Widget *newBboxButton(EZ_Widget *bbox, int active)
{
  EZ_Widget *frame, *btn = NULL;

  if(bbox)
    {
      frame = EZ_CreateWidget(EZ_WIDGET_FRAME, bbox, 
			      EZ_PADX, 4, EZ_PADY, 4,
			      EZ_BORDER_WIDTH, 1,
			      EZ_BORDER_STYLE, active == 0? EZ_BORDER_NONE: EZ_BORDER_DOWN,
			      NULL);
      btn = EZ_CreateWidget(EZ_WIDGET_NORMAL_BUTTON, frame, NULL);
      EZ_AddEventHandler(btn, bboxBtnEHandler, bbox, 0);
      EZ_AddWidgetCallBack(btn, EZ_CALLBACK, bboxBtnCallback, bbox, -1);
      EZ_SetWidgetPtrData(frame, btn);
      if(active)
	{
	  EZ_Widget *cframe = (EZ_Widget *)EZ_GetWidgetPtrData(bbox);
	  if(cframe)  EZ_ConfigureWidget(cframe, EZ_BORDER_STYLE, EZ_BORDER_NONE, NULL);
	  EZ_SetWidgetPtrData(bbox, frame);
	}
      return(btn);
    }
  return(NULL);
}
/***********************************************************************
 *
 * Return the currnet active button on the button box or NULL if there
 * is no button is selected. Selected button is visually displayed
 * in a sunken frame. (that is what the frames are for).
 */
EZ_Widget *getBboxCurrentButton( EZ_Widget *bbox)
{
  if(bbox)
    {
      EZ_Widget *frame = (EZ_Widget *)EZ_GetWidgetPtrData(bbox);
      if(frame) return((EZ_Widget *) EZ_GetWidgetPtrData(frame));
    }
  return(NULL);
}
/***********************************************************************
 *
 * Callback for the buttons. It sets the current active button 
 * to 'btn' and invoke the callbacks of the parent bbox.
 */
static void bboxBtnCallback(EZ_Widget *btn, void *data)
{
  EZ_Widget *parent = EZ_GetParentWidget(btn);
  EZ_Widget *bbox = (EZ_Widget *)data;
  EZ_Widget *cframe = (EZ_Widget *)EZ_GetWidgetPtrData(bbox);
  
  if(cframe != parent)
    {
      if(cframe) EZ_ConfigureWidget(cframe, EZ_BORDER_STYLE, EZ_BORDER_NONE, NULL);
      EZ_ConfigureWidget(parent, EZ_BORDER_STYLE, EZ_BORDER_DOWN, NULL);
      EZ_SetWidgetPtrData(bbox, parent);
    }
  EZ_CallWidgetCallbacks(bbox);
}
/***********************************************************************
 *
 * Event handler, when button1 is press on a button, it sets the current
 * active button to the press button.
 */
static void bboxBtnEHandler(EZ_Widget *btn, void *data, int etype, XEvent *xevent)
{
  if(xevent->type == ButtonPress && xevent->xbutton.button == EZ_Btn1)
    {
      EZ_Widget *parent = EZ_GetParentWidget(btn);
      EZ_Widget *bbox = (EZ_Widget *)data;
      EZ_Widget *cframe = (EZ_Widget *)EZ_GetWidgetPtrData(bbox);
      if(cframe != parent)
	{
	  if(cframe) EZ_ConfigureWidget(cframe, EZ_BORDER_STYLE, EZ_BORDER_NONE, NULL);
	  EZ_ConfigureWidget(parent, EZ_BORDER_STYLE, EZ_BORDER_DOWN, NULL);
	  EZ_SetWidgetPtrData(bbox, parent);
	}
    }
}
/***********************************************************************/


/***********************************************************************
 *
 *  Test code
 */

static void bboxTestCb(EZ_Widget *widget, void *data)
{
  EZ_Widget *btn = getBboxCurrentButton(widget);
  if(btn)
    {
      int which = EZ_GetWidgetIntData(btn);
      if(which >= 0)
	{
	  EZ_Widget *entry = (EZ_Widget *)data;
	  printf("You've pressed Button %d: ", which);
	  printf("EntryString is [%s]\n", EZ_GetEntryString(entry));
	}
      else {  EZ_Shutdown();   exit(0);}
    }
  else printf("====== No active button !!!\n");
}

static void entryCb(EZ_Widget *widget, void *data)
{
  EZ_Widget *bbox = (EZ_Widget *)data;
  if(bbox) EZ_CallWidgetCallbacks(bbox);  
}

int main(int ac, char **av)
{
  EZ_Widget *frame, *entry, *bbox, *btn1, *btn2, *btn3, *quit;

  EZ_Initialize(ac,av,0);
  EZ_SetKbdHighlightColor("red");

  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
			  EZ_ORIENTATION,  EZ_VERTICAL,
			  EZ_FILL_MODE,    EZ_FILL_BOTH,
			  EZ_LABEL_STRING, "Button Box Test", NULL);
  entry = EZ_CreateWidget(EZ_WIDGET_ENTRY, frame,
			  EZ_ENTRY_STRING, "Hit the return key here",
			  NULL);
  
  bbox = createButtonBox(frame, "Button Box");
  EZ_ConfigureWidget(bbox,  EZ_CALLBACK, bboxTestCb, entry, 0);
  
  EZ_AddWidgetCallBack(entry, EZ_CALLBACK, entryCb, bbox, 0);

  btn1 = newBboxButton(bbox, 0);
  EZ_ConfigureWidget(btn1, 
		     EZ_LABEL_STRING, "Button 1",
		     EZ_CLIENT_INT_DATA, 1,
		     NULL);

  btn2 = newBboxButton(bbox, 1);
  EZ_ConfigureWidget(btn2, 
		     EZ_LABEL_STRING, "Button 2",
		     EZ_CLIENT_INT_DATA, 2,
		     NULL);

  btn3 = newBboxButton(bbox, 1);
  EZ_ConfigureWidget(btn3, 
		     EZ_LABEL_STRING, "Button 3",
		     EZ_CLIENT_INT_DATA, 3,
		     NULL);

  quit = newBboxButton(bbox, 0);
  EZ_ConfigureWidget(quit, 
		     EZ_LABEL_STRING, "Quit",
		     EZ_CLIENT_INT_DATA, -1,
		     NULL);

  EZ_DisplayWidget(bbox);
  EZ_EventMainLoop();
}

/***********************************************************************/
