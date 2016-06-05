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
#define _EZ_RESOURCE_C_
#include "EZ_Widget.h"
#include "EZ_CQuarks.h"

/*****************************************************************/
int   EZ_InstallSymbolicInt   MY_ANSIARGS((char *name, int value));
int   EZ_RetrieveSymbolicInt  MY_ANSIARGS((char *name, int *value_ret));

int   EZ_InstallSymbolicFloat   MY_ANSIARGS((char *name, float value));
int   EZ_RetrieveSymbolicFloat  MY_ANSIARGS((char *name, float *value_ret));

int   EZ_InstallSymbolicPointer   MY_ANSIARGS((char *name, void *value));
int   EZ_RetrieveSymbolicPointer  MY_ANSIARGS((char *name, void **value_ret));

int   EZ_InstallSymbolicULong   MY_ANSIARGS((char *name, unsigned long value));
int   EZ_RetrieveSymbolicULong  MY_ANSIARGS((char *name, unsigned long *value_ret));

int EZ_GetApplicationResource MY_ANSIARGS((char *rname, char **v_ret));
int EZ_GetWidgetResource MY_ANSIARGS((EZ_Widget *, char *rname, char **v_ret));
int EZ_GetItemResource   MY_ANSIARGS((EZ_Item *, char *rname, char **v_ret));
/*****************************************************************/
int       EZ_InstallSymbol  MY_ANSIARGS((char *name, EZ_Value value));
EZ_Value  *EZ_RetrieveSymbol MY_ANSIARGS((char *name));

int   EZ_ValuesEQ MY_ANSIARGS((EZ_Value a, EZ_Value b));
/*****************************************************************/
XrmDatabase  *EZ_GetResourceDatabase MY_ANSIARGS(( void ));
/*****************************************************************/
void EZ_DestroyXrmDB          MY_ANSIARGS((void));
char *EZ_GetInstanceName      MY_ANSIARGS((void));
char *EZ_GetClassName         MY_ANSIARGS((void));
int  EZ_InitXrmAndX11 MY_ANSIARGS((char *className,
				   XrmOptionDescRec *optstable,  int nopts,
				   int *ac, char **av, char *fbkstr, int initgl));

void  EZ_SetApplicationName MY_ANSIARGS((char *name));
char  *EZ_GetApplicationName MY_ANSIARGS((void));

void  EZ_SetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char *cn, char *in));
void  EZ_GetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char **cn, char **in));
void  EZ_SetDefaultFontsFromResources MY_ANSIARGS((void));
void  EZ_SetDefaultAttributesFromResources MY_ANSIARGS((void));
int   EZ_GetWidgetType  MY_ANSIARGS((EZ_Widget *w));

int   EZ_GetOptionCINames MY_ANSIARGS((int i, char **cn, char **in));
/*****************************************************************/
static void displayDB MY_ANSIARGS((void));
static void defineQuarkSymbolicConstants MY_ANSIARGS((void));

/*****************************************************************/
XrmDatabase EZ_Xrm_db = (XrmDatabase)NULL;   /* the global option data base, merged
					      * from all possible sources */

/**/
EZ_SymbQConst *EZ_QConstants;
int           EZ_NumQConstants, EZ_NumQCInUse;  /* - sign signify out of order */
/*****************************************************************
 *
 *        QUARKS QUARKS QUARKS QUARKS QUARKS
 *
 *  1. A few quarks used for setting global attributes of EZWGL 
 *     Used only in EZ_ExtractGlobalResources()
 */
#define EZ_QuarkStringPair(_varN_, _def_) static XrmQuark _varN_;
#include "EZ_GQuarks.h"
#undef EZ_QuarkStringPair
/*
 * 2. Quarks used for configuring widgets. Not all
 *    configuration options are supported. 
 */
XrmQuark EZ_OptionsNQs[EZ_WIDGET_CONFIGURE_LAST_OPTION+1];
XrmQuark EZ_OptionsCQs[EZ_WIDGET_CONFIGURE_LAST_OPTION+1];
/*****************************************************************
 *
 *  Standard Command Line Options
 */
static XrmOptionDescRec EZ_StandardCommandLineOptions[] = {
  {"-display",          ".display",          XrmoptionSepArg,  NULL},
  {"-visual",           ".visual",           XrmoptionSepArg,  NULL},
  {"-geometry",         ".geometry",         XrmoptionSepArg,  NULL},
  {"-background",       ".background",       XrmoptionSepArg,  NULL},
  {"-bg",               ".background",       XrmoptionSepArg,  NULL},
  {"-foreground",       ".foreground",       XrmoptionSepArg,  NULL},
  {"-fg",               ".foreground",       XrmoptionSepArg,  NULL},
  {"-iconic",           ".initialState",     XrmoptionNoArg,  "iconicState"},
  {"-noBackingStore",   ".noBackingStore",   XrmoptionNoArg,  "True"},
  {"-privateColormap",  ".privateColormap",  XrmoptionNoArg,  "True"},
  {"-readWriteColormap",".readWriteColormap",XrmoptionNoArg,  "True"},
  {"-dualLabel",        ".dualLabel",        XrmoptionNoArg,  "True"},
};

