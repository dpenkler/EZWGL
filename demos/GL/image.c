/* test EZ_GetGLImage***  */
#include "EZ.h"

static void eventHandler(EZ_Widget *, void *, int, XEvent *);
static void draw(EZ_Widget *);

static EZ_GLImage *testImage, *bkImage, *saveImage = NULL;
int                imageW, imageH, bkImgW, bkImgH;
int                saveX, saveY;

main(int argc, char **argv)
{
  EZ_Widget *frame, *canvas;

  EZ_Initialize(argc, argv, 1);
  /*
   * canvases do not resize propertly as toplevel
   * widgets (bug!) It is recommended to put it
   * under a frame at least for now.
   */
  frame = EZ_CreateWidget(EZ_WIDGET_FRAME, NULL, 
                         EZ_FILL_MODE, EZ_FILL_BOTH,
                         0);
  canvas = EZ_CreateWidget(EZ_WIDGET_3D_CANVAS, frame,
                           EZ_WIDTH_HINT,  640, 
                           EZ_HEIGHT_HINT, 420,
			   EZ_BORDER_WIDTH, 0,
			   EZ_BORDER_TYPE, EZ_BORDER_SUNKEN,
                           EZ_EVENT_HANDLE, eventHandler, NULL,
                           0);
  /*
   * now display the canvas. One must first display
   * a 3DCanvas before calling any GL functions !!!
   */
  EZ_DisplayWidget(canvas);         

  /* the little glimage */
  {
    EZ_Pixmap  *testPixmap;
    testPixmap = EZ_CreateLabelPixmapFromXpmFile("flowers.xpm");
    testImage = EZ_CreateGLImage(testPixmap);
    EZ_FreeLabelPixmap(testPixmap);
    EZ_GetGLImageInfo(testImage, NULL, NULL, &imageW, &imageH);
  }

  /* the background pixmap */
  {
    EZ_Pixmap  *testPixmap;
    testPixmap = EZ_CreateLabelPixmapFromImageFile("guilin.gif");
    bkImage = EZ_CreateGLImage(testPixmap);
    EZ_FreeLabelPixmap(testPixmap);
    EZ_GetGLImageInfo(bkImage, NULL, NULL, &bkImgW, &bkImgH);
  }

  
  /* now setup global GL attributes */
  EZ_RGBMode();                 /* we are using RGB mode  */
  EZ_ShadeModel(EZ_SMOOTH);     /* turn on smooth shading */
  EZ_AutoSelectBackBuffer();    /* select a back buffer   */
  EZ_DrawBuffer(EZ_BACK); /* draw the background */
  /*
   * We'll draw a 2D polygon, so we don't have to setup
   * a complex projection matrix. The default matrix mode
   * is EZ_MODELVIEW. We just set the projection matrix
   * on this default matrix stack.
   */
  EZ_LoadIdentity();            /* clear the top mtx 2 id */     
  EZ_Ortho2(-1.0,1.0,-1.0,1.0); /* project the unit cube  */
  EZ_EventMainLoop();           /* handle events          */
}

/* 
 * the event handler. It should handle at least the
 * EZ_REDRAW event and the EZ_RESIZE.
 */
static void eventHandler(EZ_Widget *canvas, void *data,
                         int eventType, XEvent *xevent)
{
  switch(eventType) 
    {
    case EZ_REDRAW:
    case EZ_RESIZE:
      /* resize redraw-> repaint the whole thing */
      EZ_Clear(EZ_COLOR_BUFFER_BIT); /* clear the frame buffer */  
      EZ_PutGLImageXWindow(bkImage, 0, 0, 640, 420, 0, 0);
      EZ_SwapBuffers();
      break;
    case EZ_KEY_PRESS:
      if(EZ_PressedKey == EZ_ESCAPE_KEY) exit(0);
      break;
    case EZ_BUTTON1_PRESS:
      {
	XEvent xevent;
	/* repare damaged background */
	if(saveImage)
	  {
	    EZ_PutGLImageXWindow(saveImage, 0,0, 64, 64, saveX, saveY);
	    EZ_FreeGLImage(saveImage); /* this is important or your program will grow! */
	    saveImage = NULL;
	  }
	/* save the part of the background to be overwritten */
	saveX = EZ_PointerCoordinates[0];
	saveY = EZ_PointerCoordinates[1];
	EZ_GetGLImageXWindow(saveX, saveY, 64, 64, &saveImage);
	EZ_PutGLImageXWindow(saveImage, 0, 0, 64,64,0, 0);

	EZ_PutGLImageXWindow(testImage, 0, 0, 64,64,saveX, saveY);
	EZ_SwapBuffers();
	while(1)
	  {
	    int device = EZ_GLNextEvent(canvas, &xevent);	    
	    if(device == EZ_POINTER_MOTION)
	      {
		if(saveImage)  /* repair the background */
		  {
		    EZ_PutGLImageXWindow(saveImage, 0, 0, 64, 64, saveX, saveY);
		    EZ_FreeGLImage(saveImage);
		  }
		/* save */
		saveX = EZ_PointerCoordinates[0];
		saveY = EZ_PointerCoordinates[1];

		EZ_GetGLImageXWindow(saveX, saveY, 64,64, &saveImage);
		
		/* this is what is in saveImage, displayed at the UL corner of the canvas */
		EZ_PutGLImageXWindow(saveImage, 0, 0, 64, 64, 0,0);
		/* overwritten */
		EZ_PutGLImageXWindow(testImage, 0, 0, 64,64,saveX,saveY);

		EZ_SwapBuffers();
	      }
	    else if(device == EZ_BUTTON1_RELEASE)
	      {
		if(saveImage) /* if needed */
		  {
		    EZ_PutGLImageXWindow(saveImage, 0, 0, 64, 64, saveX, saveY);
		    EZ_FreeGLImage(saveImage);
		    saveImage = NULL;
		  }
		break;
	      }
	  }
      }
    break;
    default:
      break;
    }
}
/*********************************************************/  
