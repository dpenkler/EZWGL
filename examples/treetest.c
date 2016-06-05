/**************************** ExampleTree ******************************/
#include "EZ.h"

static void resettree(EZ_Widget *w, void *d)
{
  static int ii = 0;
  ii++;
  EZ_TreeSetCharacter(d, EZ_TREE, 20, 2, (ii & 1) ? 1 : 2);
}

void callback(EZ_Widget *tree, void *d);  
void wcallback(EZ_Widget *tree, void *d);  

EZ_TreeNode *Root;
main(int ac, char **av)
{
  EZ_Item     *item;
  EZ_Widget   *tree, *btn;
  EZ_TreeNode *root, *node1, *node2, *node11, *tmp;
  EZ_TextProperty *p1, *p2, *p3;

  EZ_Initialize(ac, av, 0);
  tree = EZ_CreateWidget(EZ_WIDGET_LIST_TREE, NULL,
                         EZ_WIDTH_HINT, 400, EZ_HEIGHT_HINT, 400, EZ_IPADY, 2, 
			 EZ_ROW_BG,     1, "white", "bisque",
                         EZ_SELECTION_BACKGROUND, "green3",  0);

  EZ_ConfigureWidget(tree, EZ_CALLBACK, callback, tree, NULL);

  p1 = EZ_GetTextProperty(EZ_PIXMAP_FILE, "question.xpm",
			  EZ_FOREGROUND, "red",
			  EZ_FONT_NAME, 
			  "-Adobe-Helvetica-Medium-R-Normal--*-180-*-*-*-*-*-*",
			  0);
  p2 = EZ_GetTextProperty(EZ_PIXMAP_FILE, "folder.xpm",
			  EZ_FOREGROUND, "blue", 0);
  p3 = EZ_GetTextProperty(EZ_PIXMAP_FILE, "file.xpm",
			  EZ_FOREGROUND, "yellow", 0);

  /* the root node */
  item = EZ_CreateLabelItem("root", p1);
  Root = root = EZ_CreateTreeNode(NULL, item);
  
  /* children */
  item = EZ_CreateLabelItem("child 1", p2);
  node1 = EZ_CreateTreeNode(root, item);
  item = EZ_CreateLabelItem("child 2", p2);
  node2 = EZ_CreateTreeNode(root, item);  

  /* grand children */
  item = EZ_CreateLabelItem("grandchild 11", p3);
  node11 = EZ_CreateTreeNode(node1, item);
  item = EZ_CreateLabelItem("grandchild 12", p3);
  tmp = EZ_CreateTreeNode(node1, item);  
  item = EZ_CreateLabelItem("grandchild 21", p3);
  tmp = EZ_CreateTreeNode(node2, item);  

  /* grand grand children */
  item = EZ_CreateLabelItem("gg child 111", p2);
  tmp = EZ_CreateTreeNode(node11, item);
  btn = EZ_CreateButton(NULL, "A Widget", 0);
  /* EZ_AddWidgetCallBack(btn, EZ_CALLBACK, resettree, root, 0);*/
  item = EZ_CreateWidgetItem(btn);
  EZ_AddWidgetCallBack(btn, EZ_CALLBACK, wcallback, item, 0);
  node11 = EZ_CreateTreeNode(node11, item);
  item = EZ_CreateLabelItem("child of widget item", p2);
  node11 = EZ_CreateTreeNode(node11, item);

  item = EZ_CreateLabelItem("test node 1", p2);
  node11 = EZ_CreateTreeNode(node11, item);

  item = EZ_CreateLabelItem("test node 2", p2);
  node11 = EZ_CreateTreeNode(node11, item);

  item = EZ_CreateLabelItem("test node 3", p2);
  node11 = EZ_CreateTreeNode(node11, item);

  item = EZ_CreateLabelItem("test node 4", p2);
  node11 = EZ_CreateTreeNode(node11, item);

  item = EZ_CreateLabelItem("test node 5", p2);
  tmp = node11 = EZ_CreateTreeNode(node11, item);

  {
    unsigned   long gcmask;
    XGCValues  gcvalues;
    unsigned   long red, green, blue, yellow, cyan, magenta;
    GC         gc;
  
    gcvalues.foreground = red;
    gc = EZ_GetGC( GCForeground, &gcvalues);
    item = EZ_CreateItem(EZ_FIG_ITEM,    NULL);
    /*EZ_FigItemAddRectangle(item, gc, 0,0,90, 80);*/
    gcvalues.foreground = green;
    gc = EZ_GetGC( GCForeground, &gcvalues);
    EZ_FigItemAddFilledArc(item, gc, 0, 0, 50, 50, 0, 300*64);
    /* item = EZ_CreateLabelItem("test node 6", p2);*/
    node11 = EZ_CreateTreeNode(node11, item);
  }

  EZ_SetListTreeWidgetTree(tree, root);
  EZ_ListTreeWidgetSelectNode(tree, tmp, NULL);

  EZ_TreeSetCharacter(root, EZ_BRANCHED_LIST_TREE, 20, 2, 2);
  EZ_DisplayWidget(tree);

  EZ_EventMainLoop();
}
void callback(EZ_Widget *wree, void *tree)
{
  EZ_TreeNode *node =EZ_GetListTreeWidgetSelection(tree);
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeGetItem(node);
      if(item)
	{
	  int w, h;
	  /* EZ_FreezeWidget(tree);*/
	  EZ_ConfigureItem(item, EZ_FONT_NAME,
			   "-Adobe-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*",
			   NULL);
	  /*
	  EZ_ComputeSubtreeSize(Root, &w, &h);
	  EZ_UnFreezeWidget(tree);
	  */
	}
    }
}

void wcallback(EZ_Widget *w, void *d)
{
  if(EZ_GetWidgetIntData(w) == 0)
    { 
      EZ_ConfigureWidget(w, EZ_FONT_NAME,
			 "-Adobe-Helvetica-Medium-R-Normal--*-240-*-*-*-*-*-*",
			 NULL);  
      EZ_SetWidgetIntData(w, 1);

    }
  else
    {
      EZ_ConfigureWidget(w, EZ_FONT_NAME,
			 "-Adobe-Helvetica-Medium-R-Normal--*-120-*-*-*-*-*-*",
			 NULL);  
      EZ_SetWidgetIntData(w, 0);
    }
  EZ_UpdateDisplayItem(d);
}