/*****************************************************************/
void EZ_PrintStandardOptions()
{
  fprintf(stderr, "   Standard Options are as follows:\n");
  fprintf(stderr, "\t-display    DISPLAY\n");
  fprintf(stderr, "\t-visual     VISUAL\n");
  fprintf(stderr, "\t-geometry   [WIDTHxHEIGHT][{+-}XOFFSET{+-}YOFFSET]\n");
  fprintf(stderr, "\t-background COLOR\n");
  fprintf(stderr, "\t-bg         COLOR\n");
  fprintf(stderr, "\t-foreground COLOR\n");
  fprintf(stderr, "\t-fg         COLOR\n");
  fprintf(stderr, "\t-dualLabel\n");
  fprintf(stderr, "\t-iconic\n");
  fprintf(stderr, "\t-noBackingStore\n");
  fprintf(stderr, "\t-privateColormap\n");
  fprintf(stderr, "\t-readWriteColormap\n");
  fprintf(stderr, "    Other options are:\n");
  fprintf(stderr, "\t-xrm       resourceString\n");
  fprintf(stderr, "\t-xrmfile   resourceFile\n");
  fprintf(stderr, "\t-name      nameString\n");
  fprintf(stderr, "\t-className nameString\n");
  fprintf(stderr, "\t-dispXrmDB\n");
  fprintf(stderr, "\t-synchronous\n");
}
/*****************************************************************/
char *EZ_GetInstanceName()
{
  return(EZ_QuarkToString(EZ_InstanceQName));
}
/*****************************************************************/
char *EZ_GetClassName()
{
  return(EZ_QuarkToString(EZ_ClassQName));
}
/*****************************************************************/
char  *EZ_GetApplicationName()
{
  return(EZ_QuarkToString(EZ_InstanceQName));
}
/*****************************************************************/
void  EZ_SetApplicationName(name)
     char *name;
{
  XrmInitialize();
  EZ_InstanceQName = EZ_StringToQuark(name);  
}
/*****************************************************************/
Bool enumdb(XrmDatabase *nu, XrmBindingList bindings,
	    XrmQuarkList quarks, XrmRepresentation *type,
	    XrmValue *value, XPointer closure) 
{
  int i = 0;
  while (quarks[i] != NULLQUARK)
    {
      fprintf(stderr, "\t %d  %s: [%s] [%d]\n",
	      bindings[i], EZ_QuarkToString(quarks[i]), value->addr, value->size);
      i++;
    }
  return False;
}
static void displayDB()
{
  XrmName names[2];
  XrmClass classes[2];
  names[0] = (XrmName) EZ_InstanceQName; names[1] = NULLQUARK;
  classes[0] = (XrmClass) EZ_ClassQName; classes[1]= NULLQUARK;
  
  fprintf(stderr, "ClassName = [%s]   ", EZ_QuarkToString(EZ_ClassQName));
  fprintf(stderr, "InstanceName = [%s]\n", EZ_QuarkToString(EZ_InstanceQName));
  XrmEnumerateDatabase(EZ_Xrm_db, names, classes, XrmEnumAllLevels, enumdb, 
		       (XPointer)NULL);
}
/*****************************************************************
 *
 * This routine is called from EZ_Init and EZ_Initialize.
 * It needs to know the instance name and classname of the
 * application. In EZ_Init, they are set before this routine
 * is invoked. In EZ_Initialize, they are set here.
 */
