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
 ***              Managing Resources                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_RESOURCE_EDIT_C_
#include "EZ_Widget.h"
#include "EZ_ResEdit.h"
/**********************************************************************/
int EZ_HandleResourceEditorMessage MY_ANSIARGS((EZ_Message *msg));
/**********************************************************************/
char         *EZ_PrintOneWidgetTree  MY_ANSIARGS((EZ_Widget *widget));
char         *EZ_PrintWidgetTree  MY_ANSIARGS((void));
EZ_TreeNode  *EZ_ParseWidgetTree  MY_ANSIARGS((char *str));
EZ_TreeNode  *EZ_ParseWidgetTreeA  MY_ANSIARGS((char *str, int which));
Window        EZ_SelectWindowWithButtonClick MY_ANSIARGS((void));
char         *EZ_PrintApplicationResources MY_ANSIARGS((void));
char         *EZ_PrintWidgetResources MY_ANSIARGS((EZ_Widget *widget));
char         *EZ_PrintWidgetResourcesAll MY_ANSIARGS((EZ_Widget *widget));
char         *EZ_PrintSetApplicationResources MY_ANSIARGS((char *msg, int len));
char         *EZ_PrintSetWidgetResources MY_ANSIARGS((EZ_Widget *widget, char *msg, int len));
char         *EZ_PrintSetWidgetResourcesN MY_ANSIARGS((char *msg, int len));
EZ_Item      **EZ_ParseResourcesTable MY_ANSIARGS((char *str, int *cntRet));
int          EZ_GetResourceTypes  MY_ANSIARGS((char *in, char *out));
int          EZ_GetResourceTypesInternal  MY_ANSIARGS((char *in, char *out, int all));

void         EZ_GetWidgetFullPath MY_ANSIARGS((EZ_Widget *widget, char **n, char **c));
void         EZ_GetWidgetFullPathQuarks MY_ANSIARGS((EZ_Widget *widget, XrmQuark **n, XrmQuark **c));
void         EZ_RecordResourceString MY_ANSIARGS((char **dest, char *src));

