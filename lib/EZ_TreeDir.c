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
 ***     Manipulating Dir Trees.                               ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_TREE_DIR_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:  ?????
 */
EZ_TreeNode      *EZ_CreateDirTreeS MY_ANSIARGS((EZ_DirTreeNodeCreator nctr,char *pattern,
                                                 void *cdata, int omode, void *sorter));
EZ_TreeNode      *EZ_CreateDirTree MY_ANSIARGS((EZ_DirTreeNodeCreator nctr,
						char *pattern, void *cdata, int omode));
EZ_TreeNode      *EZ_CreateDirTreeNodeDefault MY_ANSIARGS((char *fname));
void             EZ_OpenCloseDirTreeDirectoryNode MY_ANSIARGS((EZ_Item *item, int collaps));
char             *EZ_GetDirTreeNodeFullPath MY_ANSIARGS((EZ_TreeNode *node));
void             EZ_SetDirTreeOpenDirectoryMode MY_ANSIARGS((EZ_TreeNode *node, int mode));
void             EZ_SetDirTreeDefaultPixmaps MY_ANSIARGS((EZ_Bitmap *o, EZ_Bitmap *c, EZ_Bitmap *f));
void             EZ_SetDirTreeDefaultPixmapsFromXpmFiles MY_ANSIARGS((char *o, char *c, char *f));
int              EZ_UpdateDirTreeWork MY_ANSIARGS((EZ_TreeNode *root,EZ_TreeNode *sel,int *flag));
void             EZ_SetDirTreeSorter MY_ANSIARGS((EZ_TreeNode *root, int (*cmp)MY_ANSIARGS((void *, void *)) ));
/*************************************************************
 *
 * private functions.
 */
static EZ_DirTreeNodeInfo *allocateDirTreeNodeInfo MY_ANSIARGS((void));
static int    updateDirTreeWork MY_ANSIARGS((EZ_TreeNode *root,EZ_TreeNode *sel,int *flag));
static void   sortDirTree MY_ANSIARGS((EZ_TreeNode *node));
static EZ_TreeNode *CompleteDirTreeBranch MY_ANSIARGS((EZ_TreeNode *node, char *path, int start,
						       EZ_DirTreeNodeCreator nctr,void *cdata,
						       int omode, int *status_ret, void *sorter));
static void        InitDefaultDirTreeItemProperty MY_ANSIARGS((void));
static void        *createDirTreeNode MY_ANSIARGS((EZ_DirTreeNodeCreator nctr, char *name, void  *cdata,
                                                   int omode, void *sorter));
static int         cmpDirNodes MY_ANSIARGS((void *na, void *nb));
/* private data */
#include "xpms/folderC.xpm"
#include "xpms/folderO.xpm"
#include "xpms/file.xpm"

static EZ_TextProp *dirItemProperty = NULL;
static EZ_TextProp *fileItemProperty = NULL;
static EZ_Bitmap *folderCBitmap = NULL;
static EZ_Bitmap *folderOBitmap = NULL;
static EZ_Bitmap *fileBitmap = NULL;

/*************************************************************
 *
 * given a pathname (may contain glob characters or ~),
 * create a (partial) DirTree.
 */
EZ_TreeNode *EZ_CreateDirTree(nctr, pat, cdata, omode)
     EZ_DirTreeNodeCreator nctr;  /* caller supplied node creator */
     char *pat;                   /* patterns    */
     void *cdata;                 /* client data */
     int omode;                   /* dir node action when double clicked */
{
  return(EZ_CreateDirTreeS(nctr, pat, cdata, omode, cmpDirNodes));
}


