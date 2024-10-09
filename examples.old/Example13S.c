/******************* Example 15S ***************************************/
#include <string.h>
#include "EZ.h"

int     encodeFileName(EZ_Widget *, void *, char **, int *, int *);
int     encodeFileContents(EZ_Widget *, void *, char **, int *, int *);
void    destroyCallBack(EZ_Item *, void *);

int     testencoder(EZ_Widget *, void *, char **, int *, int *);
void    testCallBack(EZ_Item *, void *);

EZ_TreeNode *MyCreateFileItem(char *, void *);

Atom MY_FILE_NAME_ATOM;
Atom MY_FILE_CONTENTS_ATOM;

static char *colors[] = { "red", "green", "blue", "cyan", 
                          "magenta", "yellow", "red3","#cf00cf"};

int main(int ac, char **av)
{
  EZ_Widget *frame, *listTree;
  EZ_TreeNode *root;
  int i;
  
  EZ_Initialize(ac,av,0);

  /* the two target atoms */	
  MY_FILE_NAME_ATOM = EZ_GetAtom("MY_FILE_NAME_ATOM");
  MY_FILE_CONTENTS_ATOM = EZ_GetAtom("MY_FILE_CONTENTS_ATOM");

  frame = EZ_CreateFrame(NULL, "Drag sources");
  EZ_ConfigureWidget(frame, EZ_FILL_MODE, EZ_FILL_BOTH, 
		     EZ_WIDTH, 300, EZ_HEIGHT, 400, 0);
  listTree = EZ_CreateListTree(frame, 1, 1);

  /* we have to use a customized fileNode creater
   * to register DnD encoders and to remember pathnames
   */
  root = EZ_CreateDirTree(MyCreateFileItem, "./*", NULL, EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT); /* */
  EZ_SetListTreeWidgetTree(listTree, root); 
  
  EZ_DisplayWidget(frame);
  EZ_EventMainLoop();
}

EZ_TreeNode *MyCreateFileItem(char *fname, void *data)
{
  EZ_TreeNode *node = NULL;
  if(fname)
    {
      EZ_Item *item = NULL;
      char *ptr, name[256];
      int len;
      
      /* don't mess with fname, work on a local copy of it */
      strcpy(name, fname);
      len = strlen(name); 
      if(name[len-1] == '/') name[len-1] = 0;
      if((ptr = strrchr(name,'/'))) ptr++;
      else ptr = name;
      
      item = EZ_CreateLabelItem(ptr, NULL);
      if(item)
	{
          /* store the full pathname. Use destroyCallback to
           * release the allocated memory !!
           */
          char *path = (char *)malloc((len+2)*sizeof(char));
          (void)strcpy(path,fname);
          EZ_ConfigureItem(item, EZ_CLIENT_PTR_DATA, path,
                           EZ_CLIENT_INT_DATA, len,
                           EZ_DESTROY_CALLBACK, destroyCallBack, path, 0);
	  EZ_ItemAddDnDDataEncoder(item,MY_FILE_NAME_ATOM,0,
                                   encodeFileName,NULL, NULL, NULL);
          EZ_ItemAddDnDDataEncoder(item,MY_FILE_CONTENTS_ATOM,0,
                                   encodeFileContents, NULL, NULL, NULL);
          EZ_ItemAddDnDDataEncoder(item,MY_FILE_NAME_ATOM, ~0,
                                   testencoder, path, testCallBack, path);
          node = EZ_CreateTreeNode(NULL, item);
        }
    }
  return(node);
}

void  destroyCallBack(EZ_Item *item, void *data)
{
  if(data) (void)free((char *)data);
}

int encodeFileName(EZ_Item *item, void *data,
                   char **message, int *length, int *needFree)
{
  if(item)
    {
      char *ptr = (char *)EZ_GetItemPtrData(item);
      int  len = EZ_GetItemIntData(item);
      if(len > 0)
        {
          *length = len;
          *message = ptr;
          *needFree = 0;
          return(EZ_DND_SUCCESS);
        }
    }
  return(EZ_DND_FAILURE);
}

int encodeFileContents(EZ_Item *item, void *data,
                       char **message, int *length, int *needFree)
{
  if(item)
    {
      char *ptr = (char *)EZ_GetItemPtrData(item);
      int  len = EZ_GetItemIntData(item);
      if(len > 0)
        {
          char *msg;
          int  c, totalLength = 0;
          FILE *fp = fopen(ptr, "r");
          if(fp) while(fgetc(fp) != EOF) totalLength++;
          (void)rewind(fp);
          msg = (char *)malloc( (totalLength + 1)*sizeof(char));
          ptr = msg;
          while((c = fgetc(fp)) != EOF) *ptr++ =c;
          fclose(fp);
         *length = totalLength;
         *message = msg;
         *needFree = 1;
         return(EZ_DND_SUCCESS);
        }
    }
  return(EZ_DND_FAILURE);
}
/******************* Example 15S ***************************************/

void  testCallBack(EZ_Item *item, void *data)
{
  if(data) printf("hello from: [%s]\n", data);
}

int testencoder(EZ_Item *item, void *data,
		char **message, int *length, int *needFree)
{
  if(item)
    {
      char *ptr = (char *)EZ_GetItemPtrData(item);
      int  len = EZ_GetItemIntData(item);
      if(len > 0)
        {
	  EZ_Widget *widget, *tmp;
	  int x,y, lx, ly;
	  char *str = strrchr(ptr, '/');
	  widget = EZ_CreateFreeLabel(NULL, NULL);
	  EZ_GetDnDDropLocation(&x,&y, &lx, &ly);
	  EZ_ConfigureWidget(widget, EZ_X,x, EZ_Y, y, 
			     EZ_BORDER_TYPE, EZ_BORDER_RAISED, EZ_BORDER_WIDTH,2,
			     EZ_BACKGROUND, "bisque2", EZ_ORIENTATION, EZ_VERTICAL, 0);
	  tmp = EZ_CreateNWLabel(widget, NULL);
	  EZ_ConfigureWidget(tmp, EZ_PIXMAP_FILE, "question.xpm", 0);
	  tmp = EZ_CreateNWLabel(widget,str==NULL? ptr: str+1);
	  EZ_DisplayWidget(widget);
          return(EZ_DND_SUCCESS);
        }
    }
  return(EZ_DND_FAILURE);
}