static char  *EZ_PrintWidgetResourcesWork MY_ANSIARGS((EZ_Widget *widget, int flag));
/**********************************************************************/
extern EZ_AllWidgetHTEntry  EZ_AllWidgetsHashTable[EZ_WIDGET_HASH_TABLE_SIZE];
/**********************************************************************/
typedef struct {
  unsigned long     id;
  unsigned long     win;
  XrmQuark          iname;
  XrmQuark          cname;
  char              *info;
} nodeData;
/**********************************************************************/
int  EZ_HandleResourceEditorMessage(msg)
     EZ_Message *msg;
{
  char *replyData = NULL;
  unsigned long uid=0;
  Window      win;
  EZ_Widget   *widget;  
  Atom        atom;
  int         handled = 0;

  switch(msg->messageId)
    {
    case EZ_RE_GET_TREE:
      {
        /* message broadcasted from the resource editor 
         * requesting the whole widget tree.
         */
        sscanf(msg->message, "%lx", &uid);
        win = (Window)uid;
        atom = (Atom)uid;
        if(atom == EZ_InstanceQName || atom == EZ_ClassQName ||
           (widget = EZ_LookupWidgetFromMappedHT(win)) != NULL)
          replyData = EZ_PrintWidgetTree();
      }
    break;
    case EZ_RE_GET_TREE1:
      {
        /* message broadcasted from the resource editor 
         * requesting one tree.
         */
        sscanf(msg->message, "%lx", &uid);
        win = (Window)uid;
        widget = EZ_LookupWidgetFromMappedHT(win);
        if(widget)  replyData = EZ_PrintOneWidgetTree(widget);
      }
    break;
    case EZ_RE_GET_RESOURCE:
      {
        /* message sent from the resource editor 
         * requesting the resource table of a widget.
         */
        sscanf(msg->message, "%lx", &uid);
        atom = (Atom)uid;
        if(atom == EZ_InstanceQName || atom == EZ_ClassQName)
          replyData = EZ_PrintApplicationResources();
        else
          {
            widget = (EZ_Widget *)uid;
            if(EZ_WidgetExist(widget))
              replyData = EZ_PrintWidgetResources(widget);
          }
      }
    break;
    case EZ_RE_SET_RESOURCE1: /* set one resource only */
      {
        char *resStr = msg->message + 20;
        int  length =  msg->messageLength - 20;
        if(length > 0)
          {
            sscanf(msg->message, "%lx", &uid);
            atom = (Atom)uid;
            if(atom == EZ_InstanceQName || atom == EZ_ClassQName)
              replyData = EZ_PrintSetApplicationResources(resStr, length);
            else
              {
                widget = (EZ_Widget *)uid;
                if(EZ_WidgetExist(widget))
                  replyData = EZ_PrintSetWidgetResources(widget, resStr,length);
              }
          }
      }
      break;
    case EZ_RE_INSTALL_RESOURCE:
      {
        char *resStr = msg->message + 20;
        int  length =  msg->messageLength - 20;
        if(length > 0)
          {
            XrmDatabase dspDb =NULL;
            resStr[length]= '\0';
            dspDb = XrmGetStringDatabase(resStr);            
            XrmCombineDatabase(dspDb, &EZ_Xrm_db, 1);            
          }
      }
    /* follow through */
    case EZ_RE_SET_RESOURCE:
      {
        char *resStr = msg->message + 20;
        int  length =  msg->messageLength - 20;
        if(length > 0)
          {
            sscanf(msg->message, "%lx", &uid);
            atom = (Atom)uid;
            if(atom == EZ_InstanceQName || atom == EZ_ClassQName)
              replyData = EZ_PrintSetApplicationResources(resStr, length);
            else 
              {
                widget = (EZ_Widget *)uid;
                if(EZ_WidgetExist(widget))
                  replyData = EZ_PrintSetWidgetResourcesN(resStr,length);
              }
          }
      }
    break;
    case EZ_RE_SET_RESOURCE2:
      {
        int  length =  msg->messageLength;
        char *resStr = msg->message;
        if(length > 0)
          {
            replyData = EZ_PrintSetApplicationResources(resStr, length);
            if(replyData)(void)my_free(replyData);
            replyData = EZ_PrintSetWidgetResourcesN(resStr,length);
          }
      }
     break;
    case EZ_RE_BLINK_WIDGET:
      {
        int  length =  msg->messageLength;
        if(length > 0)
          {
            sscanf(msg->message, "%lx", &uid);
            widget = (EZ_Widget *)uid;
            if(EZ_WidgetExist(widget))
              {
                EZ_ReplyMessage(msg, "ok ", 2);
                EZ_BlinkWidget(widget,3);
                handled = 1;
              }
          }
      }
     break;
    case EZ_RE_WHO_ARE_YOU:
      EZ_ReplyMessage(msg, "hi ", 2);
      handled = 1;
      break;
    default:
      break;
    }
  if(replyData != NULL)
    {
      EZ_ReplyMessage(msg, replyData, strlen(replyData));
      my_free(replyData);
      replyData = NULL;
      handled = 1;
    }
  return(handled);
}
/*****************************************************************************************/
static void ritemDestroyCb(item, data) EZ_Item *item; void *data;
{
  nodeData *nd = (nodeData *)data;
  if(nd)
    {
      if(nd->info) my_free(nd->info);
      my_free((char *)nd); 
    }
}
/**********************************************************************/

