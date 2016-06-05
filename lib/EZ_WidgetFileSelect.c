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
 ***              Widget File Selector                         ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_FILE_SELECT_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateFileSelector  MY_ANSIARGS((EZ_Widget *parent, char *pattern));
void             EZ_GetFileSelectorWidgetComponents  MY_ANSIARGS((EZ_Widget *widget,
							    EZ_Widget **pe, EZ_Widget **se,
							    EZ_Widget **dl, EZ_Widget **fl,
							    EZ_Widget **ok, EZ_Widget **ft,
							    EZ_Widget **cancel));
void             EZ_SetFileSelectorInitialPattern MY_ANSIARGS((EZ_Widget *widget, char *pattern));
char             *EZ_GetFileSelectorSelection MY_ANSIARGS((EZ_Widget *widget));

int              EZ_ProcessFileSelectorPattern MY_ANSIARGS((char **dirName, char *pattern));
int              EZ_CompareStr MY_ANSIARGS((CONST void *ptr1, CONST void *ptr2));
int              EZ_CountSetFilePatterns MY_ANSIARGS((char *pat, char **patterns));
int              EZ_IsGlobPattern MY_ANSIARGS((char *pat));
int              position_after_last_slash_before_glob_chars MY_ANSIARGS((char *str));
/*********************************************************************
 * 
 * Local functions.
 */
static void CancelCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void OkCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void FilterCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void ListboxAMotionCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void ListboxBMotionCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void ListboxACallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void ListboxBCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void EntryACallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void EntryBCallBack MY_ANSIARGS((EZ_Widget *widget, void *data));
static void EZ_FreeFileSelectData MY_ANSIARGS((EZ_Widget *widget));

