/*
 * Modified to compile with EZWGL.  mzou@math.utexas.edu
 *
 * Esetroot -- Set the root pixmap.  This program enables non-Enlightenment
 *             users to use Eterm's support for pseudotransparency.
 *
 * Written by Nat Friedman <ndf@mit.edu> with modifications by Gerald Britton
 * <gbritton@mit.edu> and Michael Jennings <mej@tcserv.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "EZ.h"

static XrmOptionDescRec cmdOptions[] = {
  {"-brightness",           ".bright",         XrmoptionSepArg,  NULL  },
  {"-bright",               ".bright",         XrmoptionSepArg,  NULL  },
  {"-b",                    ".bright",         XrmoptionSepArg,  NULL  },
  {"-scale",                ".scale",          XrmoptionNoArg,   "True"},
  {"-s",                    ".scale",          XrmoptionNoArg,   "True"},
  {"-retain",               ".retain",         XrmoptionNoArg,   "True"},
  {"-h",                    ".help",           XrmoptionNoArg,   "True"},
  {"-help",                 ".help",           XrmoptionNoArg,   "True"},
};

int main(int argc, char *argv[]) 
{
  Display    *theDisplay;
  Window     theRootWindow;
  int        theScreenNum, theDepth;
  Visual     *theVisual;
  char       *fname;
  Atom       XROOTPMAP_ID;
  EZ_Pixmap  *theImage = NULL;
  int        scale = 0;
  int        retain = 0;
  int        brightness =100;
  int        sW, sH, gx,gy,gw=0,gh=0,jnk, iwidth, iheight;
  unsigned char *rgbdata = NULL;
  char       *value;

  EZ_InitializeXrm("setroot",                  /* class name            */
		   cmdOptions,                 /* cmd line options      */
		   EZ_ArraySize(cmdOptions),   /* # of cmd line options */
		   &argc,                /* arg count, in and out */
		   argv,                 /* cmd args, in and out  */
		   NULL,                 /* fallback resources    */
		   0);                   /* useless junk          */

  if(argc != 2 || EZ_GetApplicationResource("help", &value)) 
    {
      fprintf(stderr, "\nUsage: %s imagefile\n", argv[0]);
      fprintf(stderr, "\t-scale                 'scale to fit the whole screen area'\n");
      fprintf(stderr, "\t-retain                'save as perminant server resource'\n");
      fprintf(stderr, "\t-brightness percent    'darken or brighten the image'\n");
      fprintf(stderr, "\t-geometry WIDTHxHEIGHT 'specify geometry'\n");
      /*EZ_PrintStandardOptions();*/
      EZ_Shutdown();
      exit(0);
    }
  fname = argv[1];

  if(EZ_GetApplicationResource("geometry", &value)) 
    EZ_ParseGeometry(value, &jnk, &gx, &gy, &gw, &gh);
  if(EZ_GetApplicationResource("scale", &value)) 
    scale = (strcmp(value, "True") == 0);
  if(EZ_GetApplicationResource("retain", &value)) 
    retain = (strcmp(value, "True") == 0);
  if(EZ_GetApplicationResource("bright", &value)) 
    { 
      int ii= 0; if(sscanf(value, "%d", &ii) == 1)
      brightness = (ii > 0 && ii < 200)? ii : 100; 
    }

  if((XROOTPMAP_ID = EZ_GetAtom("_XROOTPMAP_ID")) == None)
    {
      fprintf(stderr, "EZ_GetAtom(\"_XROOTPMAP_ID\") failed");
      EZ_Shutdown();
      exit(1);
    }
  EZ_GetDisplayInfo( &theDisplay, &theScreenNum, &theVisual, &theDepth);
  theRootWindow = RootWindow(theDisplay, theScreenNum);
  if(gw <= 0 || gh <= 0) EZ_GetDisplaySize(&sW, &sH);
  else {sW = gw; sH = gh; scale = 1;}
  
  if(EZ_ReadImageFile(fname, &iwidth, &iheight, &rgbdata))
    {
      unsigned char *newrgb = NULL;
      if(scale || brightness != 100) 
        {
          if(scale == 0) { sW = iwidth; sH = iheight;}
          newrgb = EZ_ScaleRGB(rgbdata, iwidth, iheight, sW, sH, brightness, 1);
          if(newrgb)
            {
              (void) free(rgbdata);
              rgbdata = newrgb;
              iwidth = sW;
              iheight = sH;
            }
        }
      theImage = EZ_CreateLabelPixmapFromRawRGB(rgbdata, iwidth, iheight);
      (void)free(rgbdata);
    }
  if(theImage == NULL)
    {
      fprintf(stderr, "Couldn't create pixmap from '%s'\n", fname);
      EZ_Shutdown();
      exit(2);      
    }
  else
    {
      Pixmap         pixmap=None, shape, copy;
      int            width=0,  height=0;

      EZ_GetLabelPixmapInfo(theImage, &pixmap, &shape, &width, &height);
      if(pixmap != None)
        {
          copy = XCreatePixmap(theDisplay, pixmap, width, height, theDepth);
          XCopyArea(theDisplay,pixmap, copy, DefaultGC(theDisplay, theScreenNum),
                    0,0,width,height,0,0);
          if(retain)
            {
              XGrabServer(theDisplay);
              /* this is the only correct way to do it right */
              /* set the new background */
              XChangeProperty(theDisplay, theRootWindow, XROOTPMAP_ID, XA_PIXMAP, 32, 
                              PropModeReplace, (unsigned char *) &copy, 1);
              XSetCloseDownMode(theDisplay, RetainTemporary);
              XUngrabServer(theDisplay);
            }
          XFlush(theDisplay);
          XSetWindowBackgroundPixmap(theDisplay, theRootWindow, copy);
          XClearWindow(theDisplay, theRootWindow);
          XFlush(theDisplay);
        }
    }
  EZ_Shutdown();
  exit(0);
}
/**********************************************************************************/