char *EZ_PrintApplicationResources()
{
  int  total, size = 1024;
  char *ptr, *result = (char *) my_malloc(1024 * sizeof(char), _RES_EDIT_);
  char rvtypes[256], tmpstr[256];
  
  EZ_ResourceHandle *rhandle= EZ_ApplicationResourceHandle;  
  result[0]='(';
  total = 1;
  while(rhandle)
    {
      int i, nn = rhandle->nresources;
      UDFResource *resources = rhandle->resources;
      for(i = 0; i < nn; i++)
        {
          char *inameS =EZ_QuarkToString(resources[i].iname);
          char *cnameS =EZ_QuarkToString(resources[i].cname);
          int tlen = EZ_GetResourceTypes(resources[i].vtypes, rvtypes);
          if(tlen > 0)
            {
              int len;
              sprintf(tmpstr, "[%s,%s,%d,%s]", inameS, cnameS, resources[i].option,rvtypes);
              len = strlen(tmpstr);
              if(total + len+8 >= size)
                {
                  size += size;
                  result = (char *)my_realloc(result, size * sizeof(char), _RES_EDIT_);
                }
              ptr = result + total;
              strcpy(ptr, tmpstr);
              total += len;
            }
        }
      rhandle = rhandle->next;
    }
  result[total++] = ')';
  result[total] = '\0';      
  return(result);
}
/**********************************************************************/
static char *EZ_PrintWidgetResourcesWork(widget, flag)
     EZ_Widget *widget; int flag;
{
  if(widget == NULL) return(NULL);
  else
    {
      /* 1. get common resources */
      int i, n, total, size = 2048;
      EZ_Value values[10];
      char *ptr, *result = (char *) my_malloc(2048 * sizeof(char), _RES_EDIT_);
      char rvtypes[256], tmpstr[256];

      result[0]='(';
      total = 1;
      for(i = 1; i <= EZ_WIDGET_CONFIGURE_LAST_OPTION; i++)
        {
          n=EZ_GetWidgetAttribute(widget, i, 0, values);
          if(n>0)
            {
              char *inameS = EZ_QuarkToString(EZ_OptionsNQs[i]);
              char *cnameS = EZ_QuarkToString(EZ_OptionsCQs[i]);
              int tlen = EZ_GetResourceTypesInternal(EZ_ConfigureOptionTypes[i], rvtypes, flag);
              if(tlen > 0)
                {
                  int len;
                  sprintf(tmpstr, "[%s,%s,%d,%s]", inameS, cnameS, i, rvtypes);
                  len = strlen(tmpstr);
                  if(total + len +8>= size)
                    {
                      size += size;
                      result = (char *)my_realloc(result, size * sizeof(char), _RES_EDIT_);
                    }
                  ptr = result + total;
                  strcpy(ptr, tmpstr);
                  total += len;
                }
            }
        }
      result[total] = ')';   result[total+1] = '\0';
      total++;
      /* 1. get registered resources */  
      {
        EZ_ResourceHandle *rhandle = EZ_WidgetResourceHandle(widget);
        result[total++] = '(';    
        while(rhandle)
          {
            int i, nn = rhandle->nresources;
            UDFResource *resources = rhandle->resources;
            for(i = 0; i < nn; i++)
              {
                char *inameS= EZ_QuarkToString(resources[i].iname);
                char *cnameS= EZ_QuarkToString(resources[i].cname);
                int tlen = EZ_GetResourceTypes(resources[i].vtypes, rvtypes);
                if(tlen > 0)
                  {
                    int len;
                    sprintf(tmpstr, "[%s,%s,%d,%s]", inameS, cnameS, 
                            resources[i].option, rvtypes);
                    len = strlen(tmpstr) + 3;
                    if(total + len >= size)
                      {
                        size += size;
                        result = (char *)my_realloc(result, size * sizeof(char), _RES_EDIT_);
                      }
                    ptr = result + total;
                    strcat(ptr, tmpstr);
                    total += len;
                  }
              }
            rhandle = rhandle->next;
          }
        result[total++] = ')';
        result[total] = '\0';
      }
      return(result);
    }
}
char *EZ_PrintWidgetResources(widget) EZ_Widget *widget;
{
  return(EZ_PrintWidgetResourcesWork(widget, 0));
}
char *EZ_PrintWidgetResourcesAll(widget) EZ_Widget *widget;
{
  return(EZ_PrintWidgetResourcesWork(widget, 1));
}
/**********************************************************************/
int EZ_CompareItem(aptr,bptr) void *aptr, *bptr;
{
  EZ_Item **va = (EZ_Item **)aptr;
  EZ_Item **vb = (EZ_Item **)bptr;
  EZ_Item *ap = *va;
  EZ_Item *bp = *vb;
  char    *a, *b;
  if(ap == NULL) return(1);
  else if( bp == NULL) return(-1);
  a = EZ_LabelItemString(ap);
  b = EZ_LabelItemString(bp);
  if(a == NULL) return(1);
  else if( b == NULL) return(-1);
  while(*a && *b && *a == *b) {a++; b++;}
  if(*a == 0) return(-1);
  else if(*b == 0) return(1);
  return( (*a > *b)? 1: -1);
}