int EZ_InitXrmAndX11(className, optstable, nopts, 
		      argc, argv, fbkresrc, initgl) 
     char *className;
     XrmOptionDescRec *optstable;
     int nopts;
     int *argc; 
     char *argv[];
     char *fbkresrc;
     int initgl;
{
  char name[1024];
  char *dpy_name = (char *)NULL;
  char *bgColor = (char *)NULL;
  char *fgColor = (char *)NULL;
  char *geometry = (char *)NULL;
  char *iname = (char *)NULL;
  char *cname = (char *)NULL;
  char *xrmfile = (char *)NULL;
  char *xrmstring[100];
  int dispXrmDB = 0, nxrmstring = 0;
  int  i, c, cmaptype = EZ_DEFAULT_COLORMAP, backing_store = 1, suggested_v = -1;
  int  synchr = 0;
  int  ac = *argc;
  char **av = argv;

  XrmInitialize(); 

  /* Initialize Quarks */
  /* 1. default widget class names */
  for(i = 0; i < 128; i++)
    {
      strcpy(name, EZ_WidgetDefaultClassNames[i]);
      EZ_WidgetDefaultQClassNames[i] =
        EZ_PermStringToQuark(EZ_WidgetDefaultClassNames[i]);
      c = name[0];
      if(c >= 'A' && c <= 'Z') name[0] = c - 'A'  + 'a';
      EZ_WidgetDefaultQInstanceNames[i] = EZ_StringToQuark(name);
    }
  for(i = 0; i < 10; i++)
    {
      strcpy(name, EZ_ItemDefaultClassNames[i]);
      EZ_ItemDefaultQClassNames[i] =
        EZ_PermStringToQuark(EZ_ItemDefaultClassNames[i]);
      c = name[0];
      if(c >= 'A' && c <= 'Z') name[0] = c - 'A'  + 'a';
      EZ_ItemDefaultQInstanceNames[i] = EZ_StringToQuark(name);
    }
  /* 2. Quarks for global attributes */
#define EZ_QuarkStringPair(_varN_, _def_) _varN_ = XrmPermStringToQuark(_def_) ;
#include "EZ_GQuarks.h"
#undef EZ_QuarkStringPair

  /* 3. Quarks corresponding to configuration options */
  for(i = 0; i <= EZ_WIDGET_CONFIGURE_LAST_OPTION; i++)
    { EZ_OptionsNQs[i] = NULLQUARK;  EZ_OptionsCQs[i] = NULLQUARK;}

#define EZ_OptionIdNames(_CID_, _TYPE_, _ONAME_, _NQDef_, _CQDef_) \
  {\
     EZ_OptionsNQs[_CID_] = XrmPermStringToQuark(_NQDef_); \
     EZ_OptionsCQs[_CID_] = XrmPermStringToQuark(_CQDef_); \
  }
#include "EZ_WidgetConfig.h"
#undef EZ_OptionIdNames
  
  /* 4. Symbolic Constants */
  defineQuarkSymbolicConstants();


  /* parse command line, extract generic options */
  if(ac > 1)
    {
      for(i = 1; i < ac; i++)
	{
	  char *arg = av[i];
	  if(arg[0] == '-')
	    {
	      switch(arg[1])
		{
		case 'd':                   /* -display */
		  if(!strcmp(arg,"-display"))
		    {
		      if(++i < ac) dpy_name = av[i];
		      else --i;
		    }
		  else if(!strcmp(arg,"-dispXrmDB"))
		    {
		      dispXrmDB = 1; av[i] = NULL;
		    }
		  break;
		case 'n':
		  if(!strcmp(arg,"-name"))
		    {
		      if(++i < ac) { iname = av[i]; av[i-1] = av[i] = NULL; }
		      else --i;
		    }
		  break;  
		case 'c':
		  if(!strcmp(arg,"-className"))
		    {
		      if(++i < ac) { cname = av[i]; av[i-1] = av[i] = NULL; }
		      else --i;
		    }
		  break;  
                case 's':
                  if(!strncmp(arg,"-synchronous", 8))
                    { synchr = 1;  av[i] = NULL;}
                  break;
		case 'x':
		  if(!strcmp(arg,"-xrm"))
		    {
		      if(++i < ac) 
                        { 
                          if(nxrmstring < 100) xrmstring[nxrmstring++] = av[i];
                          av[i-1] = av[i] = NULL; 
                        }
		      else --i;
		    }
		  else  if(!strncmp(arg,"-xrmfile",5))
		    {
		      if(++i < ac) { xrmfile = av[i]; av[i-1] = av[i] = NULL; }
		      else --i;
		    }
		  break;
		case 'E': /* -EmBeD  commuWin parentWin x y */ /* internally used only */
		  if(!strncmp(av[i],"-EmBeD", 4) && i + 1 < ac)
		    {
		      sscanf(av[i+1], "%lx_%lx",(unsigned long *)&EZ_DnDCommuWin,
			     (unsigned long *)&EZ_DnDParentWin);
		      av[i] = av[i+1] = NULL;
		      i += 1;
		    }
		  else if(!strncmp(av[i],"-EDepTh", 4))
		    {
		      if(++i < ac) { EZ_EmbedingDepth = atoi(av[i]); av[i-1] = av[i] = NULL;}
		      else --i;
		    }
		  break;
		default:
		  break;
		}
	    }
	}
      /* now, reset ac and av */
      for(i = 1, c = 1; i < ac; i++)
	{
	  while(av[i] == NULL && i < ac) i++;
	  if(i < ac) av[c++] = av[i];
	  *argc = c;
	}
    }
  /* set instance name */
  if(iname != NULL) 
    {
      strcpy(name, iname);
      EZ_InstanceQName = EZ_StringToQuark(name);  
    }
  else if(EZ_InstanceQName == NULLQUARK)
    {
      char *ptr = getenv("RESOURCE_NAME");
      if(ptr == NULL)
	{
	  ptr = strrchr(av[0], '/');
	  if(ptr) ptr++; else ptr = av[0];
	}
      strcpy(name, ptr);
      EZ_InstanceQName = EZ_StringToQuark(name);  
    }

  /* set class name */
  if(cname != NULL) strcpy(name, cname);  /* testing and debuging */
  else if(className != NULL) strcpy(name, className);
  else
    {
      char *ptr = strrchr(av[0], '/');
      if(ptr) ptr++; else ptr = av[0];
      strcpy(name, ptr);
      c = name[0];
      if(c >= 'a' && c <= 'z') name[0] = c - 'a'  + 'A';      
      if(name[0] == 'X')
	{
	  c = name[1];
	  if(c >= 'a' && c <= 'z') name[1] = c - 'a'  + 'A'; 
	}
    }
  EZ_ClassQName = EZ_StringToQuark(name);	

  /*-----------------------------------------------
   *  Has to open display first in order to read
   *  say, SCREENRESOURCE ...
   *----------------------------------------------*/
  if( (EZ_Display = XOpenDisplay(dpy_name)) == NULL)
    {
      (void) fprintf( stderr, "Couldn't connect to X server %s\n",
		      XDisplayName(dpy_name));
      return(-1);
    }
  EZ_ScreenNum = DefaultScreen(EZ_Display);   
  if(synchr) XSynchronize(EZ_Display, True);

  EZ_InstanceAName = XInternAtom(EZ_Display, EZ_QuarkToString(EZ_InstanceQName) ,False);
  EZ_ClassAName =  XInternAtom(EZ_Display, EZ_QuarkToString(EZ_ClassQName), False);
  
  /*
   * will finish X initialization after resource database is in.
   */ 

  /*---------------------------------------------------------
   * Build resource data base, this is kind of crazy ! 
   *--------------------------------------------------------*/

  /*
   * 1. Parse fallback resource. They are of the lowest priority.
   */
  if(fbkresrc != NULL)
    {
      XrmDatabase fbk_db = XrmGetStringDatabase(fbkresrc);
      XrmCombineDatabase(fbk_db, &EZ_Xrm_db, 1);
    }
  /*
   * 2. Read application defaults, if there is one.
   */
  sprintf(name, "/usr/lib/X11/app-defaults/%s", EZ_GetClassName());
  if(access(name, R_OK) == 0)
    {
      XrmCombineFileDatabase(name, &EZ_Xrm_db, 1);
    }
  
  /* 
   * 3. check XAPPLRESDIR and XUSERFILESEARCHPATH env. 
   */
  {
    char *cptr = getenv("XAPPLRESDIR");
    if(cptr)
      {
	sprintf(name,"%s/%s", cptr, EZ_GetClassName());
	if(access(name, R_OK) == 0)
	  {
	    XrmCombineFileDatabase(name, &EZ_Xrm_db, 1);	  
	  }
      }
    cptr = getenv("XUSERFILESEARCHPATH");
    if(cptr)
      {
	sprintf(name,"%s/%s", cptr, EZ_GetClassName());
	if(access(name, R_OK) == 0)
	  {
	    XrmCombineFileDatabase(name, &EZ_Xrm_db, 1);	  
	  }
      }
  }

  /* 
   * 4. Check RESOURCE_MANAGER, the R4 way
   */
  {
    char *cptr = XResourceManagerString(EZ_Display);
    if(cptr)
      {
	XrmDatabase dspDb = XrmGetStringDatabase(cptr);
	XrmCombineDatabase(dspDb, &EZ_Xrm_db, 1);
      }
  }
  /* 
   * 5. Check SCREEN_RESOURCE, the R5 way
   */
  {
    char *cptr = XScreenResourceString(DefaultScreenOfDisplay(EZ_Display));
    if(cptr != NULL)
      {
	XrmDatabase scrnDb = XrmGetStringDatabase(cptr);
	XrmCombineDatabase(scrnDb, &EZ_Xrm_db, 1);
      }
  }
  
  /* 
   * 6. check HOME/.Xdefaults
   */
  {
    char *cptr = getenv("HOME");
    if(cptr)
      {
	sprintf(name, "%s/%s", cptr, ".Xdefaults");
	if(access(name, R_OK) == 0)
	  {
	    XrmCombineFileDatabase(name, &EZ_Xrm_db, 1);	  	
	  }
      }
  }

  /* 
   * 7. check XENVIRONMENT or Xdefault-hostname 
   */
  {
    char *cptr = getenv("XENVIRONMENT");
    if(cptr == NULL)
      {
	char *t = getenv("HOME");
	if(t != NULL)
	  {
	    int len;
	    sprintf(name, "%s/.Xdefaults-", t);
	    len = strlen(name);
	    if(gethostname(name+len, 512) == 0)
	      {
		t = name+len;
		while(*t != 0) {if(*t == '.') {*t = 0; break;} t++;}
		cptr = name;
	      }
	  }
      }
    if(cptr && access(cptr, R_OK) == 0)
      {
	XrmCombineFileDatabase(cptr, &EZ_Xrm_db, 1);	  	
      }
  }

  /* 
   * 8. command line xrmfile
   */
  if(xrmfile != NULL)
    {
      if(access(xrmfile, R_OK) == 0)
	{
	  XrmCombineFileDatabase(xrmfile, &EZ_Xrm_db, 1);	  
	}
    }
  
  /* 
   * 9. command line xrm string
   */
  if(nxrmstring)
    {
      for(i = 0; i < nxrmstring; i++)
        {
          XrmDatabase tdb = XrmGetStringDatabase(xrmstring[i]);
          XrmCombineDatabase(tdb, &EZ_Xrm_db, 1);
        }
    }
  /* 
   * 10. Parse command line for standard options.
   */
  if(1)
    {
      XrmDatabase cmdDb = NULL;
      XrmParseCommand(&cmdDb, EZ_StandardCommandLineOptions,
		      EZ_ArraySize(EZ_StandardCommandLineOptions),
		      EZ_GetInstanceName(), argc, argv);  
      XrmCombineDatabase(cmdDb, &EZ_Xrm_db, 1);
    }
  /* 
   * 11. Parse the rest of the command line. overrides all!
   */
  if(nopts > 0)
    {
      XrmDatabase cmdDb = NULL;
      XrmParseCommand(&cmdDb, optstable, nopts,
		      EZ_GetInstanceName(), argc, argv);  
      XrmCombineDatabase(cmdDb, &EZ_Xrm_db, 1);
    }

  if(dispXrmDB)  displayDB();
  
  /*------------------------------------
   * now finish up X11 initialization 
   *-----------------------------------*/
  {
    XrmQuark  inameQ[3], cnameQ[3], Q_type[3];/* not used */
    XrmValue value;

    inameQ[0] = EZ_InstanceQName;
    cnameQ[0] = EZ_ClassQName;
    inameQ[2] = cnameQ[2] = NULLQUARK;

    /* =*= bg */
    inameQ[1] = EZ_OptionsNQs[EZ_BACKGROUND];
    cnameQ[1] = EZ_OptionsCQs[EZ_BACKGROUND];
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      bgColor = value.addr;

    /* =*= fg */
    inameQ[1] = EZ_OptionsNQs[EZ_FOREGROUND];
    cnameQ[1] = EZ_OptionsCQs[EZ_FOREGROUND];
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      fgColor = value.addr;

    /* =*= geometry */
    inameQ[1] = EZ_OptionsNQs[EZ_GEOMETRY];
    cnameQ[1] = EZ_OptionsCQs[EZ_GEOMETRY];
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      geometry = value.addr;
    
    /* =*= visual */
    inameQ[1] = Qk_visual;
    cnameQ[1] = Qk_Visual;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      {
	char *ptr = value.addr;
	if(!strncmp(ptr,"pseudo",4) || !strncmp(ptr,"Pseudo",4))
	  suggested_v = PseudoColor;
	else if(!strncmp(ptr,"true",4) || !strncmp(ptr,"True",4))
	  suggested_v = TrueColor;
	else if(!strncmp(ptr,"direct",4) || !strncmp(ptr,"Direct",4))
	  suggested_v = DirectColor;
	else if(!strncmp(ptr,"gray",4) || !strncmp(ptr,"Gray",4))
	  suggested_v = GrayScale;
	else if(!strncmp(ptr,"staticg",7) || !strncmp(ptr,"staticG",7) ||
		!strncmp(ptr,"Staticg",7) || !strncmp(ptr,"StaticG",7))
	  suggested_v = StaticGray;
	else if(!strncmp(ptr,"staticc",7) || !strncmp(ptr,"staticC",7) ||
		!strncmp(ptr,"Staticc",7) || !strncmp(ptr,"StaticC",7))
	  suggested_v = StaticColor;
      }
    
    /* =*= privateColormap */
    inameQ[1] = Qk_privateColormap;
    cnameQ[1] = Qk_PrivateColormap;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      cmaptype = EZ_PRIVATE_COLORMAP;
    else
      {
	inameQ[1] = Qk_readWriteColormap;
	cnameQ[1] = Qk_ReadWriteColormap;
	value.addr = NULL;
	if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
	  cmaptype = EZ_READWRITE_COLORMAP;
      }

    /* =*= backingStore */
    inameQ[1] = Qk_noBackingStore;
    cnameQ[1] = Qk_NoBackingStore;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      backing_store = 0;
    /*=======================================================*/
    EZ_InitX11(dpy_name,bgColor,fgColor,geometry,
               suggested_v,cmaptype,backing_store, initgl);
    /*=======================================================*/
  }
  return(0);
}
/**********************************************************************/    
/*+++++++++++ Some global state variables +++++++++++++++*/
void EZ_SetDefaultAttributesFromResources()
{
  XrmQuark  inameQ[3], cnameQ[3], Q_type[3];/* not used */
  XrmValue value;

  inameQ[0] = EZ_InstanceQName;
  cnameQ[0] = EZ_ClassQName;
  inameQ[2] = cnameQ[2] = NULLQUARK;

  /* =*= highlight */
  inameQ[1] = Qk_highlight;
  cnameQ[1] = Qk_Highlight;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	{
	  if(v) EZ_EnableHighlight();
	  else  EZ_DisableHighlight();
	}
    }

  /* =*= sliderDepression */
  inameQ[1] = Qk_sliderDepression;
  cnameQ[1] = Qk_SliderDepression;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	{
	  if(v) EZ_EnableSliderDepression();
	  else EZ_DisableSliderDepression();
	}
    }
  /* =*= labelOffset */
  inameQ[1] = Qk_labelOffset;
  cnameQ[1] = Qk_LabelOffset;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	{
	  if(v) EZ_EnableLabelOffset();
	  else EZ_DisableLabelOffset();
	}
    }

  /* =*= bubbleHelp */
  inameQ[1] = Qk_bubbleHelp;
  cnameQ[1] = Qk_BubbleHelp;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	{
	  if(v)EZ_EnableBubbleHelp();
	  else EZ_DisableBubbleHelp();
	}
    }

  /* =*= ditherImage */
  inameQ[1] = Qk_ditherImage;
  cnameQ[1] = Qk_DitherImage;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	{
	  if(v) EZ_EnableImageDithering();
	  else EZ_DisableImageDithering();
	}
    }
  /* =*= initialState */
  inameQ[1] = Qk_initialState;
  cnameQ[1] = Qk_InitialState;
  value.addr = NULL;
  if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
    {
      int v;
      if(EZ_RetrieveSymbolicInt(value.addr, &v) >= 0)
	EZ_StartAsIcon((v == IconicState)); 
    }
  EZ_SetDefaultFontsFromResources();
}
/**********************************************************************/
void  EZ_SetDefaultFontsFromResources()
{
    XrmQuark  inameQ[3], cnameQ[3], Q_type[3];/* not used */
    XrmValue value;

    inameQ[0] = EZ_InstanceQName;
    cnameQ[0] = EZ_ClassQName;
    inameQ[2] = cnameQ[2] = NULLQUARK;

    /* =*= labelFont */
    inameQ[1] = Qk_labelFont;
    cnameQ[1] = Qk_LabelFont;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      EZ_SetDefaultLabelFont(value.addr);

    /* =*= textFont */
    inameQ[1] = Qk_textFont;
    cnameQ[1] = Qk_TextFont;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      EZ_SetDefaultEntryTextFont(value.addr);

    /* =*= menuTitleFont */
    inameQ[1] = Qk_menuTitleFont;
    cnameQ[1] = Qk_MenuTitleFont;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      EZ_SetDefaultMenuTitleFont(value.addr);

    /* =*= traversal highlight bd color */
    inameQ[1] = Qk_focusHighlightColor;
    cnameQ[1] = Qk_FocusHighlightColor;
    value.addr = NULL;
    if(XrmQGetResource(EZ_Xrm_db, inameQ, cnameQ, Q_type, &value) == True)
      EZ_SetKbdHighlightColor(value.addr);
}
/**********************************************************************/
void EZ_DestroyXrmDB()
{
  XrmDestroyDatabase(EZ_Xrm_db);
  EZ_Xrm_db = (XrmDatabase)NULL;
  EZ_InstanceQName = NULLQUARK;
  EZ_ClassQName = NULLQUARK;
  (void) my_free((char *)EZ_QConstants); EZ_QConstants = NULL;
  EZ_NumQConstants = 0;
}
/**********************************************************************/
static int cmpQuarks MY_ANSIARGS((void *na, void *nb));
static int cmpQuarks(na, nb)  void *na; void *nb;
{
  EZ_SymbQConst *a = (EZ_SymbQConst *)na;
  EZ_SymbQConst *b = (EZ_SymbQConst *)nb;
  return(a->quark < b->quark? -1 : 1); 
}