EZ_TreeNode *EZ_CreateDirTreeS(nctr, pat, cdata, omode, sorter)
     EZ_DirTreeNodeCreator nctr;  /* caller supplied node creator */
     char *pat;                   /* patterns    */
     void *cdata;                 /* client data */
     int  omode;                   /* dir node action when double clicked */
     void *sorter;
{
  char *pathname = NULL, pattern[1024], cpy[1024], *patterns[16];
  int position,  npatterns, ii, plen;
  EZ_TreeNode *root = NULL, *cnode = NULL;
  
  /* set the ItemCreator */
  if(omode < EZ_DIR_NODE_OPEN_DIRECTORY_BEGIN || omode > EZ_DIR_NODE_OPEN_DIRECTORY_END)
    omode = EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT; 
  if(sorter == NULL) sorter = (void *)cmpDirNodes;
  /*
   * step 1, process pat.
   */
  if(pat == NULL || *pat == 0)  return((EZ_TreeNode *)NULL);  /* nothing */
  (void)strcpy(cpy, pat); /* make a copy of pat, cpy is going to be modified */

  if( (npatterns = EZ_CountSetFilePatterns(cpy, patterns)) == 0)  
    return((EZ_TreeNode *)NULL);
  
  for(ii = 0; ii < npatterns && ii < 16; ii++)
    {
      (void) strcpy(pattern, patterns[ii]); 
      position = EZ_ProcessFileSelectorPattern(&pathname, pattern);  /* pattern may be modified */
      /* 
       * pathname is the parent-dir name of pattern, ended with a '/'
       * postion is the index after the last / before globing characters in pattern.
       * pattern is the processed pattern.
       */

      if(ii == 0)  /* create a root node */
	{
	  char tfname[8];
	  EZ_Item *item;

	  tfname[0]='/'; tfname[1]=0;
	  cnode = root = (EZ_TreeNode *) createDirTreeNode(nctr, tfname, cdata, omode, sorter);
	  if(root != (EZ_TreeNode *)NULL)
	    { 
	      EZ_TreeNodeParent(cnode) = NULL;
	      item = EZ_TreeNodeItem(root);
	      if(item)
		{
		  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
		  if(ninfo) ninfo->id =  (char *)EZ_AllocCopyString(tfname);
		}
	    }
	  else 
	    {
	      if(pathname) my_free(pathname);
	      return(NULL);
	    }
	}

      /* save glob pattern */
      if(strchr(pattern+1, '/') == NULL) /* crazy root node */
	{
	  if(EZ_IsGlobPattern(pattern+1))
	    {
	      EZ_Item *item = EZ_TreeNodeItem(root);
	      EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
	      char *cptr = ninfo->pattern;
	      if(cptr)
		{
		  char tptn[1204];
		  sprintf(tptn, "%s %s", cptr, pattern+1);
		  ninfo->pattern = EZ_ReallocCopyString(cptr,tptn);
		}
	      else ninfo->pattern = EZ_AllocCopyString(pattern+1);		  
	    }
	}

      /* root is already set, complete subtree before the globing pattern */
      if(1) 
	{
	  char *p0, *p1, *end;
	  char tfname[1024];
	  int pos = (position > 0 ? position - 1: 0);  /* must share a common / */
	  end = pathname + pos;  /* no braching up to here */
	  p1 = pathname;
	  p0 = tfname;
	  while( p1 < end) *p0++ = *p1++; *p0 = 0;
	  
	  /* start after the initial / */
	  cnode = CompleteDirTreeBranch(root, tfname, 1, nctr, cdata, omode, NULL, sorter);
	  if(cnode)
	    {
	      p1 = pattern + position;
	      if(EZ_IsGlobPattern(p1))
		{
		  EZ_Item *item = EZ_TreeNodeItem(cnode);
		  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
		  char *cptr = ninfo->pattern;
		  if(cptr)
		    {
		      sprintf(tfname, "%s %s", cptr, p1);
		      ninfo->pattern = EZ_ReallocCopyString(cptr,tfname);
		    }
		  else ninfo->pattern = EZ_ReallocCopyString(cptr,p1);
		}
	    }
	  else cnode = root;
	}
      
      /*
       * step 3. Now do the globing and create nodes or subtrees.
       */
      plen = strlen(pattern);
      if(position < plen)
	{
	  char **dirlist;
	  dirlist = (char **) EZ_GlobFileName(pattern);
	  if(dirlist)
	    {
	      int j=0;
	      while(dirlist[j] != NULL) j++;
	      qsort(dirlist, j, sizeof(char *), EZ_CompareStr);
	      
	      for(j = 0; dirlist[j] != NULL; j++)
		{
		  (void)CompleteDirTreeBranch(cnode, dirlist[j], position, nctr, cdata, omode, NULL,sorter);
		  (void)free((char *)dirlist[j]);  /* not allocated by my_malloc */
		}
	      (void)free((char *)dirlist);  /* not allocated by my_malloc */
	    }
	}
    }
  if( pathname != (char *)NULL) (void) my_free((char *)pathname);
  /* 6-1-98, sort the tree if npatterns > 1 */
  if(npatterns > 1 || sorter != (void *)cmpDirNodes) sortDirTree(root);
  return(root);
}