EZ_Item **EZ_ParseResourcesTable(str, cntRet) char *str; int *cntRet;
{
  char *qtr, *ptr = str;
  EZ_Item *item, **items = NULL;
  int  i, nn = 0;
  if(ptr)
    {
      while(*ptr)
        { 
          if(*ptr == '[') nn++; 
          ptr++;
        }
    }
  if(nn)
    {
      char rvtype[256],inameS[128],cnameS[128], oidx[32];
      nodeData *nd;
      ptr = str;
      items = (EZ_Item **)my_malloc( (nn+1) * sizeof(EZ_Item *), _RES_EDIT);
      i = 0;
      while(i < nn && *ptr)
        {
          if(*ptr == '[')
            {
              ptr++;
              qtr = inameS;
              while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
              qtr = cnameS;
              while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
              qtr = oidx;
              while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
              qtr = rvtype;
              while(*ptr != ']') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;  
              qtr = (char *)my_malloc((strlen(rvtype) + 1)*sizeof(char), _RES_EDIT);
              strcpy(qtr, rvtype);
              nd = (nodeData *)my_malloc(sizeof(nodeData), _RES_EDIT);
              nd->info = qtr;
              nd->id = 0;
              nd->win = 0;
              nd->iname = EZ_StringToQuark(inameS); 
              nd->cname = EZ_StringToQuark(cnameS); 
              item = EZ_CreateItem(EZ_LABEL_ITEM, 
                                   EZ_LABEL_STRING,     inameS, 
                                   EZ_CLIENT_PTR_DATA,  (void *)nd,
                                   EZ_CLIENT_INT_DATA,  (int)atol(oidx),
                                   EZ_DESTROY_CALLBACK, ritemDestroyCb, (void *)nd, 
                                   EZ_BORDER_WIDTH,     2, 
                                   EZ_BORDER_TYPE,      EZ_BORDER_SHADOW, 0);
              items[i++] = item;
            }
          else ptr++;
        }
      items[i] = NULL;
      nn = i;
    }
  *cntRet = nn;
  qsort(items, nn, sizeof(EZ_Item *), EZ_CompareItem);
  return(items);
}
/**********************************************************************/
char *EZ_PrintSetApplicationResources(msg, len) char *msg; int len;
{
  char *str = (char *)my_malloc(64 * sizeof(char), _RES_EDIT);
  int status = 0;
  if(msg && len > 0)
    {
      XrmDatabase dspDb =NULL;
      msg[len]= '\0';
      dspDb = XrmGetStringDatabase(msg);
      if(dspDb)
        {
          status = EZ_DecodeApplicationResourcesWithDB(dspDb);
          XrmDestroyDatabase(dspDb);
          strcpy(str, "Resources Updated");
          return(str);
        }
    }
  strcpy(str, status? "Resources Updated" : "Couldn't update resource");
  return(str);
}
/**********************************************************************/
char *EZ_PrintSetWidgetResources(widget, msg, len) 
     EZ_Widget *widget; char *msg; int len;
{
  char *str = (char *)my_malloc(64 * sizeof(char), _RES_EDIT);
  int status = 0;
  if(msg && len > 0)
    {
      XrmDatabase dspDb = NULL;
      msg[len]= '\0';
      dspDb = XrmGetStringDatabase(msg);
      if(dspDb)
        {
          status = EZ_GetAndUseWidgetResources1WithDB(widget, dspDb);
          if(status)
            {
              EZ_ClearWidgetSizeComputedFlag(widget);
              if(EZ_WidgetIsViewable(widget)) EZ_DisplayWidget(widget);
            }
          XrmDestroyDatabase(dspDb);          
        }
    }
  strcpy(str, status? "Resources Updated" : "Couldn't update resource");
  return(str);
}
/**********************************************************************
 *
 * Action to EZ_RS_SET_RESOURCE message.
 */