static EZ_WidgetHandle EZ_FileSelectHandle =
{
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetUnknown,
  EZ_FreeFileSelectData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/

EZ_Widget  *EZ_CreateFileSelector(parent, ini_pattern)
     EZ_Widget  *parent;     /* parent widget    */
     char       *ini_pattern;
{
  EZ_Widget  *frame, *frameA, *frameB,*frameB1, *frameB2,*frameC, *frameD;
  EZ_Widget  *entryA, *entryB, *label, *tmp;
  EZ_Widget  *listboxA1, *listboxA2;
  EZ_Widget  *ok, *filter, *cancel;
  
  frame = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_FILE_SELECTOR] = &EZ_FileSelectHandle;

  EZ_SetWidgetTypeAndNames(frame,EZ_WIDGET_FILE_SELECTOR);
  EZ_FileSelectorCurrentDirectory(frame) = (char *)NULL;
  EZ_FileSelectorCurrentSelection(frame) = (char **)NULL;

  EZ_WidgetStacking(frame) = EZ_VERTICAL;
  EZ_WidgetFillMode(frame) = EZ_FILL_BOTH;
  EZ_WidgetWidthHint(frame) = 300;
  EZ_WidgetHeightHint(frame) = 400;
  EZ_SetWidgetWidthHintFlag(frame);
  EZ_SetWidgetHeightHintFlag(frame);
  EZ_WidgetPadX(frame) = 2;
  EZ_WidgetPadY(frame) = 4;
  EZ_WidgetSepY(frame) = 4;

  frameA = EZ_CreateFrame(frame, NULL);
  EZ_WidgetStacking(frameA) = EZ_VERTICAL;
  EZ_WidgetFillMode(frameA) = EZ_FILL_HORIZONTALLY;
  EZ_WidgetHeight(frameA) = 0;    /* will not strectched in the V dir */
  EZ_SetWidgetHeightSetFlag(frameA);
  EZ_WidgetPadX(frameA) = 0;
  EZ_WidgetPadY(frameA) = 3;
  EZ_WidgetSepY(frameA) = 3;
  EZ_SetWidgetCINames(frameA, NULL, "frameA");

  label = EZ_CreateLabel(frameA, "Filter:");
  EZ_WidgetLabelPosition(label) = EZ_LEFT;
  EZ_WidgetPadY(label) = 0;
  EZ_WidgetPadX(label) = 0;
  EZ_SetWidgetCINames(label, NULL, "filterLabel");

  entryA = EZ_CreateOptionalEntry(frameA, ini_pattern == NULL ? "*" : ini_pattern, 1, 1);
  EZ_SetWidgetCINames(entryA, NULL, "filterEntry");
  {
    EZ_Widget *tmp = EZ_OptEntryPopup(entryA);
    EZ_SetWidgetCINames(tmp, NULL, "popupA");
  }

  frameB = EZ_CreateFrame(frame, NULL);
  EZ_WidgetFillMode(frameB) = EZ_FILL_BOTH;
  EZ_WidgetPadX(frameB) = 0;
  EZ_WidgetPadY(frameB) = 0;
  EZ_WidgetSepX(frameB) = 3;
  EZ_WidgetSepY(frameB) = 0;
  EZ_SetWidgetCINames(frameB, NULL, "frameB");
  
  frameB1 = EZ_CreateFrame(frameB, NULL);
  EZ_WidgetStacking(frameB1) = EZ_VERTICAL;
  EZ_WidgetFillMode(frameB1) = EZ_FILL_BOTH;
  EZ_WidgetPadX(frameB1) = 0;
  EZ_WidgetPadY(frameB1) = 0;
  EZ_WidgetSepX(frameB1) = 0;
  EZ_WidgetSepY(frameB1) = 3;
  EZ_SetWidgetCINames(frameB1, NULL, "frameB1");

  tmp = EZ_CreatePaneHandle(frameB);
  frameB2 = EZ_CreateFrame(frameB, NULL);
  EZ_WidgetStacking(frameB2) = EZ_VERTICAL;
  EZ_WidgetFillMode(frameB2) = EZ_FILL_BOTH;
  EZ_WidgetPadX(frameB2) = 0;
  EZ_WidgetPadY(frameB2) = 0;
  EZ_WidgetSepX(frameB2) = 0;
  EZ_WidgetSepY(frameB2) = 3;

  label = EZ_CreateLabel(frameB1, "Directories:");
  EZ_WidgetLabelPosition(label) = EZ_LEFT;
  EZ_WidgetHeight(label) = 0;    /* will not strectched in the V dir */
  EZ_SetWidgetHeightSetFlag(label);
  EZ_WidgetPadX(label) = 0;
  EZ_WidgetPadY(label) = 0;
  EZ_SetWidgetCINames(label, NULL, "directoryLabel");

  listboxA1 = EZ_CreateListBox(frameB1, 1, 1);
  EZ_SetWidgetCINames(listboxA1, NULL, "directoryListBox");

  label = EZ_CreateLabel(frameB2, "Files:");
  EZ_WidgetLabelPosition(label) = EZ_LEFT;
  EZ_WidgetHeight(label) = 0;    /* will not strectched in the V dir */
  EZ_SetWidgetHeightSetFlag(label);
  EZ_WidgetPadY(label) = 0;
  EZ_SetWidgetCINames(label, NULL, "fileLabel");

  listboxA2 = EZ_CreateListBox(frameB2, 1, 1);
  EZ_SetWidgetCINames(listboxA2, NULL, "fileListBox");
  
  frameC = EZ_CreateFrame(frame, NULL);  
  EZ_WidgetStacking(frameC) = EZ_VERTICAL;
  EZ_WidgetFillMode(frameC) = EZ_FILL_HORIZONTALLY;
  EZ_WidgetPadX(frameC) = 0;
  EZ_WidgetSepY(frameC) = 3;
  EZ_WidgetHeight(frameC) = 0;    /* will not strectched in the V dir */
  EZ_SetWidgetHeightSetFlag(frameC);
  EZ_SetWidgetCINames(frameC, NULL, "frameC");

  label = EZ_CreateLabel(frameC, "Selection:");
  EZ_WidgetLabelPosition(label) = EZ_LEFT;
  EZ_WidgetPadY(label) = 0;
  EZ_WidgetPadX(label) = 0;
  EZ_SetWidgetCINames(label, NULL, "selectionLabel");

  entryB = EZ_CreateOptionalEntry(frameC, NULL, 1, 1);
  EZ_SetWidgetCINames(entryB, NULL, "selectionEntry");
  {
    EZ_Widget *tmp = EZ_OptEntryPopup(entryB);
    EZ_SetWidgetCINames(tmp, NULL, "popupB");
  }

  frameD =  EZ_CreateFrame(frame, NULL);
  EZ_WidgetFillMode(frameD) = EZ_FILL_HORIZONTALLY;
  EZ_WidgetSepX(frameD) = 10;
  EZ_WidgetHeight(frameD) = 0;    /* will not strectched in the V dir */
  EZ_SetWidgetHeightSetFlag(frameD);
  EZ_SetWidgetCINames(frameD, NULL, "frameD");

  ok = EZ_CreateButton(frameD, "Ok", 0);
  EZ_SetWidgetCINames(ok, NULL, "okButton");
  filter = EZ_CreateButton(frameD, "Filter", 0);  
  EZ_SetWidgetCINames(filter, NULL, "filterButton");
  cancel = EZ_CreateButton(frameD, "Hide", 0);  
  EZ_SetWidgetCINames(cancel, NULL, "hideButton");

  EZ_ButtonFSB(ok)     = frame;
  EZ_ButtonFSB(cancel) = frame;
  EZ_ButtonFSB(filter) = frame;
  EZ_AddWidgetCallBackInternal(ok,  EZ_CALLBACK, (EZ_CallBack)OkCallBack, NULL,0);
  EZ_AddWidgetCallBackInternal(cancel, EZ_CALLBACK, (EZ_CallBack)CancelCallBack,NULL,0);
  EZ_AddWidgetCallBackInternal(filter, EZ_CALLBACK, (EZ_CallBack)FilterCallBack,NULL,0);
  {
    EZ_Widget *rentryA, *rentryB;
    EZ_Widget *ilistboxA1, *ilistboxA2;

    rentryA =  EZ_OptEntryEntry(entryA);
    rentryB =  EZ_OptEntryEntry(entryB);
    ilistboxA1 = EZ_ListBoxIListBox(listboxA1);
    ilistboxA2 = EZ_ListBoxIListBox(listboxA2);
    
    EZ_AddWidgetCallBackInternal(ilistboxA1, EZ_MOTION_CALLBACK,(EZ_CallBack)ListboxAMotionCallBack,NULL,0);
    EZ_AddWidgetCallBackInternal(ilistboxA2, EZ_MOTION_CALLBACK,(EZ_CallBack)ListboxBMotionCallBack,NULL,0);
    EZ_AddWidgetCallBackInternal(ilistboxA1, EZ_CALLBACK, (EZ_CallBack) ListboxACallBack, NULL,0);
    EZ_AddWidgetCallBackInternal(ilistboxA2, EZ_CALLBACK, (EZ_CallBack) ListboxBCallBack, NULL,0);
    EZ_AddWidgetCallBackInternal(rentryA,  EZ_CALLBACK,(EZ_CallBack) EntryACallBack, NULL,0);
    EZ_AddWidgetCallBackInternal(rentryB,  EZ_CALLBACK,(EZ_CallBack) EntryBCallBack, NULL,0);

    EZ_FileSelectorPatternEntry(frame)  = entryA;
    EZ_FileSelectorSelectionEntry(frame) = entryB;
    EZ_FileSelectorDirListbox(frame)     = listboxA1;
    EZ_FileSelectorFileListbox(frame)    = listboxA2;
    EZ_FileSelectorOkButton(frame)       = ok;
    EZ_FileSelectorFilterButton(frame)   = filter;
    EZ_FileSelectorCancelButton(frame)   = cancel;

    /* ptr to the current selection */
    EZ_FileSelectorCurrentSelection(frame) = (char **) &(EZ_EntryData(rentryB));
    
    EZ_IListBoxFSL(ilistboxA1) = frame;
    EZ_IListBoxFSL(ilistboxA2) = frame;
    EZ_EntryFSE(rentryA) = frame;
    EZ_EntryFSE(rentryB) = frame;

    /* initialize the file selector */
    EntryACallBack(rentryA, NULL);
  }
  EZ_SetWidgetTreeGroupLeader(frame, frame);
  return(frame);
}