static EZ_TreeNode *CompleteDirTreeBranch(pnode, path, start, nctr, cdata, omode, status_ret, sorter)
     EZ_TreeNode *pnode;  /* the branch being created will start at this node */
     char    *path;   /* complete path name */ 
     int     start;   /* position of path to begin with, must not be the leading / */
     void    *cdata, *sorter;
     int     omode, *status_ret;
     EZ_DirTreeNodeCreator nctr;
{
  EZ_TreeNode *cnode, *children, *node_ret = NULL;
  char *p0, *p1, *end, *cptr, *t;
  char tname[1024];
  int length, find;

  if(pnode == (EZ_TreeNode *)NULL || path == (char *)NULL) return(NULL);
  length = strlen(path);
  if(length < start) return(NULL);  /* nothing need to be done */
  
  cnode = pnode;
  end = path + length;
  p0 = path + start;

  while(1)
    {
      /* 1. grab the next field */
      p1 = p0;
      while(*p1 != '/'  &&  p1 < end) p1++;
      cptr = tname;
      t = p0;
      while(t < p1) { *cptr++ = *t++;} *cptr = 0;
    
      /* 2. search to see if there is already a tree-node for it */
      find = 0;
      children = EZ_TreeNodeChildren(cnode);
      while(children)
	{
	  EZ_Item *item = EZ_TreeNodeItem(children);
	  if(item)
	    {
	      EZ_DirTreeNodeInfo *tinfo = EZ_ItemDirItemInfo(item);
	      if(tinfo && tinfo->id && !strcmp(tname,tinfo->id))
		{
		  find = 1;
		  node_ret = cnode = children;
		  break;
		}
	    }
	  children = EZ_TreeNodeSibling(children);
	}
      /* 3. if not, complete the rest */
      if(find == 0) 
	{
	  EZ_TreeNode *tnode;
	  /* 1. get the full path for this item */
	  cptr = tname;
	  t = path;
	  while(t <= p1) { *cptr++ = *t++;} *cptr = 0;
	  /* 2. create the item and set it up */
	  node_ret = tnode = (EZ_TreeNode *) createDirTreeNode(nctr, tname, cdata, omode, sorter);
	  if(tnode) /* new node */
	    {
	      EZ_TreeNode *sib;
	      EZ_Item *item = EZ_TreeNodeItem(tnode);

	      cptr = tname;
	      while( p0 < p1) { *cptr++ = *p0++;} *cptr = 0;
	      if(item)
		{
		  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
		  ninfo->id =  EZ_AllocCopyString(tname);
		  ninfo->node = tnode;
		  ninfo->mode = omode;
		}
	      EZ_TreeNodeType(tnode) = EZ_TreeNodeType(cnode);
	      EZ_TreeNodeParent(tnode) = cnode;		  
	      EZ_TreeNodeIndent(tnode) = EZ_TreeNodeIndent(cnode);
	      EZ_TreeNodeSpacing(tnode) = EZ_TreeNodeSpacing(cnode);
              EZ_TreeNodeDrawBranch(tnode) = EZ_TreeNodeDrawBranch(cnode);

	      sib = EZ_TreeNodeChildren(cnode);
	      if(sib == (EZ_TreeNode *)NULL) EZ_TreeNodeChildren(cnode) = tnode;
	      else
		{
		  while(EZ_TreeNodeSibling(sib)) 
		    sib = EZ_TreeNodeSibling(sib);
		  EZ_TreeNodeSibling(sib) = tnode;
		}
	      cnode = tnode;
	      if(status_ret) *status_ret += 1;
	    }
	} 
      p1++; 
      if(p1 >= end) { /* have done all */ return(node_ret); break;}
      p0 = p1;
    }
  return(node_ret);
}

/*********************************************************************
 *
 * Open a dir node.
 */