static void defineQuarkSymbolicConstants()
{
  int i;
  EZ_NumQConstants = EZ_SYM_Q_TAB_INIT_SIZE;
  
  EZ_QConstants = (EZ_SymbQConst *)
    my_malloc(EZ_SYM_Q_TAB_INIT_SIZE * sizeof(EZ_SymbQConst), _SYMB_CONST_TB_);
  for(i = 0; i < EZ_NumQConstants; i++)
    EZ_QConstants[i].quark = NULLQUARK;
#define EZ_ConstIdQkPair(_i_,_str_,_v_) \
  {\
     EZ_QConstants[_i_].quark = EZ_StringToQuark(_str_); \
     EZ_QConstants[_i_].value.type = EZ_INT;\
     EZ_QConstants[_i_].value.value.i = _v_;\
     i = _i_;\
  }
#include "EZ_CQuarks.h"
#undef   EZ_ConstIdQkPair
  EZ_NumQCInUse = (i+1);
  qsort(EZ_QConstants, EZ_NumQCInUse, sizeof(EZ_SymbQConst), 
        (int (*) MY_ANSIARGS((CONST void *,CONST void *))) cmpQuarks);
}
/**********************************************************************/

int EZ_InstallSymbolicInt(name, v) char *name; int v;
{
  EZ_Value value;
  value.type = EZ_INT; value.value.i = v;
  return(EZ_InstallSymbol(name, value));
}
     
