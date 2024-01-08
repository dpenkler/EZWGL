/* test gradient bar widget */

#include "EZ.h"

/* callback of the first gbar, it sets the second gradient bar
 */
static void callback(EZ_Widget *gbar, void *pdata)
{
  int     r,g,b;
  char    str[16];

  EZ_GetGradientBarColor(gbar, &r, &g, &b);
  sprintf(str, "#%02x%02x%02x", r, g,b);
  EZ_ConfigureWidget(pdata, EZ_BAR_COLOR_N, 1, str, 0);
  EZ_CallWidgetCallbacks(pdata);
}

/* motion callback for the first gbar, if set
 * it will be called instead of the callback when pointer
 * moves with button1 pressed.
 */
static void nullCallback(EZ_Widget *gbar, void *pdata) { }


/* callback of the Second gbar, it sets the background color of
 * the label to the selected color 
 */
static void callback1(EZ_Widget *gbar, void *pdata)
{
  int       r,g,b;
  char     str[16], str1[16];

  EZ_GetGradientBarColor(gbar, &r, &g, &b);
  sprintf(str, "#%02x%02x%02x", r, g,b);
  sprintf(str1, "#%02x%02x%02x", 255-r, 255-g, 255-b);
  EZ_ConfigureWidget(pdata, EZ_BACKGROUND, str, 
                     EZ_FOREGROUND, str1,
                     EZ_LABEL_STRING, str, 0);
}


int main(int ac, char **av)
{
  EZ_Widget *frame, *gbar, *gbar1, *label;
  EZ_Initialize(ac, av, 0);
  frame  = EZ_CreateWidget(EZ_WIDGET_FRAME,    NULL,
                           EZ_FILL_MODE,       EZ_FILL_BOTH,
                           EZ_ORIENTATION,     EZ_VERTICAL,
                           EZ_SIZE,            300, 140,
                           0);

  gbar = EZ_CreateWidget(EZ_WIDGET_GRADIENT_BAR,    frame,
                         EZ_BORDER_WIDTH,           2,
                         EZ_SLIDER_STYLE,           1, /* 0,1,2,3,4 */
                         /* EZ_MOTION_CALLBACK, nullCallback, NULL,*/
                         EZ_BAR_COLOR_N, 0, "green",      /*gradient colors */
                         EZ_BAR_COLOR_N, 1, "yellow",
                         EZ_BAR_COLOR_N, 2, "red",
                         EZ_BAR_COLOR_N, 3, "magenta",
                         EZ_BAR_COLOR_N, 4, "blue",
                         EZ_BAR_COLOR_N, 5, "cyan",
                         EZ_BAR_COLOR_N, 6, "white",
                         0);

  gbar1 = EZ_CreateWidget(EZ_WIDGET_GRADIENT_BAR,  frame,
                          EZ_BORDER_WIDTH,         2,
                          EZ_SLIDER_STYLE,         1,
                          EZ_BAR_COLOR_N, 0,       "black",
                          0);

  label = EZ_CreateWidget(EZ_WIDGET_LABEL, frame,
                          EZ_BORDER_WIDTH, 2,
                          EZ_BORDER_TYPE,  EZ_BORDER_SUNKEN,
                          EZ_SIZE,         120,64,
                          0);

  EZ_AddWidgetCallBack(gbar,  EZ_CALLBACK, callback, gbar1, 0);
  EZ_AddWidgetCallBack(gbar1, EZ_CALLBACK, callback1, label, 0);

  EZ_DisplayWidget(frame);
  EZ_CallWidgetCallbacks(gbar);
  EZ_EventMainLoop();
}
/*********************************************************************/