/*************************************************************
 *
 *  This is really ugly !!
 *  EZ_FileSelectorCurrentDirectory(fileSelect) always holds 
 *  the current absolute path !!!
 */

static void EntryACallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect, *ilistboxA, *listboxA, *ilistboxB, *listboxB;
  char pathname[1024], pattern[1024], cpy[1024], dir_pat[1024], tdir_pat[1024];
  char *patterns[16], **cdir;
  char *pat = EZ_GetEntryString(widget);
  int ii, npatterns, position, gposition = -1;
  char **dirItems = NULL, **fileItems = NULL;
  int   dirItemCnt = 0, fileItemCnt = 0;
  
  if(pat == NULL) return;
  (void)strcpy(cpy, pat);

  fileSelect = EZ_EntryFSE(widget);
  listboxA =  EZ_FileSelectorDirListbox(fileSelect);
  ilistboxA = EZ_ListBoxIListBox(listboxA);
  listboxB =  EZ_FileSelectorFileListbox(fileSelect);
  ilistboxB = EZ_ListBoxIListBox(listboxB);

  if( (npatterns = EZ_CountSetFilePatterns(cpy, patterns)) == 0) return;
  
  EZ_FreezeWidget(listboxA);  
  EZ_FreezeWidget(listboxB);  

  /* pointer grabing is removed. 6-1-98 */
  EZ_WaitCursor(fileSelect, EZ_GetCursor(EZ_C_WATCH));
  
  /* if more than one patterns are specified, find common root. 6-1-98 */
  if(npatterns > 1)
    {
      int i, j, k;
      char **cdir;

      for(i = 0; i < npatterns && i < 16; i++)
	{
	  (void) strcpy(pattern, patterns[i]);
	  cdir = (char **)&EZ_FileSelectorCurrentDirectory(fileSelect);
	  position = EZ_ProcessFileSelectorPattern(cdir, pattern);
	  if(i == 0) { strcpy(tdir_pat, *cdir); gposition = position;}
	  else
	    {
	      char *p, *q;
	      p = *cdir;
	      q = tdir_pat;
	      j = 0; k = 0;
	      while(*p && *q && *p == *q)
		{
		  j++;
		  if(*p == '/') k = j;
		  p++; q++;
		}
	      *q = 0;
	      gposition = k;
	    }
	}
    }

  for(ii = 0; ii < npatterns && ii < 16; ii++)
    {
      (void) strcpy(pattern, patterns[ii]);
      cdir = (char **)&EZ_FileSelectorCurrentDirectory(fileSelect);
      position = EZ_ProcessFileSelectorPattern(cdir, pattern);
      strcpy(tdir_pat, *cdir);
      if(gposition < 0) gposition = position;
      if(gposition > 0) (*cdir)[gposition] = 0; /* ????? */
      if(ii == 0) EZ_SetEntryString(widget, pattern);
      else 
	{
	  EZ_EntryEditEndOfInput(widget);
	  EZ_EntryEditInsertString(widget,"  ");
	  EZ_EntryEditInsertString(widget,pattern);
	  if(EZ_WidgetMapped(widget))  EZ_DrawWidget(widget);  
	}
      
      /* contents of dir listbox */
      {
	char *tmp = tdir_pat;
	char **dirlist;
	int  do_it; 
	(void)strcpy(pathname, tmp);
	(void)strcat(pathname, "*/");
	
	if(ii == 0) { do_it  = 1; (void)strcpy(dir_pat,pathname);}
	else if(strcmp(dir_pat, pathname)) do_it = 1;
	else do_it =0;
	
	if(do_it)
	  {
	    dirlist = (char **) EZ_GlobFileName(pathname);    
	    if(dirlist)
	      {
		int i, j = 0, k; 
		while(dirlist[j] != NULL) j++;
		k = dirItemCnt;
		dirItemCnt += j;
		if(dirItems == NULL)
		  dirItems = (char **)my_malloc((dirItemCnt+1)*sizeof(char *),_MISC_DATA_);
		else dirItems = (char **)my_realloc(dirItems, (dirItemCnt+1)*sizeof(char *),_MISC_DATA_);
		for(i = 0; i < j; i++)   dirItems[i+k] = dirlist[i];
		(void)free((char *)dirlist);  /* not allocated by my_malloc */
	      }
	  }
      }
      /* content of the files listbox */
      {
	char **dirlist = (char **) EZ_GlobFileName(pattern);
	if(dirlist)
	  {
	    int len = strlen(pattern);
	    int dir_only = (pattern[len-1] == '/') ? 1 : 0;
	    if(dirlist)
	      {
		struct stat finfo;
		int i, j = 0, ll; 
		while(dirlist[j] != NULL) j++;
		if(fileItems == NULL)
		  fileItems = (char **)my_malloc((fileItemCnt+j+2)*sizeof(char *),_MISC_DATA_);
		else fileItems=(char **)my_realloc(fileItems,(fileItemCnt+j+2)*sizeof(char *),_MISC_DATA_);
		
		for(ll = 0, i = 0; i < j; i++)
		  {
		    if( dir_only == 1 || ((stat (dirlist[i], &finfo) >= 0) && !S_ISDIR (finfo.st_mode)))
		      {
			fileItems[fileItemCnt + ll] = dirlist[i];
			ll++;
		      }
		    else (void)free((char *)dirlist[i]);  /* not allocated by my_malloc */
		  }
		fileItemCnt += ll;
		(void)free((char *)dirlist);  /* not allocated by my_malloc */
	      }
	  }
      }
    } /** end of loop */

  /* update the two listboxes */
  EZ_ClearListBox(ilistboxB);  /* file listbox */
  EZ_ClearIListBox(ilistboxA); /* dir listbox  */
  EZ_InsertIListBoxItem(ilistboxA, "..", 0);
  if(dirItems != NULL)
    {
      int i;
      if(dirItemCnt > 0)
	{
	  qsort(dirItems, dirItemCnt, sizeof(char *), EZ_CompareStr);
	  EZ_InsertIListBoxItem(ilistboxA, dirItems[0]+gposition, -1);
	  for(i = 1; i < dirItemCnt; i++)
	    {
	      if(strcmp(dirItems[i], dirItems[i-1]))
		EZ_InsertIListBoxItem(ilistboxA, dirItems[i]+gposition, -1);
	    }
	  for(i = 0; i < dirItemCnt; i++)
	    my_free(dirItems[i]);
	}
      my_free(dirItems);
    }
  if(fileItems != NULL)
    {
      int i;
      if(fileItemCnt > 0)
	{
	  qsort(fileItems, fileItemCnt, sizeof(char *), EZ_CompareStr);
	  EZ_InsertIListBoxItem(ilistboxB, fileItems[0]+gposition, -1);
	  for(i = 1; i < fileItemCnt; i++)
	    {
	      if(strcmp(fileItems[i], fileItems[i-1]))
		EZ_InsertIListBoxItem(ilistboxB, fileItems[i]+gposition, -1);
	    }
	  for(i = 0; i < fileItemCnt; i++)
	    my_free(fileItems[i]);
	}
      my_free(fileItems);
    }
  EZ_NormalCursor(fileSelect);
  EZ_UnFreezeWidget(listboxA);  
  EZ_UnFreezeWidget(listboxB);  
}
/*************************************************************/
static void ListboxACallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_IListBoxFSL(widget);
  EZ_Widget *oEntry = EZ_FileSelectorPatternEntry(fileSelect); /* pattern entry */
  EZ_Widget *entry = EZ_OptEntryEntry(oEntry);

  if(EZ_IListBoxSLine(widget) >= 0)
    {
      EZ_ClearWidgetHighlightFlag(entry); /* remove highlighting on entry */
      EZ_DrawWidget(entry);	  
      /* call the entry call backs */
      EZ_ExecuteWidgetCallBacks(entry);
      if(oEntry) {EZ_ExecuteWidgetCallBacks(oEntry);}
    }
}
/*************************************************************/
static void ListboxBCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_IListBoxFSL(widget);
  EZ_Widget *oEntry = EZ_FileSelectorSelectionEntry(fileSelect);  /* selection entry */
  EZ_Widget *entry = EZ_OptEntryEntry(oEntry);
  
  if(EZ_IListBoxSLine(widget) >= 0)  
    {
      EZ_ClearWidgetHighlightFlag(entry); /* remove highlighting on entry */
      EZ_DrawWidget(entry);	  
      /* call the entry call backs */
      EZ_ExecuteWidgetCallBacks(entry);
      if(oEntry) {EZ_ExecuteWidgetCallBacks(oEntry);}
    }
}
/*************************************************************/
static void ListboxAMotionCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  char str[1024];
  EZ_Widget *fileSelect = EZ_IListBoxFSL(widget);
  EZ_Widget *oEntry = EZ_FileSelectorPatternEntry(fileSelect);  /* pattern entry */
  EZ_Widget *entry = EZ_OptEntryEntry(oEntry);
  char *item = EZ_GetListBoxSelectedItem(widget);
  if(item) 
    {
      int len = strlen(item);
      (void)strcpy(str, item);
      if(str[len-1] == '/') { str[len] = '*';  str[len+1] = 0; }
      else { str[len] = '/'; str[len+1] = '*'; str[len+2] = 0; }

      EZ_SetEntryString(entry, str);
    }
}
/*************************************************************/
static void ListboxBMotionCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_IListBoxFSL(widget);
  EZ_Widget *oEntry = EZ_FileSelectorSelectionEntry(fileSelect);  /* selection entry */
  EZ_Widget *entry = EZ_OptEntryEntry(oEntry);
  char *item = EZ_GetListBoxSelectedItem(widget);

  if(item)
    {
      char str[1024], *dir;
      dir = EZ_FileSelectorCurrentDirectory(fileSelect);
      (void)strcpy(str, dir);
      (void)strcat(str, item);
      EZ_SetEntryString(entry, str);  
    }
}
/*************************************************************/
static void EntryBCallBack(widget,notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect =  EZ_EntryFSE(widget);
  if(fileSelect) 
    {
      EZ_Widget *listbox = EZ_FileSelectorFileListbox(fileSelect);
      char *item = EZ_GetListBoxSelectedItem(listbox);
      if(item)
	{
	  char str[1024], *dir;
	  char *estr = EZ_GetEntryString(widget);
	  dir = EZ_FileSelectorCurrentDirectory(fileSelect);
	  (void)strcpy(str, dir);
	  (void)strcat(str, item);
	  if(strcmp(str,estr))
	    {
	      EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	      EZ_IListBoxSLine(ilistbox) = -1;
	      EZ_IListBoxSLineEnd(ilistbox) = -1;
	      EZ_DrawWidget(ilistbox);
	    }
	}
      EZ_ExecuteWidgetCallBacks(fileSelect);
    }      
}

