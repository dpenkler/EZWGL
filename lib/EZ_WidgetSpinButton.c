/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*****************************************************************
 ***                                                           ***
 ***              Spin Button                                 ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_SPIN_BUTTON_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */

/*********************************************************************/
EZ_Widget  *EZ_CreateSpinButton   MY_ANSIARGS((EZ_Widget *parent));
char       *EZ_GetSpinButtonValue MY_ANSIARGS((EZ_Widget *spin));
void        EZ_GetSpinButtonComponents MY_ANSIARGS((EZ_Widget *spin,
                                                    EZ_Widget **entry,
                                                    EZ_Widget **sbtn));
void EZ_SetupSpinButton  MY_ANSIARGS((EZ_Widget *spin, char *value,
                                      char *(func) MY_ANSIARGS((int, int, void *)),
                                      void *cdata));
/*********************************************************************/
static void btnCB MY_ANSIARGS((EZ_Widget *btn, void *data));
/*********************************************************************/
EZ_Widget  *EZ_CreateSpinButton(parent)
     EZ_Widget  *parent;     /* parent widget  */
{
  EZ_Widget  *frame, *entry, *sbtn;

  frame = EZ_CreateNewWidget(parent);
  /* use the default widget handle */
  EZ_SetWidgetTypeAndNames(frame,EZ_WIDGET_SPIN_BUTTON);
  EZ_WidgetPadX(frame) = 0;
  EZ_WidgetPadY(frame) = 0;
  EZ_WidgetSepX(frame) = 2;
  EZ_WidgetSepY(frame) = 0;
  EZ_WidgetBorderWidth(frame) = 0;
  EZ_WidgetStacking(frame) = EZ_HORIZONTAL;
  EZ_WidgetFillMode(frame) = EZ_FILL_HORIZONTALLY;
  EZ_SetWidgetHeightSetFlag(frame);

  entry = EZ_CreateEntry(frame, NULL);
  EZ_WidgetGroupLeader(entry) = frame;
  EZ_WidgetPadX(entry) = 0;
  EZ_WidgetPadY(entry) = 0;
  EZ_EntryLineLength(entry) = 10;

  sbtn = EZ_CreateScrollButton(frame, EZ_UP_DOWN);
  EZ_WidgetGroupLeader(sbtn) = frame;
  EZ_WidgetPadX(sbtn) = 0;
  EZ_WidgetPadY(sbtn) = 0;
  EZ_SetWidgetWidthSetFlag(sbtn);

  EZ_EntrySpin(entry) = sbtn;

  EZ_SpinButtonEntry(frame) = entry;
  EZ_SpinButtonSButton(frame) = sbtn;
  EZ_SpinButtonCurrent(frame) = 0;
  EZ_SpinButtonCData(frame) = NULL;
  EZ_SpinButtonNextF(frame) = NULL;
  EZ_ScrollButtonSpin(sbtn) = frame;
  EZ_AddWidgetCallBackInternal(sbtn, EZ_CALLBACK,
                               (EZ_CallBack)btnCB, (void *)frame, 0);
  return(frame);
}
/*******************************************************************/
static void btnCB(EZ_Widget *btn, void *data)
{
  EZ_Widget *spin = (EZ_Widget *)data;
  int current = EZ_SpinButtonCurrent(spin);
  int direc = EZ_GetScrollButtonScrollCount(btn);
  if(EZ_SpinButtonNextF(spin))
    {
      int  nv = current + (direc > 0? 1 : -1);
      char *value = (EZ_SpinButtonNextF(spin))(current, nv, EZ_SpinButtonCData(spin));
      EZ_SpinButtonCurrent(spin) = nv;
      EZ_SetEntryString(EZ_SpinButtonEntry(spin), value);
    }
}
char *EZ_GetSpinButtonValue(spin) EZ_Widget *spin;
{
  if(spin && EZ_WidgetType(spin) == EZ_WIDGET_SPIN_BUTTON)
    {
      EZ_Widget *entry = EZ_SpinButtonEntry(spin);
      return(EZ_EntryData(entry));
    }
  return(NULL);
}
/******************************************************************/
void EZ_GetSpinButtonComponents(spin, entry, sbtn)
     EZ_Widget *spin, **entry, **sbtn;
{
  if(spin && EZ_WidgetType(spin) == EZ_WIDGET_SPIN_BUTTON)
    {
      if(entry) *entry = EZ_SpinButtonEntry(spin);
      if(sbtn)  *sbtn  = EZ_SpinButtonSButton(spin);
    }
}
/*******************************************************************/
void EZ_SetupSpinButton(spin, value, func, cdata)
     EZ_Widget *spin; char *value; void *cdata;
     char *(func) MY_ANSIARGS((int, int, void *));
{
  if(spin && EZ_WidgetType(spin) == EZ_WIDGET_SPIN_BUTTON)
    {
      EZ_SetEntryString(EZ_SpinButtonEntry(spin), value);
      EZ_SpinButtonNextF(spin) = func;
      EZ_SpinButtonCData(spin) = cdata;
    }
}
/*******************************************************************/
#undef _EZ_WIDGET_SPIN_BUTTON_C_
