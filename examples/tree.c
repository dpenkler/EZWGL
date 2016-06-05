/**************************** ExampleTree ******************************/
#include "EZ.h"

static quit(EZ_Widget *widget, void *data)
{
  if(getenv("SIMAGE"))
    {
      FILE *fp = fopen("tree.ppm", "w");
      if(fp) 
	{
	  EZ_SaveWidget2PPM(data, fp);
	  fclose(fp);
	}
    }
  EZ_Shutdown();
  exit(0);
}

main(int ac, char **av)
{
  EZ_TreeNode *root1, *root2;
  EZ_Widget *frame, *tree1, *tree2, *frame5, *btn, *tmp;

  EZ_Initialize(ac,av,0);
  frame  = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    NULL,
                              "Frames",           "frames",
                              EZ_FILL_MODE,       EZ_FILL_BOTH,
                              EZ_IPADX,           4,
                              EZ_SIZE,            500, 400,
                              EZ_BG_IMAGE_FILE,   "bg.gif",
                              0);
  tree1= EZ_CreateWidget(EZ_WIDGET_TREE,           frame,  
                         EZ_WIDTH, 100, 
                         0);

  tmp = EZ_CreateWidget(EZ_WIDGET_PANE_HANDLE, frame, 0);

  tree2= EZ_CreateWidget(EZ_WIDGET_TREE,           frame,  
                         EZ_ROW_BG, 1, "bisque", "white", 0);

  root1 = EZ_CreateDirTree(NULL, "/*", NULL, 0);
  root2 = EZ_CreateDirTree(NULL, "/zip2/*", NULL, 0);

  EZ_TreeSetCharacter(root2, EZ_BRANCHED_LIST_TREE, 20, 2, 1);

  EZ_SetListTreeWidgetTree(tree1, root1); 
  EZ_SetListTreeWidgetTree(tree2, root2); 

  frame5 = EZ_CreateWidgetXrm(EZ_WIDGET_FRAME,    frame,
                              "Frame5",           "frame5",
                              EZ_BG_IMAGE_FILE,   "marble",
                              EZ_DOCKABLE,        True,
                              EZ_BORDER_TYPE,     EZ_BORDER_RAISED,
                              EZ_BORDER_WIDTH,    2,
                              EZ_SIZE, 0, 0,
                              0);
  btn  =   EZ_CreateWidgetXrm(EZ_WIDGET_NORMAL_BUTTON,    frame5,
                              "AButton",           "aButton",
                              EZ_LABEL_STRING,     "Done",
                              EZ_CALLBACK,          quit, frame,
                              0);
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}