/*************************************************************/
static void CancelCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_ButtonFSB(widget);  /* the file selector */
  if(fileSelect) EZ_HideWidget(fileSelect);
}
/*************************************************************/
static void OkCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_ButtonFSB(widget);  /* the file selector */
  if(fileSelect)
    {
      EZ_Widget *listbox = EZ_FileSelectorFileListbox(fileSelect);
      char *item = EZ_GetListBoxSelectedItem(listbox);
      if(item)
	{
	  char str[1024], *dir, *estr;
	  EZ_Widget *entry = EZ_FileSelectorSelectionEntry(fileSelect);

	  entry = EZ_OptEntryEntry(entry);
	  estr = EZ_GetEntryString(entry);
	  dir = EZ_FileSelectorCurrentDirectory(fileSelect);
	  (void)strcpy(str, dir);
	  (void)strcat(str, item);
	  if(strcmp(str,estr))
	    {
	      EZ_Widget *ilistbox = EZ_ListBoxIListBox(listbox);
	      EZ_IListBoxSLine(ilistbox) = -1;
	      EZ_IListBoxSLineEnd(ilistbox) = -1;
	      EZ_DrawWidget(ilistbox);
	    }
	}
      EZ_ExecuteWidgetCallBacks(fileSelect);
    }      
}
/*************************************************************/
static void FilterCallBack(widget, notused)
     EZ_Widget *widget;
     void *notused;
{
  EZ_Widget *fileSelect = EZ_ButtonFSB(widget);  /* the file selector */
  if(fileSelect)
    {
      EZ_Widget *oEntry = EZ_FileSelectorPatternEntry(fileSelect);  /* pattern entry */
      EZ_Widget *entry = EZ_OptEntryEntry(oEntry);

      if(entry) { EZ_ExecuteWidgetCallBacks(entry);}
      {EZ_ExecuteWidgetCallBacks(oEntry);}
    }
}  
/*************************************************************/
static void EZ_FreeFileSelectData(widget)
     EZ_Widget *widget;
{
  if(EZ_FileSelectorCurrentDirectory(widget))
    (void)my_free((char *)EZ_FileSelectorCurrentDirectory(widget));
}
/*************************************************************/
void EZ_GetFileSelectorWidgetComponents(widget, pattern_entry, selection_entry,
				  dir_listbox, file_listbox, ok_button,
				  filter_button, cancel_button)
     EZ_Widget *widget;
     EZ_Widget **pattern_entry, **selection_entry, **dir_listbox,
               **file_listbox, **ok_button, **filter_button,
               **cancel_button;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_FILE_SELECTOR)
    {
      if(pattern_entry)
	*pattern_entry = EZ_FileSelectorPatternEntry(widget);
      if(selection_entry)
	*selection_entry = EZ_FileSelectorSelectionEntry(widget);
      if(dir_listbox)
	*dir_listbox = EZ_FileSelectorDirListbox(widget);
      if(file_listbox)
	*file_listbox = EZ_FileSelectorFileListbox(widget);
      if(ok_button)
	*ok_button = EZ_FileSelectorOkButton(widget);
      if(filter_button)
	*filter_button = EZ_FileSelectorFilterButton(widget);
      if(cancel_button)
	*cancel_button = EZ_FileSelectorCancelButton(widget);
    }
}
/*************************************************************/
void EZ_SetFileSelectorInitialPattern(widget, pattern)
     EZ_Widget *widget; char *pattern;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_FILE_SELECTOR)  
    {
      EZ_Widget *rentry, *entry = EZ_FileSelectorPatternEntry(widget);
      EZ_SetOptionalEntryString(entry, pattern);
      rentry = EZ_OptEntryEntry(entry);
      EntryACallBack(rentry, NULL);
    }
}
/*************************************************************/
char *EZ_GetFileSelectorSelection(widget)
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_FILE_SELECTOR)
    {
      return( *(EZ_FileSelectorCurrentSelection(widget)));
    }
  return(NULL);
}
/*************************************************************/
int EZ_CmpStr(vaptr,vbptr)
     CONST void *vaptr;
     CONST void *vbptr;
{
  return( EZ_CompareStr(vaptr,vbptr));
}
int EZ_CompareStr(vaptr,vbptr)
     CONST void *vaptr;
     CONST void *vbptr;
{
  char **aptr = (char **)vaptr;
  char **bptr = (char **)vbptr;
  char *a = *aptr, *b = *bptr;
  if(a == NULL) return(1);
  else if( b == NULL) return(-1);
  while(*a && *b && *a == *b) {a++; b++;}
  if(*a == 0) return(-1);
  else if(*b == 0) return(1);
  return( (*a > *b)? 1: -1);
}
/*************************************************************
 *
 *  given the a pattern and a place or storing the current
 *  directory. Process opattern, and update cdir.
 *     cdir is a malloced array!
 *     opattern is overwritten !!!
 *  Potential problem: assume opattern is a big-enough 
 *  array and working directly on it.
 */