int EZ_RetrieveSymbolicInt(name, v) char *name; int *v;
{
  EZ_Value *value = EZ_RetrieveSymbol(name);
  if(value && value->type == EZ_INT)
    { *v = value->value.i; return(0);}
  else if(value && value->type == EZ_FLOAT)
    { *v = (int) value->value.f; return(0);}
  return(-1);
}

int EZ_InstallSymbolicFloat(name, v) char *name; float v;
{
  EZ_Value value;
  value.type = EZ_FLOAT; value.value.f = v;
  return(EZ_InstallSymbol(name, value));
}
     
int EZ_RetrieveSymbolicFloat(name, v) char *name; float *v;
{
  EZ_Value *value = EZ_RetrieveSymbol(name);
  if(value && value->type == EZ_FLOAT)
    { *v = value->value.f; return(0);}
  else if(value && value->type == EZ_INT)
    { *v = (float)value->value.i; return(0);}  
  return(-1);
}

int EZ_InstallSymbolicPointer(name, v) char *name; void *v;
{
  EZ_Value value;
  value.type = EZ_POINTER; value.value.p = v;
  return(EZ_InstallSymbol(name, value));
}
     
int EZ_RetrieveSymbolicPointer(name, v) char *name; void **v;
{
  EZ_Value *value = EZ_RetrieveSymbol(name);
  if(value && value->type == EZ_POINTER)
    { *v = value->value.p; return(0);}
  return(-1);
}

