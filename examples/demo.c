#include <unistd.h>

#include "EZ.h"

void listTreeCallBack(EZ_Widget *widget, void *data) /* callback for listTree */
{              
  EZ_TreeNode  *node = EZ_GetListTreeWidgetSelection(widget);
  EZ_Widget    *textWidget = (EZ_Widget *)data;
  if(node && textWidget)
    {
      char *path = EZ_GetDirTreeNodeFullPath(node);
      char tmpname[1024];
      
      if(access(path, X_OK) == 0)
	{
          sprintf(tmpname, "%s &", path);
	  (void) system(tmpname);
	}
      else if(access(path, R_OK) == 0)
        {
          if(EZ_GetImageFileType(path) >= 0)
            {
              EZ_Pixmap *pptr;
              if((pptr = EZ_CreateLabelPixmapFromXpmFile(path)) == NULL)
                pptr = EZ_CreateLabelPixmapFromImageFile(path);
              if(pptr)
                {
                  EZ_TextProperty *tmpTp = EZ_GetTextProperty(EZ_LABEL_PIXMAP, pptr, NULL);
                  EZ_TextInsertString(textWidget, "\n"); 
                  EZ_TextInsertStringWithProperty(textWidget, path, tmpTp); 
                  EZ_TextInsertString(textWidget, "\n"); 
                }              
              EZ_TextInsertString(textWidget, path); 
            }
          else EZ_TextLoadFile(textWidget, path);
        }
    }
}

static void rtimer_callback(EZ_Timer *timer, void *pdata)
{
  static int cnt = 0;
  EZ_Widget *ltree = (EZ_Widget *)pdata;
    EZ_UpdateDirTreeWidget(ltree);
}

main(int argc, char **argv)
{
  EZ_Widget *frame, *listTree;
  EZ_Widget *textW, *tmp;
  EZ_TreeNode *root;
  EZ_LabelPixmap *cp, *op;

  EZ_Initialize(argc,argv,0);
  
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL,  /* a toplevel frame   */
                          EZ_WIDTH, 600, EZ_HEIGHT, 400,
                          EZ_PADY, 10, EZ_PADX,10, 
                          EZ_FILL_MODE, EZ_FILL_BOTH, 
                          0);
                                         /* a list tree   */
  listTree = EZ_CreateWidget(EZ_WIDGET_TREE, frame,
                             EZ_WIDTH, 200,     /* width  */
                             EZ_OPTIONAL_HSCROLLBAR, False,
                             EZ_OPTIONAL_VSCROLLBAR, True,
                             0);
  EZ_SetVScrollbarDiscreteSpeed(listTree, 10);

  /* add a pane handle so that user can adjust the relative 
   * size of the two sibling widgets */
  tmp = EZ_CreateWidget(EZ_WIDGET_PANE_HANDLE, frame,
                        0);
  textW = EZ_CreateWidget(EZ_WIDGET_TEXT, frame,
                          EZ_OPTIONAL_HSCROLLBAR, True,
                          EZ_OPTIONAL_VSCROLLBAR, True,                 
                          EZ_TEXT_WIDGET_EDITABLE, True,
                          EZ_SELECTION_BACKGROUND, "yellow",
                          EZ_EMBEDER,               True,
                          0);
  root = EZ_CreateDirTree(NULL, "./*", NULL, 0);  /* open the current dir   */
  EZ_TreeSetCharacter(root, EZ_BRANCHED_LIST_TREE, 20, 2, 1);  
  /* EZ_SetDirTreeOpenDirectoryMode(root, 1); */

  EZ_AddWidgetCallBack(listTree, EZ_CALLBACK, 
		       listTreeCallBack, textW, 0);

  EZ_SetListTreeWidgetTree(listTree, /* link root to listTree*/
                           root); 

  (void)EZ_CreateTimer(2, 0, -1, rtimer_callback, listTree, 0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/*************************************************/