void  EZ_OpenCloseDirTreeDirectoryNode(ditem, collapsing)
     EZ_Item *ditem;  int collapsing;
{
  if(EZ_ItemExist(ditem))
    {
      char path[1024], *pathptr;
      EZ_Item *theItem = ditem;
      EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(theItem);
      EZ_TreeNode *node, *snode;
      EZ_TreeNode *children;
      EZ_Widget *widget;
      EZ_TreeNode *root;
      int idx;
      int widgetOK = 0;
      int openIt = 1;
      int PntrGrabed = 0;
      int omode;
      EZ_DirTreeNodeCreator creator;
      void  *cdata, *sorter;
      Window win = (Window )NULL;

      if(ninfo == NULL) return; /* something is wrong */
      snode = (EZ_TreeNode *)ninfo->node;
      creator = ninfo->creator;
      cdata = ninfo->cdata;
      sorter = ninfo->sorter;
      omode = ninfo->mode;
      if(snode == NULL) return; /* something is wrong */
      root = node = snode;

      children = EZ_TreeNodeChildren(node);      
      widget = (theItem == NULL ? NULL : EZ_ItemParentWidget(theItem));

      if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)
	{
	  if(EZ_WidgetMapped(widget))
	    {
	      widgetOK = 1;
	      win = EZ_WidgetWindow(widget);
	      if( XGrabPointer(EZ_Display,
			       win,
			       True,
			       0,
			       GrabModeAsync,
			       GrabModeAsync, 
			       None,
			       EZ_GetCursor(EZ_C_WATCH), 
			       CurrentTime) == GrabSuccess)
		PntrGrabed = 1;
	    }
	}
      /* find root, record its current geometry */
      if(children)  /* destroy all descendants */
	{
	  char *sptn;
	  EZ_TreeDestroyNodeDescendants(node);
	  if(theItem)
	    {
	      if(EZ_ItemPixmap(theItem) == folderOBitmap) 
		EZ_ItemPixmap(theItem) = folderCBitmap;
	      sptn = ninfo->pattern;
	      if(sptn) (void)my_free((char *)sptn);
	      ninfo->pattern = NULL;
	    }
	  while(EZ_TreeNodeParent(root)) root = EZ_TreeNodeParent(root);
	  openIt = 0;
	}
      
      if(collapsing)
	{
	  EZ_TreeNode *tmp, *parent, *children, *sibling;

	  parent = EZ_TreeNodeParent(node);
	  if(parent)
	    {
	      EZ_Item *item = EZ_TreeNodeItem(parent);
	      if(item)
		{
		  EZ_DirTreeNodeInfo *tinfo = EZ_ItemDirItemInfo(item);
		  char *sptn = tinfo->pattern;
		  if(sptn) (void)my_free( sptn);
		  tinfo->pattern = NULL;
		  if(EZ_ItemPixmap(item) == folderOBitmap) 
		    EZ_ItemPixmap(item) = folderCBitmap;
		}
	      children = EZ_TreeNodeChildren(parent);
	      if(node == children)
		{
		  sibling = EZ_TreeNodeSibling(children);
		  EZ_TreeNodeSibling(children) = NULL;
		}
	      else
		{
		  tmp = sibling = children;
		  while( EZ_TreeNodeSibling(tmp) != node)
		    tmp = EZ_TreeNodeSibling(tmp);
		  EZ_TreeNodeSibling(tmp) = EZ_TreeNodeSibling(node);
		  EZ_TreeNodeSibling(node) = NULL;
		  EZ_TreeNodeChildren(parent) = node;
		}
	      while(sibling)
		{
		  EZ_TreeNodeParent(sibling) = NULL;
		  tmp = EZ_TreeNodeSibling(sibling);
		  EZ_TreeDestroySubtree(sibling);
		  sibling = tmp;
		}
	    }
	}


      if(openIt | collapsing) /* now open the directory */
	{
	  path[1023] = 0;
	  path[1022] = '*';
	  path[1021] = '/';
	  idx = 1020;

	  if(theItem)
	    {
	      char *cptr = ninfo->pattern;
	      if(EZ_ItemPixmap(theItem) == folderCBitmap)
		EZ_ItemPixmap(theItem) = folderOBitmap;	 
	      ninfo->pattern = EZ_ReallocCopyString(cptr,path+1022);
	    } 
	  while(node) 
	    {
	      EZ_Item *item;
	      char *ptr, *tstr;
	      EZ_DirTreeNodeInfo *tinfo = NULL;

	      item = EZ_TreeNodeItem(node);
	      if(item) tinfo = EZ_ItemDirItemInfo(item);
	      if(tinfo) tstr = tinfo->id;
	      else tstr = NULL;
	      /* printf("{%s}", tstr);*/
	      
	      if(tstr && *tstr)
		{
		  ptr = tstr + (strlen(tstr) -1);
		  while( ptr >= tstr) { path[idx--] = *ptr--;}
		  path[idx--] = '/';
		}
	      root = node;
	      if(item) EZ_ItemDirty(item) = 1;
	      node = EZ_TreeNodeParent(node);	  
	    }
	  pathptr = path + (idx + 3); /* root node is labled by /, */
	  idx = 1022 - (int)(pathptr - path);
	  /* printf("[%s] %d [%s]\n",pathptr, idx, pathptr+idx); */
	  /* now open this directory and create nodes */
	  {
	    char **dirlist = (char **) EZ_GlobFileName(pathptr);
	    if(dirlist)
	      {
		int j=0;
		while(dirlist[j] != NULL) j++;

                qsort(dirlist, j, sizeof(char *), EZ_CompareStr);
		for(j = 0; dirlist[j] != NULL; j++)
		  {
		    (void) CompleteDirTreeBranch(snode, dirlist[j], idx, creator,cdata, omode, NULL,sorter);
		    (void) free((char *)dirlist[j]);
		  }
	      }
	    (void)free((char *)dirlist); 
            if(sorter != (void *)cmpDirNodes) sortDirTree(snode);
	  }
	}
      
      if(widgetOK)
	{
	  extern void tree_scroll_update_selection 
	    MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *node));
	  EZ_Item *item = EZ_TreeNodeItem(root);	  
	  if(item) EZ_ItemDirty(item) = 1;

	  EZ_DrawWidget(widget);
	  tree_scroll_update_selection(widget, snode);
	  if(PntrGrabed) XUngrabPointer(EZ_Display, CurrentTime);
	}
    }
}
static void dirNodeCallback(notused, data)
     EZ_Item *notused; void *data;
{
  EZ_Item *item = (EZ_Item *)data;
  if(item)
    {
      EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
      int mode = ninfo->mode;
      if(mode != EZ_DIR_NODE_OPEN_DIRECTORY_NONE)
	EZ_OpenCloseDirTreeDirectoryNode(item,(mode == EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING));
    }
}
/*************************************************************/
static void InitDefaultDirTreeItemProperty()
{
  folderCBitmap = EZ_CreateLabelPixmapFromXpmData(closed_folder_xpm);
  folderOBitmap = EZ_CreateLabelPixmapFromXpmData(opened_folder_xpm);
  fileBitmap = EZ_CreateLabelPixmapFromXpmData(file_xpm);
     
  dirItemProperty = EZ_GetTextProperty(EZ_FOREGROUND, "red", EZ_LABEL_PIXMAP, folderCBitmap, 0);
  fileItemProperty = EZ_GetTextProperty( EZ_LABEL_PIXMAP, fileBitmap, 0);
}