int EZ_InstallSymbolicULong(name, v) char *name; unsigned long v;
{
  EZ_Value value;
  value.type = EZ_ULONG; value.value.l = v;
  return(EZ_InstallSymbol(name, value));
}
     
int EZ_RetrieveSymbolicULong(name, v) char *name; unsigned long*v;
{
  EZ_Value *value = EZ_RetrieveSymbol(name);
  if(value && value->type == EZ_ULONG)
    { *v = value->value.l; return(0);}
  else if(value && value->type == EZ_INT)
    { *v =(unsigned long) value->value.i; return(0);}  
  else if(value && value->type == EZ_FLOAT)
    { *v =(unsigned long) value->value.f; return(0);}  
  else if(value && value->type == EZ_POINTER)
    { *v =(unsigned long) value->value.p; return(0);}  
  return(-1);
}
/***************************************************************/
static int  FindSymbolicConstant(quark) XrmQuark quark;
{
  int i, b1, b2;
  if(quark == NULLQUARK) return(-1);

  b1 = 0; b2 = EZ_NumQCInUse-1;
  while( b1 <= b2)
    {
      i = (b1+b2)>>1;
      if(quark < EZ_QConstants[i].quark) b2 = i-1;
      else if(quark >  EZ_QConstants[i].quark) b1 = i+1;
      else return(i);
    }
  return(-1);
}

