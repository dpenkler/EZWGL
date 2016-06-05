/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997  Maorong Zou
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
/**********************************************************************/

{
  if(item)
    {
      int option, i1, i2, i3, i4; 
      unsigned long ul1, ul2;
      void *p1, *p2;
      int flag;

      option = va_arg(ap, int);
      
      while(option != 0)
	{
	  i1 = i2 = i3 = i4 = 0; ul1 = ul2 = 0L; p1 = p2 = NULL;
	  flag = 1;
	  switch(option)
	    {
	    case EZ_ORIENTATION:
	    case EZ_JUSTIFICATION:
	    case EZ_LABEL_POSITION:
	    case EZ_ATTACH_INT_DATA:
	    case EZ_FONT_ID:
	    case EZ_IPADX:   case EZ_IPADY:
	    case EZ_PADX:    case EZ_PADY: 
	    case EZ_BORDER_TYPE: 
	    case EZ_BORDER_WIDTH: 
	    case EZ_TEXT_LINE_LENGTH:
	    case EZ_X:
	    case EZ_Y:
	    case EZ_WIDTH:
	    case EZ_HEIGHT:
	    case EZ_MARK_CLEAN:
	      i1 = va_arg(ap, int);
	      break;
	    case EZ_LOCATION:
	    case EZ_SIZE:
            case EZ_LABEL_SHADOW:
	      i1 = va_arg(ap, int);
	      i2 = va_arg(ap, int);
	      break;
            case EZ_GEOMETRY:
	      i1 = va_arg(ap, int);
	      i2 = va_arg(ap, int);
	      i3 = va_arg(ap, int);
	      i4 = va_arg(ap, int);
              break;
	    case EZ_CALLBACK:
	    case EZ_DESTROY_CALLBACK:
	      {
		EZ_CallBack callback = va_arg(ap, EZ_CallBack);
		void *data = va_arg(ap, void *);
		p1 = (void *)callback;
		p2 = data;
	      }
	    break;
	    case EZ_PIXMAP_DATA:
	      p1 = (void *) va_arg(ap, char **);
	      break;
	    case EZ_BITMAP_DATA:
	    case EZ_RAW_RGB_DATA:
	      p1 = (void *)va_arg(ap, char *);
	      i1 = va_arg(ap, int);
	      i2 = va_arg(ap, int);
	      break;
	    case EZ_DND_DRAG_CURSOR:
	      ul1 = (unsigned long) va_arg(ap,Cursor);
	      break;
	    case EZ_X_PIXMAP:
	      ul1 = (unsigned long) va_arg(ap,Pixmap);
	      i1 = va_arg(ap,int);
	      i2 = va_arg(ap,int);
	      i3 = va_arg(ap,int);
	      i4 = va_arg(ap, int);	      
	      break;

	    case EZ_PIXMAP_FILE:
	    case EZ_IMAGE_FILE:
	    case EZ_DND_BUBBLE_HELP:
	    case EZ_FONT_NAME:
	    case EZ_FOREGROUND:
	    case EZ_BACKGROUND:
	    case EZ_LABEL_PIXMAP:
	    case EZ_LABEL_STRING:
            case EZ_NAME:
            case EZ_CLASS:
            case EZ_RESOURCE_STRING:
	      p1 = (void *)va_arg(ap, char *);
	      break;
	    case EZ_ATTACH_PTR_DATA:
	    case EZ_ITEM_WIDGET_WIDGET:
	      p1 = (void *)va_arg(ap, void *);
	      break;
	    default: 
	      flag = 0;
	      break;
	    }
	  if(flag) EZ_SetItemAttributes(item, option, i1, i2, p1, p2, ul1, ul2, i3, i4);
	  option = va_arg(ap, int);	  
	}
    }
}
