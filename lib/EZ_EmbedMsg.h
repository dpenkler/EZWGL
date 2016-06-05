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
/********************************************************
 *
 *  message types used for communication
 *
 *  suffix:   _S   message to the embeder widget.
 *            _C   message to the embeded widget.
 */

/*
 * Embeding depth. Embeding is limited by this number.
 */
#define EZ_EMBEDING_DEPTH  2

/*
 * When a client wants to be embeded. It broadcasts a message
 * of type EZ_EMBEING_REQUEST, and wait for a corresponding reply message
 * from another client, of type EZ_EMBEDING_REPLY. It then takes the first 
 * reply and embeded itself into the window supplied. And sends an
 * EZ_EMBEDING_CONFIRM back to the responsible application.
 */
#define  EZ_EMBEDING_REQUEST_S                      1
#define  EZ_EMBEDING_REPLY_C                        2   
#define  EZ_EMBEDING_CONFIRM_S                      3


/*
 * When an embeded widget is unmapped (HideWidget or NonActive)
 * it sends a message of type EZ_EMBEDING_UNMAPPED_S to its
 * embeder. When it is re-mapped, it sends a message of type
 * EZ_EMBEDING_REMAPPED_S to its embeder.
 */
#define  EZ_EMBEDING_UNMAPPED_S                     4
#define  EZ_EMBEDING_REMAPPED_S                     5
/*
 * When an embeded widget exists, it sends a message of type
 * EZ_EMBEDING_CLIENT_DESTROIED to its embeder application.
 */
#define  EZ_EMBEDING_CLIENT_DIED_S                  6

/* 
 * when a window containing an application is going to be destroyed,
 * it sends either a message of type DESTROY_YOUSELF or a message
 * of type MANAGE_YOUSELF to the client.
 */
#define EZ_EMBEDING_DESTROY_YOUSELF_C               7
#define EZ_EMBEDING_MANAGE_YOUSELF_C                8
#define EZ_EMBEDING_SELF_MANAGED_S                  9

/*
 * When the embeded application window is resized, it sends a relevent
 * message to its embeder.
 */
#define EZ_EMBEDING_WINDOW_RESIZED_S                10
#define EZ_EMBEDING_MOVE_WINDOW_C                   11
#define EZ_EMBEDING_RESIZE_WINDOW_C                 12
#define EZ_EMBEDING_MOVE_RESIZE_WINDOW_C            13

/***********************************************************************
 ***                                                                 ***
 ***            Embeding Message                                     ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct _EZ_EmbedingMessage {
  int                 type;        /* message type  */

  unsigned long       length;      /* length of this message */
  unsigned long       id;          /* message id */

  unsigned long       toCommWin;   /* receiver info */
  unsigned long       toWin;
  unsigned long       toWidget;

  unsigned long       fmCommWin;   /* sender info  */
  unsigned long       fmWin;
  unsigned long       fmWidget;

  int                 x,y,w,h;     /* geometry of sender */
  int                 a1,a2;       /* extra info */
} EZ_EmbedingMessage;

/**********************************************************************/