int EZ_InstallSymbol(name, value)
     char *name;  EZ_Value value;
{
  int  exist;
  char *ptr = name;
  XrmQuark quark;

  if(name == NULL || *name == 0) return(-1);
  if(*name >= '0' && *name <= '9')
    {
      fprintf(stderr, "'%s' cannot be used as a symbolic constant.\n", name);
      return(-1);
    }
  while(*ptr)
    {
      int c = *ptr;
      if(c == '.' || c == '*' || c == ' ' || c == '\n' || c =='\t'
	 || c == '?' || c == ':')
	{
	  fprintf(stderr, "'%s' is not an identifier.\n", name);
	  return(-1);
	}
      ptr++;
    }
  quark = EZ_StringToQuark(name);
  
  exist = FindSymbolicConstant(quark);
  if(exist >= 0)
    {
      if(EZ_ValuesEQ(EZ_QConstants[exist].value, value)) return(exist);
      else 
        {
          fprintf(stderr, "Symbol '%s' is already in use.\n",name);
          return(-1);
        }
    }
  else
    {
      int i =  EZ_NumQCInUse;
      if(i ==  EZ_NumQConstants)
        {
          int n =  i+i;
          EZ_QConstants = my_realloc(EZ_QConstants, n * sizeof(EZ_SymbQConst), _SYMB_CONST_TB_);
          while(i < n) {EZ_QConstants[i].quark = NULLQUARK; i++;}
          EZ_NumQConstants = n;
        }
      /* insert */
      i = EZ_NumQCInUse - 1;
      while(i >= 0)
        {
          if(EZ_QConstants[i].quark < quark) break;
          else memcpy((void *)&(EZ_QConstants[i]), (void *)&(EZ_QConstants[i+1]),sizeof(EZ_SymbQConst));
          i--;
        }
      i++;
      EZ_QConstants[i].quark = quark;
      memcpy((void *)&(EZ_QConstants[i].value), (void *)&(value), sizeof(EZ_Value));
      EZ_NumQCInUse++;
      return(i);
    }
}

EZ_Value *EZ_RetrieveSymbol(name)
     char *name; 
{
  XrmQuark quark;
  int i;
  
  if(name == NULL || *name == 0) return(NULL);
  quark = EZ_StringToQuark(name);

  i = FindSymbolicConstant(quark);
  if(i >= 0)  return(&(EZ_QConstants[i].value));
  return(NULL);
}


