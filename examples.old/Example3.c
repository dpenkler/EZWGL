/************************** Example 3 ***********************/
#include "EZ.h"                     /* the header file      */

void listTreeCallBack(EZ_Widget *widget, void *data) 
{                                  /* callback for listTree */
  EZ_TreeNode  *node = EZ_GetListTreeWidgetSelection(widget);
  EZ_Widget    *textWidget = (EZ_Widget *)data;
  if(node && textWidget)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      if(item)
	{
	  char *str=NULL;   int len;
	  EZ_GetLabelItemStringInfo(item, &str, &len);
	  if(str)  EZ_TextLoadFile(textWidget, str);
	}
    }
}

int main(int argc, char **argv)
{
  EZ_Widget *frame, *listTree;
  EZ_Widget *textW, *tmp;
  EZ_TreeNode *root;

  frame = EZ_CreateFrame(NULL,NULL);  /* a toplevel frame   */
  EZ_ConfigureWidget(frame, 
		     EZ_WIDTH, 600, EZ_HEIGHT, 400,
		     EZ_PADY, 10, EZ_PADX,10, 
		     EZ_BACKGROUND, "bisque2",
		     EZ_FILL_MODE, EZ_FILL_BOTH, 0);

  listTree = EZ_CreateListTree(frame,1,1); /* a list tree   */
  EZ_ConfigureWidget(listTree, EZ_WIDTH, 200, 0); /* width  */
  /* add a pane handle so that user can adjust the relative 
   * size of the two sibling widgets */
  tmp = EZ_CreatePaneHandle(frame);  
  textW = EZ_CreateTextWidget(frame,1,1,1);/* a text Widget */
  EZ_ConfigureWidget(textW,
		     EZ_SELECTION_BACKGROUND, "yellow",
		     0);
  root = EZ_CreateDirTree(NULL, "./*", NULL, EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT);  /* open the current dir   */

  EZ_AddWidgetCallBack(listTree, EZ_CALLBACK, 
		       listTreeCallBack, textW, 0);

  EZ_SetListTreeWidgetTree(listTree, /* link root to listTree*/
		       root); 
  
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
/************************** Example 3 ***********************/