int EZ_ProcessFileSelectorPattern(cdir, opattern)
     char **cdir, *opattern;
{
  char *pattern = opattern, *epat;
  char pathname[1024];

  if((epat = EZ_TildeExpand(pattern)) != NULL) 
    {
      (void)strcpy(pattern, epat);
      (void)free((char *)epat);  /* not allocated by my_malloc */
    }

  /* set the current-directory if *cdir = NULL */
  {
    /* cdir end with a / */
    char **tmp = cdir;
    
    if(*tmp == NULL)
      {
	char *ptr;

	if(pattern[0] == '/')
	  {
	    *tmp = EZ_AllocCopyString(pattern);
	    ptr = (char *)strrchr(*tmp, '/');
	    if(ptr) *(ptr+1) = 0;
	  }
	else  /* path is not absolute, use cwd */
	  {
	    int  len;
	    char cwd[1024];
	    if(getcwd(cwd,1024) == NULL)
              {
                char *home = getenv("HOME");
                if(home==NULL) home="./";
                (void)strcpy(cwd, home);
              }
	    len = strlen(cwd);
	    if(len > 1 && cwd[len-1] != '/') { cwd[len] = '/'; cwd[len+1] = 0; }

	    *tmp = EZ_AllocCopyString(cwd);
	  }
      }
  }
  
  /* collapsing .. and ./  Result are saved in pattern */
  if(pattern[0] == '.' && pattern[1] == '/')
    {
      int i, j = 2;

      (void)strcpy(pathname, *cdir);
      i = strlen(pathname);
      while(pattern[j] != 0) pathname[i++] = pattern[j++];
      pathname[i] = 0;
      (void)strcpy(pattern, pathname);
    }
  else if(pattern[0] == '.' && pattern[1] == '.' && (pattern[2] == 0 || pattern[2] == '/')) 
    {
      int i, j;
      char *slash;
      (void)strcpy(pathname, *cdir);
      i = strlen(pathname);
      if(pathname[i-1] == '/') pathname[i-1] = 0;
      slash = (char *)strrchr(pathname, '/');
      if(slash) *(slash+1) = 0;
      else { pathname[0] = '/';  pathname[1] = 0;}

      i = strlen(pathname);      
      if(pattern[2] == '/')
	{
	  j = 3;
	  while(pattern[j] != 0) pathname[i++] = pattern[j++];
	  pathname[i] = 0;
	}
      else
	{
	  pathname[i++] = '*'; pathname[i] = 0; 
	}
      (void) strcpy(pattern, pathname);
    }
  else if(pattern[0] != '/')
    {
      /* relative to the current path */
      int pos = position_after_last_slash_before_glob_chars( *cdir);
      (void)strncpy(pathname, *cdir, pos);
      pathname[pos] = 0;
      (void)strcat(pathname, pattern);
      (void)strcpy(pattern, pathname);
    }

  /* update cdir */
  {
    int len, dot = 0, changed = 0;
    char *s, *p = pattern, *q = pathname;
   
    while(*p)
      {
	switch(*p)
	  {
	  case '.':
	    dot++;
	    break;
	  case '/':
	    if(dot == 1)
	      {
		q -= 2;
		if(q < pathname) q = pathname;
		changed = 1;
	      }
	    else if(dot == 2) 
	      {
		q -= 3;
		*q = 0;
		s = strrchr(pathname, '/');
		if(s) q = s;
		else q = pathname;
		changed = 1;		
	      }
	    dot = 0;
	    break;
	  default:
	    dot = 0;
	    break;
	  }
	*q = *p;
	p++; q++;
      }
    *q = 0;

    if(changed) strcpy(pattern, pathname);
    len = strlen(pathname);
    if(pathname[len -1] == '/')   pathname[ (--len)] = 0;
    if(len > 0)
      {
	char *slash = (char *)strrchr(pathname, '/');
	if(slash)  *(slash+1) = 0;
	else len = 0;
      }
    if(len <= 0) *cdir = EZ_ReallocCopyString(*cdir, "/");
    else *cdir = EZ_ReallocCopyString(*cdir, pathname);
  }
  return(position_after_last_slash_before_glob_chars( *cdir));
}