char *EZ_PrintSetWidgetResourcesN(msg,len)
     char *msg; int len;
{
  char *str = (char *)my_malloc(64 * sizeof(char), _RES_EDIT);
  int status=0;
  if(msg && len > 0)
    {
      EZ_Widget *widget, *parent;
      EZ_AllWidgetHTEntry  *tmp;  
      int i, itmp;
      XrmDatabase dspDb = NULL;
      msg[len]= '\0';
      dspDb = XrmGetStringDatabase(msg);
      if(dspDb)
        {
          EZ_FreezeDisplay();  /* lock the display */
          for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
            {
              tmp = EZ_AllWidgetsHashTable[i].next;
              while(tmp)
                {
                  widget = tmp->widget;
                  if( (itmp = EZ_GetAndUseWidgetResources1WithDB(widget,dspDb)))
                    {
                      status++;
                      parent = widget;
                      while(parent)
                        {
                          EZ_ClearWidgetSizeComputedFlag(parent);
                          parent = EZ_WidgetParent(parent);
                        }
                    }
                  tmp = tmp->next;
                }
            }
          /* do the items */
          EZ_EditItemResources(dspDb);
          XrmDestroyDatabase(dspDb);
          EZ_ThawDisplay();  /* unlock display */
          if(status)
            {
              EZ_Widget *toplevels[256];
              int i, ntoplevels = 0;
              EZ_AllWidgetHTEntry  *tmp;  
              /* extract all toplevel widgets */
              for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
                {
                  tmp = EZ_AllWidgetsHashTable[i].next;
                  while(tmp)
                    {
                      if(EZ_WidgetParent(tmp->widget) == NULL)
                        toplevels[ntoplevels++] = tmp->widget;
                      tmp = tmp->next;
                    }
                }
              for(i = 0; i < ntoplevels; i++)
                {
                  if(EZ_WidgetIsViewable(toplevels[i]))
                    EZ_DisplayWidget(toplevels[i]);
                }
            }
        }
    }
  strcpy(str, status? "Resource updated" : "Couldn't update resource");
  return(str);
}
/**********************************************************************/

static int find_dlength(widget, level) EZ_Widget *widget; int level;
{
  int len = 0;
  if(widget)
    {
      char *iname, *cname;
      EZ_Widget *child  = EZ_WidgetChildren(widget);
      EZ_Widget *sibling = EZ_WidgetSibling(widget);
      EZ_GetWidgetCINames(widget, &cname, &iname);      
      len = strlen(iname) + strlen(cname) + 38 +(level + level);
      if(child)  len += find_dlength(child, level+1);
      if(sibling) { len++; len += find_dlength(sibling, level+1);}
    }
  return(len);
}


static void printWidgetTreeWork(widget, str) EZ_Widget *widget; char **str;
{
  if(widget)
    {
      char *cname, *iname, *ptr = *str;
      char buf[512];
      int  tlen;
      EZ_Widget *child = EZ_WidgetChildren(widget);
      EZ_Widget *sibling = EZ_WidgetSibling(widget);
      EZ_GetWidgetCINames(widget, &cname, &iname);
      sprintf(buf,"[%lx,%lx,%s,%s]",
              (unsigned long)widget, (unsigned long)EZ_WidgetWindow(widget),iname,cname);
      tlen = strlen(buf);
      strcpy(ptr, buf);
      ptr += tlen;
      if(child) 
        {
          *ptr++='(';
          *str = ptr;
          printWidgetTreeWork(child, str);
          ptr = *str;
          *ptr++=')';
          *str = ptr;
        }
      if(sibling)
        {
          *str = ptr;
          printWidgetTreeWork(sibling, str);
          ptr = *str;
        }
      *str = ptr;
      *ptr='\0';
    }
}

