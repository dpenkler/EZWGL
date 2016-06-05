/**************************** ExampleTree ******************************/
#include "EZ.h"

main(int ac, char **av)
{
  EZ_Item     *item;
  EZ_Widget   *tree, *btn;
  EZ_TreeNode *root, *node1, *node2, *node11, *tmp;
  EZ_TextProperty *p1, *p2, *p3;

  EZ_Initialize(ac, av, 0);
  tree = EZ_CreateListTree(NULL, 1, 1);
  EZ_ConfigureWidget(tree, EZ_WIDTH, 400, EZ_HEIGHT, 400, EZ_IPADY, 2, 
                     EZ_SELECTION_FOREGROUND, "black",  0);
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
  root = EZ_CreateTreeNode(NULL, item);
  
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
  item = EZ_CreateWidgetItem(btn);
  tmp = EZ_CreateTreeNode(node11, item);

  EZ_TreeSetCharacter(root, EZ_TREE, 20, 40, 1);
  EZ_SetListTreeWidgetTree(tree, root);
  
  EZ_DisplayWidget(tree);
  EZ_EventMainLoop();
}