/*****************************************************************/
int position_after_last_slash_before_glob_chars(path)
     char *path;
{
  register char c;
  register char *ptr = path;
  int  done = 0;
  int  count = 0;
  while((c = *ptr++) != '\0' && done == 0)
    switch (c)
      {
      case '?':
      case '*':
      case '[':
	done = 1;
	break;
      case '/':
	count = (ptr - path);
	break;
      default:
	break;
      }
  return(count);
}

int EZ_IsGlobPattern(path)
     char *path;
{
  if(path)
    {
      register char c;
      register char *ptr = path;
      while((c = *ptr++) != '\0')
	switch (c)
	  {
	  case '?':
	  case '*':
	  case '[':
	    return(1);
	    break;
	  default:
	    break;
	  }
    }
  return(0);
}

/***********************************************************************/
int EZ_CountSetFilePatterns(pat, patterns)
     char *pat, *patterns[];
{
  int space = 1, count = 0;
  char *ptr = pat;
  
  while(*ptr)
    {
      switch(*ptr)
	{
	case ' ':
	case '\t':
	  if(!space) { space = 1; *ptr = 0; }
	  break;
	case 0:
	  break;
	default:
	  if(space) { patterns[count++] = ptr; space = 0;}
	  break;
	}
      ptr++;
    }
  return(count);
}
/***********************************************************************/
#undef _EZ_WIDGET_FILE_SELECT_C_