char *EZ_PrintOneWidgetTree(widget) EZ_Widget *widget;
{
  char *ptr, *str =NULL;
  if(widget)
    {
      char *iname, *cname;
      char buf[512];
      int len, tlen;
      cname = EZ_QuarkToString(EZ_ClassQName);
      iname = EZ_QuarkToString(EZ_InstanceQName);
      sprintf(buf,"[%lx,0,%s,%s](",  (unsigned long)EZ_InstanceQName, iname, cname);
      tlen = strlen(buf);
      len = find_dlength(widget, 1) + tlen + 10;
      str = my_malloc( (len+16) * sizeof(char), _RES_EDIT);
      if(str == NULL) EZ_OutOfMemory("EZ_PrintWidgetWidgetTree");
      strcpy(str, buf);
      ptr = str + tlen;
      printWidgetTreeWork(widget, &ptr);      
      *ptr++=')';
      *ptr = '\0';
    }
  return(str);
}

  
char *EZ_PrintWidgetTree()
{
  char names[256], *ptr, *str =NULL;
  EZ_Widget *toplevels[320];
  int i, ntoplevels = 0, len, tlen;
  EZ_AllWidgetHTEntry  *tmp;  

  /* first, list widget in widget_items */
  EZ_ListAllWidgetItems(toplevels, &ntoplevels);

  /* extract all toplevel widgets */
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp)
        {
          if(EZ_WidgetParent(tmp->widget) == NULL)
            toplevels[ntoplevels++] = tmp->widget;
          tmp = tmp->next;
          if(ntoplevels >= 320) break;
        }
    }
  sprintf(names, "[%lx,0,%s,%s](", (unsigned long)EZ_InstanceQName,
          EZ_QuarkToString(EZ_InstanceQName), EZ_QuarkToString(EZ_ClassQName));
  tlen = strlen(names);
  len = tlen+2;
  for(i = 0; i < ntoplevels; i++) len +=  find_dlength(toplevels[i], 1);
  str = my_malloc( (len+1) * sizeof(char), _RES_EDIT);
  if(str == NULL) EZ_OutOfMemory("EZ_PrintWidgetWidgetTree");
  strcpy(str,names);
  ptr = str + tlen;
  for(i = 0; i < ntoplevels; i++)
    {
      printWidgetTreeWork(toplevels[i], &ptr);      
    }
  *ptr++=')';
  *ptr = '\0';
  return(str);
}
/***************************************************************************/

static int EZ_ParseWidgetTreeWork(parent, str, which, nret)
     EZ_TreeNode *parent, **nret; char *str; int which;
{
  char *qtr, *ptr = str, s[80];
  nodeData *sdata;
  EZ_Item *item = NULL;
  EZ_TreeNode *newNode = NULL, *retNode=NULL;
  char idS[256],winS[256],inameS[256],cnameS[256];

  if(*ptr == '[') 
    {
      ptr++;
      qtr = idS;     while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
      qtr = winS;    while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
      qtr = inameS;  while(*ptr != ',') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;
      qtr = cnameS;  while(*ptr != ']') { *qtr++ = *ptr++;} *qtr='\0'; ptr++;      
      sdata = (nodeData *)my_malloc(sizeof(nodeData),_RES_EDIT);
      sscanf(idS, "%lx", (unsigned long *)&(sdata->id));
      sscanf(winS, "%lx", (unsigned long *)&(sdata->win));
      sdata->iname = EZ_StringToQuark(inameS);
      sdata->cname = EZ_StringToQuark(cnameS);
      sdata->info = NULL;
      switch(which)
        { 
        case 1: sprintf(s, "id: 0x%s", idS);     break;
        case 2: sprintf(s, "win: 0x%s",winS);    break;
        case 3: sprintf(s, "%s", cnameS);  break;
        default:sprintf(s, "%s", inameS);  break;
        }
      item = EZ_CreateItem(EZ_LABEL_ITEM, EZ_LABEL_STRING, s, 
                           EZ_CLIENT_PTR_DATA, sdata,
                           EZ_DESTROY_CALLBACK, ritemDestroyCb, (void *)sdata, 
                           0);
      newNode = EZ_CreateTreeNode(parent,item);
      *nret = newNode;

      /* parse descendants  */
      if(*ptr == '(')
        {
          ptr++;
          ptr += EZ_ParseWidgetTreeWork(newNode, ptr, which, &retNode);
          ptr++;
        }
      /* parse siblings */
      if(*ptr == '[')
        {
          ptr += EZ_ParseWidgetTreeWork(parent, ptr, which, &retNode);
        }
    }
  return((int)(ptr-str));
}