/*************************************************************/
static void setDirOMode(node,mode)
     EZ_TreeNode *node; int mode;
{
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeItem(node);
      if(item)
	{
	  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
	  if(ninfo) ninfo->mode = mode;	      
	}
      {
	EZ_TreeNode *children = EZ_TreeNodeChildren(node);
	EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
	setDirOMode(children,mode);
	setDirOMode(sibling,mode);
      }
    }
}
void  EZ_SetDirTreeOpenDirectoryMode(node, mode)
     EZ_TreeNode *node; int mode;
{
  if(node)
    {
      if(dirItemProperty == (EZ_TextProp *)NULL)
	InitDefaultDirTreeItemProperty();
      if(mode >= EZ_DIR_NODE_OPEN_DIRECTORY_BEGIN && 
	 mode <= EZ_DIR_NODE_OPEN_DIRECTORY_END) 
	{
	  EZ_TreeNode *root = node;
	  while(EZ_TreeNodeParent(root)) {root = EZ_TreeNodeParent(root); }
	  setDirOMode(root,mode);
	}
      else (void) fprintf(stderr, "EZ_SetDirTreeOpenDirectoryMode: invalid mode %d\n", mode);
    }
}
/*************************************************************/
char *EZ_GetDirTreeNodeFullPath(dnode)
     EZ_TreeNode *dnode;
{
  static char pathname[1024];
  if(dnode)
    {
      EZ_DirTreeNodeInfo *ninfo = NULL;
      EZ_TreeNode *node = dnode;
      int idx=1022, cnt = 0;
      pathname[1023] = 0;
    
      while(node) 
	{
	  EZ_Item *item;
	  char *ptr, *tstr;

	  item = EZ_TreeNodeItem(node);
	  if(item) ninfo = EZ_ItemDirItemInfo(item);
	  if(ninfo) tstr = ninfo->id;
	  else tstr = (char *)NULL;

	  if(tstr && *tstr)
	    {
	      ptr = tstr + (strlen(tstr) -1);
	      while( ptr >= tstr) { pathname[idx--] = *ptr--;}
	      pathname[idx--] = '/';
	    }
	  node = EZ_TreeNodeParent(node); 
	  cnt++;
	}
      if(cnt > 1) return(pathname+(idx+3));
      else return(pathname+(idx+2));
    }
  return(NULL);
}
/********************************************************************/
EZ_TreeNode *EZ_CreateDirTreeNodeDefault(fname) char *fname;
{
  EZ_TreeNode *tnode = NULL;
  if(fname)
    {
      EZ_Item *item;
      char *tname, *filename = fname;
      int length, endslash = 0;
      EZ_TextProp *tprop;
      struct stat stat_buf;
      int status = stat(fname, &stat_buf);

      if(dirItemProperty == NULL) InitDefaultDirTreeItemProperty();
      if(status >= 0 && S_ISDIR(stat_buf.st_mode)) tprop = dirItemProperty;
      else tprop = fileItemProperty;

      length = strlen(filename);
      if(length > 1)
        {
          if(filename[length-1] == '/')
            {
              endslash = 1;
              filename[length-1] = 0;
            }
          tname = (char *)strrchr(filename,'/');
          if(tname == NULL)  tname = filename;
          else tname++;
        }
      else  tname = filename;
	  
      item = EZ_CreateLabelItem(tname, tprop);
      if(item) 
	{ 
	  tnode = EZ_CreateTreeNode(NULL, item);
	  if(tprop == dirItemProperty)
	    EZ_TreeNodeOpenable(tnode) = 1;
	}
      else tnode = NULL;
      if(endslash) filename[length-1] = '/';
    }
  return(tnode);
}
/********************************************************************/
static void *createDirTreeNode(creator, fname, cdata, omode, sorter)
     EZ_DirTreeNodeCreator creator; char *fname; void *cdata; int omode; void *sorter;
{
  EZ_TreeNode *tnode = NULL;
  if(fname && *fname)
    {
      struct stat stat_buf;
      int status = stat(fname, &stat_buf);
      
      if(creator == NULL) tnode = EZ_CreateDirTreeNodeDefault(fname);
      else tnode = (EZ_TreeNode *)creator(fname, cdata);
      if(tnode)
	{
	  EZ_Item *item = EZ_TreeNodeItem(tnode); 
	  if(item)
	    {
	      int isdir = (status >= 0 && S_ISDIR(stat_buf.st_mode));
	      EZ_DirTreeNodeInfo *ninfo = allocateDirTreeNodeInfo();
	      EZ_ItemDirItemInfo(item) = ninfo;
	      EZ_ItemIsDirItem(item) = isdir;
	      if(ninfo)
		{
		  ninfo->creator = creator; 
		  ninfo->cdata = cdata;
		  ninfo->node = tnode;
		  ninfo->sorter = sorter;
		  ninfo->mode = omode;
		}
	      if(isdir)
		{
		  EZ_CallBackStruct **funcList =  &(EZ_ItemCallback(item));
		  EZ_AddCallBackWork(funcList, (EZ_CallBack)dirNodeCallback, item,0,1);
		}
	    }
	}
    }
  return( (void *)tnode);
}
/********************************************************************/
void EZ_ResetGVTreeDirC()
{
  dirItemProperty = NULL;
  fileItemProperty = NULL;
  folderCBitmap = NULL;
  folderOBitmap = NULL;
  fileBitmap = NULL;
}
/********************************************************************/
void EZ_SetDirTreeDefaultPixmaps(o, c, f)
     EZ_Bitmap *o, *c, *f;
{
  folderCBitmap = c;
  folderOBitmap = o;  
  fileBitmap = f;

  dirItemProperty = EZ_GetTextProperty(EZ_FOREGROUND, "red", EZ_LABEL_PIXMAP, folderCBitmap, 0);
  fileItemProperty = EZ_GetTextProperty( EZ_LABEL_PIXMAP, fileBitmap, 0);
}
void EZ_SetDirTreeDefaultPixmapsFromXpmFiles(o,c,f)
     char *o, *c, *f;
{
  if(c) folderCBitmap = EZ_CreateLabelPixmapFromXpmFile(c);
  if(o) folderCBitmap = EZ_CreateLabelPixmapFromXpmFile(o);
  if(f) fileBitmap = EZ_CreateLabelPixmapFromXpmFile(f);
  dirItemProperty = EZ_GetTextProperty(EZ_FOREGROUND, "red", EZ_LABEL_PIXMAP, folderCBitmap, 0);
  fileItemProperty = EZ_GetTextProperty( EZ_LABEL_PIXMAP, fileBitmap, 0);
}    
/**************************************************/
static int delete_dir_node(node, sel, level)
     EZ_TreeNode *node, *sel; int level;
{
  if(node)
    {
      int deleted = 0;
      EZ_Item *item = EZ_TreeNodeItem(node);
      EZ_TreeNode *sib;
      EZ_TreeNode *child = EZ_TreeNodeChildren(node);

      if(level) sib = EZ_TreeNodeSibling(node); else sib = NULL;
      if(node == sel) deleted = 1;
      (void)my_free((char *)node);
      EZ_DestroyItem(item);
      deleted += delete_dir_node(child, sel, 1);
      if(sib) deleted += delete_dir_node(sib, sel, 1);
      return(deleted);
    }
  return(0);
}
/**************************************************/
int EZ_UpdateDirTreeWork(root, sel, removed)
     EZ_TreeNode *root; /* root of dir tree */
     EZ_TreeNode *sel;  /* current selection */
     int *removed;      /* flag if selection is deleted */
{
  int modified = updateDirTreeWork(root, sel, removed);
  if(modified) sortDirTree(root);
  return(modified);
}
/**************************************************/
int updateDirTreeWork(root, sel, removed)
     EZ_TreeNode *root; /* root of dir tree */
     EZ_TreeNode *sel;  /* current selection */
     int *removed;      /* flag if selection is deleted */
{
  if(root == NULL) {  *removed = 0;   return(0); }
  {
    EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
    EZ_Item *item = EZ_TreeNodeItem(root);
    EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
    int modified, deleted = 0, dec=0, des=0, decc = 0;
    
    if(ninfo == NULL) { *removed = 0; return(0);} /* not a dirTree */
    /* first update sibling node */
    modified =  updateDirTreeWork(sibling,sel, &des);
    
    if(item)
      {
	char *ptn = ninfo->pattern;

	if(1) /* delete children nodes cor. to deleted files */
	  {
	    EZ_TreeNode *tnode = EZ_TreeNodeChildren(root);	    
	    EZ_TreeNode *save = NULL, *curnode = NULL;
	    int len;
	    char prefix[1024];
	    strcpy(prefix, EZ_GetDirTreeNodeFullPath(root));
	    len = strlen(prefix);
	    if(len > 1) { prefix[len] = '/'; len++;}
	    
	    while(tnode)
	      {
		EZ_TreeNode *tmp = EZ_TreeNodeSibling(tnode);
		EZ_Item *item = EZ_TreeNodeItem(tnode);
		char *id;
		int del = 1;
		EZ_DirTreeNodeInfo *tninfo = NULL;		
		if(item) tninfo = EZ_ItemDirItemInfo(item);
		if(tninfo) id = tninfo->id;
		else id = NULL;
		
		if(id)
		  {
		    struct stat finfo;
		    char *ptr = prefix + len;
		    while(*id) *ptr++ = *id++;  *ptr = 0;
		    if(lstat(prefix, &finfo) >= 0) del = 0;
		  }
		if(del) { modified++;   deleted += delete_dir_node(tnode, sel, 0);}
		else
		  {
		    if(save == NULL) { save = tnode; curnode = tnode;}
		    else 
		      { 
			EZ_TreeNodeSibling(curnode) = tnode;
			curnode = tnode;
		      }
		  }
		tnode = tmp;
		if(curnode) EZ_TreeNodeSibling(curnode) = NULL;
	      }
	    EZ_TreeNodeChildren(root) = save;
	  }
	
	if(ptn && *ptn)  /* have a real glob pattern */
	  {
	    char pname[1024], sptn[256], **dirlist = NULL, **itemlist = NULL, *patterns[16];
	    char *sdir= NULL, **cdir = &sdir, pattern[1024];
	    int  i, j, k,l, plength, npatterns, position, nentries = 0;

	    /* build the pattern = "path / ptn" */
	    strcpy(pname, EZ_GetDirTreeNodeFullPath(root));
	    plength = strlen(pname);
	    if(plength > 1)
	      {
		plength += 1;
		sptn[0]='/'; sptn[1]=0;
		strcat(sptn, ptn);    
		strcat(pname, sptn);
	      }
	    else strcat(pname, ptn);
	    
	    npatterns = EZ_CountSetFilePatterns(pname, patterns);
	    for(i = 0; i < npatterns; i++)
	      {
		(void) strcpy(pattern, patterns[i]);
		position = EZ_ProcessFileSelectorPattern(cdir, pattern);
		dirlist = (char **) EZ_GlobFileName(pattern);
		if(dirlist)
		  {
		    j = 0;
		    while(dirlist[j] != NULL) j++;
		    k = nentries;
		    nentries += j;
		    if(itemlist == NULL) itemlist= (char **)my_malloc((nentries+1)*sizeof(char *),_MISC_DATA_);
		    else itemlist = (char **)my_realloc(itemlist, (nentries+1)*sizeof(char *),_MISC_DATA_);
		    for(l = 0; l < j; l++)   itemlist[l+k] = dirlist[l];
		    (void)free((char *)dirlist); 
		  }
	      }
	    if(itemlist)
	      {
		EZ_TreeNode *tnode = root;
		void *cdata = ninfo->cdata;
		void *sorter = ninfo->sorter;
		int  omode = ninfo->mode;
		EZ_DirTreeNodeCreator  creator = ninfo->creator;
		int newnodes = 0;

                qsort(itemlist, nentries, sizeof(char *), EZ_CompareStr); 
		for(i = 0; i < nentries;  i++)
		  {
		    (void) CompleteDirTreeBranch(tnode, itemlist[i], plength, creator,
						 cdata, omode, &newnodes, sorter);
		    (void) free(itemlist[i]);
		  }
		(void)free((char *)itemlist); 	    
		if(newnodes) modified++;
                if(sorter != (void *)cmpDirNodes) modified++;
	      }
	  }
      }
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      /* update children node */
      modified += updateDirTreeWork(children,sel, &dec);
    }
    *removed = deleted + des + dec + decc;
    return(modified);
  }
}
/********************************************************************/
static EZ_DirTreeNodeInfo *allocateDirTreeNodeInfo()
{
  EZ_DirTreeNodeInfo *info = my_malloc(sizeof(EZ_DirTreeNodeInfo), _DIR_TNI_INFO_);
  if(info)
    {
      info->id = NULL;
      info->pattern = NULL;
      info->creator = NULL;
      info->cdata = NULL;
      info->node = NULL;
      info->mode = EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT;
    }
  return(info);
}
/********************************************************************/
static int cmpDirNodes(va, vb) void *va, *vb;
{
  EZ_TreeNode **nap = (EZ_TreeNode **)va;
  EZ_TreeNode **nbp = (EZ_TreeNode **)vb;
  EZ_TreeNode *na = *nap;
  EZ_TreeNode *nb = *nbp;

  if(na == NULL) return (1);
  else if(nb == NULL) return (-1);
  else
    {
      EZ_Item *a = EZ_TreeNodeItem(na);
      EZ_Item *b = EZ_TreeNodeItem(nb);
      char    *astr = EZ_LabelItemString(a);
      char    *bstr = EZ_LabelItemString(b);
      return(EZ_CompareStr( &astr, &bstr));
    }
}