int  EZ_ValuesEQ(a,b)
     EZ_Value a,b;
{
  if(a.type != b.type) return(0);
  switch(a.type)
    {
    case EZ_INT:
      if(a.value.i == b.value.i) return(1);
      break;
    case EZ_FLOAT:
      if(a.value.f == b.value.f) return(1);
      break;
    case EZ_POINTER:
      if(a.value.p == b.value.p) return(1);
      break;
    case EZ_ULONG:
      if(a.value.l == b.value.l) return(1);
      break;
    default:
      break;
    }
  return(0);
}
/**********************************************************************/
void  EZ_SetWidgetCINames(widget, cn, in)
     EZ_Widget *widget; char *cn, *in;
{
  if(widget)
    {
      if(cn && *cn) EZ_WidgetCName(widget) = EZ_StringToQuark(cn);  
      if(in && *in) EZ_WidgetIName(widget) = EZ_StringToQuark(in);  
    }
}
void  EZ_GetWidgetCINames(widget, cn, in)
     EZ_Widget *widget; char **cn, **in;
{
  if(widget)
    {
      if(cn) *cn = EZ_QuarkToString(EZ_WidgetCName(widget));
      if(in) *in = EZ_QuarkToString(EZ_WidgetIName(widget));
    }
}
/*******************************************************************/

int EZ_GetApplicationResource( rname, v_ret)
     char *rname; char **v_ret;
{
  XrmQuark  clist[4], nlist[4], Q_type[2], quark;
  XrmValue  xrmvalue;
  int ret = 0;
  xrmvalue.addr = NULL;
  if(rname && *rname)  
    {
      quark = EZ_StringToQuark(rname);

      clist[2] = NULLQUARK;
      nlist[2] = NULLQUARK;
      clist[1] = quark;
      nlist[1] = quark; 
      clist[0] = EZ_ClassQName;
      nlist[0] = EZ_InstanceQName;
      ret = XrmQGetResource(EZ_Xrm_db, nlist, clist, Q_type, &xrmvalue);
    }
  if(v_ret) *v_ret = xrmvalue.addr;
  return(ret);
}
/*******************************************************************/
int EZ_GetWidgetResource(widget, rname, v_ret)
     EZ_Widget *widget; char *rname, **v_ret;
{
  int ret = 0;
  XrmValue xrmvalue;

  xrmvalue.addr = NULL;
  if(widget && rname && *rname)
    {
      EZ_Widget *parent;
      XrmQuark  clist[64], nlist[64], Q_type[2], quark;
      int i;
      quark = EZ_StringToQuark(rname);

      clist[63] = NULLQUARK;
      nlist[63] = NULLQUARK;
      clist[62] = quark;
      nlist[62] = quark; 
      i = 61;
      parent = widget;
      while(parent)
	{
	  clist[i] = EZ_WidgetCName(parent);
	  nlist[i] = EZ_WidgetIName(parent);	  
	  parent = EZ_WidgetParent(parent);
	  i--;
	}
      clist[i] = EZ_ClassQName;
      nlist[i] = EZ_InstanceQName;
      ret = XrmQGetResource(EZ_Xrm_db, (nlist+i), (clist+i), Q_type, &xrmvalue);
    }
  if(v_ret) *v_ret = xrmvalue.addr;
  return(ret);
}
/**********************************************************************/
int EZ_GetItemResource(item, rname, v_ret)
     EZ_Item *item; char *rname, **v_ret;
{
  int ret = 0;
  XrmValue xrmvalue;
  xrmvalue.addr = NULL;

  if(item && rname && *rname)
    {
      XrmQuark  clist[10], nlist[10], Q_type[2], quark;
      quark = EZ_StringToQuark(rname);

      clist[0] = EZ_ClassQName;
      nlist[0] = EZ_InstanceQName;
      clist[1] = EZ_ItemCName(item);
      nlist[1] = EZ_ItemIName(item);	  
      clist[2] = quark;
      nlist[2] = quark; 
      clist[3] = NULLQUARK;
      nlist[3] = NULLQUARK;
      ret = XrmQGetResource(EZ_Xrm_db, nlist, clist, Q_type, &xrmvalue);
    }
  if(v_ret) *v_ret = xrmvalue.addr;
  return(ret);
}
/**********************************************************************/
XrmDatabase *EZ_GetResourceDatabase()
{  return(&EZ_Xrm_db); }
/**********************************************************************/
int  EZ_GetWidgetType(widget) EZ_Widget *widget;
{if(widget) return(EZ_WidgetType(widget)); return(0);}
/**********************************************************************/
int   EZ_GetOptionCINames(i, cn, in)
     int i; char **cn; char **in;
{
  if(i > 0 && i <= EZ_WIDGET_CONFIGURE_LAST_OPTION)
    {
      if(cn) *cn = EZ_QuarkToString(EZ_OptionsCQs[i]);
      if(in) *in = EZ_QuarkToString(EZ_OptionsNQs[i]);
      return(i);
    }
  return(0);
}
/**********************************************************************/
#undef   _EZ_RESOURCE_C_