EZ_TreeNode  *EZ_ParseWidgetTree(str) char *str;
{
  return(EZ_ParseWidgetTreeA(str, 0));
}

EZ_TreeNode  *EZ_ParseWidgetTreeA(str, which) char *str; int which;
{
  EZ_TreeNode *node;
  /* create a header node */
  EZ_ParseWidgetTreeWork(NULL, str, which, &node);
  EZ_TreeSetCharacter(node, EZ_BRANCHED_LIST_TREE, 20, 2, 2);  
  return(node);
}
/**********************************************************************/

Window EZ_SelectWindowWithButtonClick()
{
  XEvent xevent;
  Window root = RootWindow(EZ_Display, EZ_ScreenNum);
  Window win = None;
  int clicks = 0;
  Cursor cursor = EZ_GetCursor(EZ_C_CROSSHAIR);
  int status = XGrabPointer(EZ_Display, root, False,
			    ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			    GrabModeAsync, root, cursor, CurrentTime);
  if(status != GrabSuccess) {return(None);}


  while(win == None || clicks != 0)
    {
      XAllowEvents(EZ_Display, SyncPointer, CurrentTime);
      XWindowEvent(EZ_Display, root, ButtonPressMask|ButtonReleaseMask, &xevent);
      switch(xevent.type) 
	{
	case ButtonPress:
	  if(win == None) 
	    {
	      win = xevent.xbutton.subwindow;
	      if(win == None) win = root;
	    }
	  clicks++;
	  break;
	case ButtonRelease:
	  if(clicks > 0)   clicks--; 
	  break;
	default: 
	  break;
	}
    } 
  XUngrabPointer(EZ_Display, CurrentTime);
  return(EZ_FindClientWindow(win));
}
/**********************************************************************/
int EZ_GetResourceTypes(in, out)
     char *in, *out;
{
  if(in && out)
    {
      int i, len = (int)in[0];
      out[0] = '\0';
      for(i = 0; i <= len; i++)
        {
          int kk = (int)(in[i+1]);
          switch(kk)
            {
            case EZ_BOOL:
              strcat(out, "bool ");              
              break;
            case EZ_ENUM:
              strcat(out, "enum ");              
              break;
            case EZ_INT:
              strcat(out, "int ");
              break;
            case EZ_STRING:
              strcat(out, "string ");
              break;
            case EZ_FONTNAME:
              strcat(out, "fontName ");
              break;
            case EZ_FILENAME:
              strcat(out, "fileName ");
              break;
            case EZ_COLOR:
              strcat(out, "color ");
              break;
            case EZ_ULONG:
              strcat(out, "uint ");
              break;
            case EZ_FLOAT:
              strcat(out, "float ");
              break;
            case EZ_FUNCTION:
              strcat(out, "function ");              
              break;
            case EZ_REPEAT:
              strcat(out, "... ");
              break;
            default:
              break;
            }
        }
      len =strlen(out);
      if(len > 0) {out[len-1]='\0'; len--;};
      return(len);
    }
  return(0);
}
/**********************************************************************/
int EZ_GetResourceTypesInternal(in, out, flag)
     char *in, *out; int flag;
{
  if(in && out)
    {
      int i, len = strlen(in);
      out[0] = '\0';
      for(i = 0; i < len; i++)
        {
          switch(in[i])
            {
            case 'p':   
              if(flag)  strcat(out, "pointer ");
              else      return(0);  
              break;
            case 'i':   strcat(out, "int ");        break;
            case 'S':   strcat(out, "string ");     break;
            case 'f':   strcat(out, "float ");      break;
            case 'e':   strcat(out, "enum ");       break;
            case 'b':   strcat(out, "bool ");       break;
            case 'r':   strcat(out, "color ");      break;
            case 'F':   strcat(out, "fileName ");   break;
            case 'C':   strcat(out, "CursorName "); break;
            case 'h':   strcat(out, "Function ");   break;
            case '.':   strcat(out, "... ");        break;
            default:    break;
            }
        }
      len = strlen(out);
      if(len > 0) {out[len-1]='\0'; len--;};
      return(len);
    }
  return(0);
}
/**********************************************************************/
void EZ_GetWidgetFullPath(widget, nres, cres)
     EZ_Widget *widget; char **nres, **cres;
{
  EZ_Widget *parent;
  XrmQuark  clist[128], nlist[128];
  char tmpstr[1024];
  int i, j;
  
  if(EZ_LookupWidgetFromAllHT(widget) == NULL) return;

  parent = widget;
  i = 127;
  while(parent)
    {
      clist[i] = EZ_WidgetCName(parent);
      nlist[i] = EZ_WidgetIName(parent);	  
      parent = EZ_WidgetParent(parent);
      i--;
    }
  clist[0] = EZ_ClassQName;
  nlist[0] = EZ_InstanceQName;
  for(j = 1, i = i+1; i < 128; i++, j++)
    {
      clist[j] = clist[i];
      nlist[j] = nlist[i];
    }
  clist[j] = NULLQUARK; 
  nlist[j] = NULLQUARK; 
  if(nres)
    {
      char *str;
      tmpstr[0] = 0;
      j = 0;
      while(nlist[j] != NULLQUARK)
        {
          strcat(tmpstr, ".");
          strcat(tmpstr, EZ_QuarkToString(nlist[j]));
          j++;
        }
      str = (char *)malloc((strlen(tmpstr)+1) *sizeof(char));
      strcpy(str, tmpstr);
      *nres = str;
    }
  if(cres)
    {
      char *str;
      tmpstr[0] = 0;
      j = 0;
      while(clist[j] != NULLQUARK)
        {
          strcat(tmpstr, ".");
          strcat(tmpstr, EZ_QuarkToString(clist[j]));
          j++;
        }
      str = (char *)malloc((strlen(tmpstr)+1) *sizeof(char));
      strcpy(str, tmpstr);
      *cres = str;
    }
}
void EZ_GetWidgetFullPathQuarks (widget, nres, cres)
     EZ_Widget *widget; XrmQuark **nres, **cres;
{
  EZ_Widget *parent;
  XrmQuark  clist[128], nlist[128];
  int i, j;
  
  if(EZ_LookupWidgetFromAllHT(widget) == NULL) return;

  parent = widget;
  i = 127;
  while(parent)
    {
      clist[i] = EZ_WidgetCName(parent);
      nlist[i] = EZ_WidgetIName(parent);	  
      parent = EZ_WidgetParent(parent);
      i--;
    }
  clist[0] = EZ_ClassQName;
  nlist[0] = EZ_InstanceQName;
  for(j = 1, i = i+1; i < 128; i++, j++)
    {
      clist[j] = clist[i];
      nlist[j] = nlist[i];
    }
  clist[j] = NULLQUARK; 
  nlist[j] = NULLQUARK; 
  if(nres)
    {
      XrmQuark *qk = (XrmQuark *)malloc( (j+2) * sizeof(XrmQuark));
      i = 0;
      while(nlist[i] != NULLQUARK)
        { qk[i] = nlist[i]; i++;}
      qk[i++] = NULLQUARK; qk[i++] = NULLQUARK;
      *nres = qk;
    }
  if(cres)
    {
      XrmQuark *qk = (XrmQuark *)malloc( (j+2) * sizeof(XrmQuark));
      i = 0;
      while(clist[i] != NULLQUARK)
        { qk[i] = clist[i]; i++;}
      qk[i++] = NULLQUARK; qk[i++] = NULLQUARK;
      *cres = qk;
    }
}
/**********************************************************************
 *
 * Append Resource strint to dest
 */
void EZ_RecordResourceString(dest, src)
     char **dest, *src;
{
  if(dest && src)
    {
      int nlen = strlen(src);
      if(*dest)
        {
          nlen += strlen(*dest) + 2;
          *dest = (char *)my_realloc(*dest, nlen * sizeof(char), _RES_EDIT_);
          strcat(*dest, "\n");
        }
      else
        {
          *dest = (char *)my_malloc(nlen * sizeof(char), _RES_EDIT_);
          (*dest)[0] = '\0';
        }
      strcat(*dest, src);
    }
}
/**********************************************************************/
#undef   _EZ_RESOURCE_EDIR_C_