static void sortDirTree(node) EZ_TreeNode *node;
{
  if(node)
    {
      EZ_TreeNode *child, *sibling, *tmp;
      child = EZ_TreeNodeChildren(node);
      if(child)
	{
	  int i, cnt = 1;
          int  (*sorter) MY_ANSIARGS((void *, void *));
	  EZ_TreeNode **nodes, *snodes[512];
          EZ_DirTreeNodeInfo   *tninfo = NULL; 
          EZ_Item *item = EZ_TreeNodeItem(node);
          if(item) tninfo = EZ_ItemDirItemInfo(item);
          sorter = (tninfo? (int (*) MY_ANSIARGS((void *, void *))) tninfo->sorter : cmpDirNodes);

	  sibling = child;
	  while( (sibling = EZ_TreeNodeSibling(sibling)) != NULL) cnt++;
	  if(cnt> 1) /* sort */
	    {
	      if(cnt > 510) nodes = (EZ_TreeNode **)my_malloc( (cnt+2)*sizeof(EZ_TreeNode *), _U_DIR_TREE_);
	      else nodes = snodes;
	      nodes[0] = child;
	      sibling = child;
	      i = 1;
	      while( (sibling = EZ_TreeNodeSibling(sibling)) != NULL)
		nodes[i++] = sibling;
	      nodes[i] = NULL;
	      qsort(nodes, cnt, sizeof(EZ_TreeNode *), 
                     (int (*) MY_ANSIARGS((CONST void *,CONST void *)))sorter);	      
	      EZ_TreeNodeChildren(node) = nodes[0];
	      sibling = nodes[0];
	      i = 1;
	      while(i <= cnt)
		{
		  tmp =  nodes[i++];
		  EZ_TreeNodeSibling(sibling) = tmp;
		  sibling = tmp;
		}
	      if(nodes != snodes) (void)my_free((char *)nodes);
	    }
	}
      child = EZ_TreeNodeChildren(node);
      sortDirTree(child);
      sibling = EZ_TreeNodeSibling(node);
      sortDirTree(sibling);
    }
}
/********************************************************************/	  
int EZ_SetDirTreeSorterWork(root, sorter) EZ_TreeNode *root; void *sorter;
{
  if(root)
    {
      EZ_Item *item = EZ_TreeNodeItem(root);
      if(item)
        {
          EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
          if(ninfo && ninfo->sorter != sorter)
            {
              EZ_TreeNode *child, *sibling;
              ninfo->sorter = sorter;
              EZ_ItemDirty(item) = 1;
              child = EZ_TreeNodeChildren(root);
              sibling = EZ_TreeNodeSibling(root);
              EZ_SetDirTreeSorterWork(child, sorter);
              EZ_SetDirTreeSorterWork(sibling, sorter);
              return(1);
            }
        }
    }
  return(0);
}
/********************************************************************/	
void  EZ_SetDirTreeSorter(root, cmp)
     EZ_TreeNode *root;
     int (*cmp)MY_ANSIARGS((void *, void *));
{
  int (*sorter) MY_ANSIARGS((void *, void*));
  sorter= cmp;
  if(sorter == NULL) sorter = cmpDirNodes;
  if(EZ_SetDirTreeSorterWork(root, sorter))
    sortDirTree(root);
}
/********************************************************************/	  


#undef _EZ_TREE_DIR_C_


