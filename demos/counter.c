/************************* Counter Widget *******************************
 *
 * To make your composite widget available to EZ_CreateWidget, you need
 *   
 *  1.  Need a creation routine and a int variable TYPE
 *       EZ_Widget *create (EZ_Widget *parent, char *iname, char *cname);
 *  
 *  2.  Register your widget by calling
 *       TYPE = EZ_RegisterCompositeWidget(iname, cname, create_routine)
 *
 *  After a widget has been registered, 
 *  use EZ_CreateWidget with type=TYPE to create a 
 *  widget.
 *
 *  If you want to support resources, you need also
 *  
 *    3.  a list of resource names and their value types,
 *
 *    4.  a configuration procedure specifies how the resource
 *        values will be used, and
 *    
 *    5.  Call EZ_RegisterResourcesAll(TYPE, nrsrcs, rsrcs);
 *
 *********************************************************************/
/*
 *
 *           This file implements a counter
 *
 *
 *********************************************************************/
#include "EZ.h"

#define COUNTER_INIT_VALUE   1025
#define COUNTER_INCREMENT    1026

int  EZ_RegisterCounterWidget(char *iname, char *cname);
int  ez_GetCounter (EZ_Widget *widget);
void ez_SetCounter (EZ_Widget *widget, int value);

/*********************************************************************/
static EZ_Widget *CounterCreate(EZ_Widget *parent, char *iname, char *cname);
/*********************************************************************/
typedef struct counter_ {
  EZ_Widget   *frame,  *label;
  EZ_Widget   *btnL,  *btnR;
  int         value;
  int         increment1, increment2;
} ez_counter;
/*********************************************************************/
int  ez_GetCounter(EZ_Widget *widget)
{
  /* return counter value */
  ez_counter *ptr = (ez_counter *)EZ_GetWidgetPtrData(widget);
  return(ptr->value);
}
/*********************************************************************/
void  ez_SetCounter(EZ_Widget *widget, int value)
{
  /* set counter value and update display */
  ez_counter *ptr = (ez_counter *)EZ_GetWidgetPtrData(widget);
  if(ptr->value != value)
    {
      char str[64];
      ptr->value = value;
      sprintf(str, "%d", value);
      EZ_ConfigureWidget(ptr->label, EZ_LABEL_STRING, str, 0);
      if(EZ_WidgetIsViewable(ptr->label)) EZ_DrawWidget(ptr->label);
    }
}
/*********************************************************************/
static void freeCounter(EZ_Widget *w, void *data) { free(data);}
/*********************************************************************/
static void btnCB(EZ_Widget *widget, void *data)
{
  ez_counter *ptr = (ez_counter *)data;
  int direction = EZ_GetScrollButtonScrollCount(widget);
  int incr;
  if(abs(direction) >= 10) incr = ptr->increment2;
  else incr = ptr->increment1;
  if(direction < 0) incr = -incr;
  ez_SetCounter(widget, ptr->value + incr);  
}
/*********************************************************************/
static EZ_Widget *CounterCreate(EZ_Widget *parent, char *iname, char *cname)
{
  ez_counter *record = (ez_counter *)malloc(sizeof(ez_counter));
  memset(record, 0, sizeof(ez_counter));

  record->value = 0; record->increment1 = 1;  record->increment2 = 10;
  
  record->frame = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,        parent, 
                                     iname, cname,
                                     EZ_PADX,                0,
                                     EZ_PADY,                0,
                                     EZ_IPADX,               0,
                                     EZ_SIZE,                0,0,
                                     EZ_FILL_MODE,           EZ_FILL_BOTH,
                                     EZ_DESTROY_CALLBACK,    freeCounter, record,
                                     EZ_CLIENT_PTR_DATA,     record,
                                     0);
  record->btnL =     EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON,record->frame,
                                     EZ_SCROLL_TYPE,         EZ_LEFT,
                                     EZ_CLIENT_PTR_DATA,     record,
                                     EZ_PADX,                2,
                                     EZ_BORDER_WIDTH,        0,
                                     EZ_BORDER_TYPE,         EZ_BORDER_RAISED,
                                     EZ_CALLBACK,            btnCB, record,
                                     0);
  record->label = EZ_CreateWidgetXrm(EZ_WIDGET_LABEL,        record->frame,
                                     "counter", "Counter",
                                     EZ_WIDTH,               80,
                                     EZ_PADX,                0,
                                     EZ_PADY,                0,
                                     EZ_LABEL_POSITION,      EZ_CENTER,
                                     EZ_CLIENT_PTR_DATA,     record,
                                     EZ_PROPAGATE,           False,
                                     EZ_BORDER_WIDTH,        1,
                                     EZ_BORDER_STYLE,        EZ_BORDER_SUNKEN,
                                     0);
  record->btnR =    EZ_CreateWidget(EZ_WIDGET_SCROLL_BUTTON, record->frame,
                                    EZ_SCROLL_TYPE,          EZ_RIGHT,
                                    EZ_CLIENT_PTR_DATA,      record,
                                    EZ_PADX,                 2,
                                    EZ_CLIENT_INT_DATA,      1,   
                                    EZ_BORDER_WIDTH,         0,
                                    EZ_BORDER_TYPE,          EZ_BORDER_RAISED,
                                    EZ_CALLBACK,            btnCB, record,
                                    0);  
  
  return(record->frame);
}

/*********************************************************************
 *
 *   Resource Specification. We have two resources,
 *   intial_value and  increment.
 */
static EZ_ResourceSpec counterResources[] =
{
  {COUNTER_INIT_VALUE,  "initValue", "InitValue", "int"},
  {COUNTER_INCREMENT,   "increment", "Increment", "int, int"},
};

/*********************************************************************/
static int counterConfigure(EZ_Widget *widget, int opt, EZ_Value *values)
{
  switch(opt)
   {
    case COUNTER_INIT_VALUE:
      {
        int i = values[0].value.i;
        ez_SetCounter(widget, i);
      }
     break;
    case COUNTER_INCREMENT:
      {
        int i1 = values[0].value.i;
        int i2 = values[1].value.i;
        ez_counter *ptr = (ez_counter *)EZ_GetWidgetPtrData(widget);
        ptr->increment1 = i1;
        ptr->increment2 = i2;
      }
     break;
   default:
     break;
   }
}
/*********************************************************************/

int  EZ_RegisterCounterWidget(char *iname, char *cname)
{
  int type = EZ_RegisterCompositeWidget(iname?iname:"counter", 
					cname?cname:"Counter",
					CounterCreate);
  EZ_RegisterResourcesAll(type, 2, counterResources, counterConfigure);

  return(type);
}
/*********************************************************************/

main(int ac, char **av)
{
  EZ_Widget *frame, *counter;
  int  counterType;

  EZ_Initialize(ac,av,0);
  counterType = EZ_RegisterCounterWidget("counter", "Counter");

  frame =       EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,
                                EZ_ORIENTATION, EZ_VERTICAL,
                                EZ_FILL_MODE, EZ_FILL_BOTH,
                                0);
  counter  =    EZ_CreateWidgetXrm(counterType,       frame,
                                   "counter",         "Counter",
                                   COUNTER_INIT_VALUE, 5,
                                   EZ_BORDER_WIDTH,    2,
                                   EZ_BORDER_STYLE,    EZ_BORDER_RIDGE,
                                   0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*********************************************************************/
