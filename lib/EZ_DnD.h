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

/***********************************************************************/
#define EZ_DND_MAX_TARGETS       32
#define EZ_DND_MSG_HEADER_SIZE   400

/***********************************************************************
 * 
 *   Drag and Drop handle
 *
 */
/* DnD data conversion */
typedef struct EZ_DnDDataEncoder_ {
  Atom              type;                 /* the Target data type this handler handles */
  void              *object;              /* object this encoder is binded to */
  void              *data;
  unsigned int      tag;                  /* action type */
  EZ_DnDEncoder     encoder;              /* how to encode the data. For dragSrc only  */
  EZ_CallBackStruct callback;             /* action after message has been successfully sent */
  struct EZ_DnDDataEncoder_ *next;        /* the link */
} EZ_DnDDataEncoder;

typedef struct EZ_DnDDataDecoder_ {
  Atom              type;                 /* the Target data type this handler handles */
  void              *object;              /* object this decoder is binded to */
  void              *data;
  unsigned int      tag;                  /* action type */
  EZ_DnDDecoder     decoder;              /* how to decode the data. For dropTarget only */
  EZ_CallBackStruct callback;             /* action after successfully decode the data */
  struct EZ_DnDDataDecoder_ *next;
} EZ_DnDDataDecoder;

/***********************************************************************
 *
 * There is one copy of the following structure for each EZWGL client,
 * At any given time. 
 */

typedef struct EZ_DnDInfo_ {
  /* for each DnD transaction, there is a unique id attached to all messages.
   * this id is set by dragSrc when a drag is started, and copied to dropTargets
   * through the first broadcasted message, of type DRAG_STARTED */
  unsigned long      id;
  unsigned int       actionType;  /* action, set by src. Ored from ModifierKeyMasks */
  
  /*
   *============ Info about the DropSrc.=======================
   */
  
  /* for dragSrc, the next block is set by InitDrag; for dropTarget, it
   * is set after receiving a DRAG_STARTED message. */
  int                ImSrc;  
  int                srcType;               /* item or widget */
  unsigned long      srcCommWin; 
  unsigned long      srcWin;
  unsigned long      srcItem;
  unsigned long      srcApplicationId;
  int                srcX, srcY;
  int                srcW, srcH;
  int                srcX1, srcY1;

  /* the list of targets the current drag src converts, available whenever
   * there is a drag is in action */
  unsigned int       srcNTargets;
  Atom               srcTargets[EZ_DND_MAX_TARGETS]; 

  int                srcStatus;
  /* this field is used by dragSrc only, and it is
   * set before answering a DATA_CONVERSION request */
  EZ_DnDDataEncoder  *srcEncoder;
  Cursor              cursor;        /* drag icon, target won't know it !! */
  /*=====================================================
   * current pointer location, valid during the
   * draging process 
   *====================================================*/
  int                dragIconType;  /* either a pixmap or an outline or a cursor */
  int                px,py;    
  unsigned long      ptWin;

  /*
   * ============== Info about the DropSite. Note: src may= target =============
   */
  /*
   *  the <targetCommWin> and the <ImTarget> is initialized upon receiving
   *  a DRAG_STARTED message from the src,
   * 
   *  the <targetType>, <targetWin>  <targetItem> and the XYWH's are 
   *  initialized when a message DRAG_DROPED is received. Note: this message
   *  is broadcasted by the dragSrc, but with a designated receiver. Only the
   *  designated receving client initialize the other fields.
   */
  int                ImTarget;         
  int                targetType;       
  unsigned long      targetCommWin;    
  unsigned long      targetWin;        
  unsigned long      targetItem;
  unsigned long      targetApplicationId; 
  Atom               thisAtom;
  int                targetX,targetY;  
  int                targetW,targetH;
  int                targetX1, targetY1;

  int                targetStatus;

  unsigned int       targetNTargets;
  Atom               targetTargets[EZ_DND_MAX_TARGETS];   
  EZ_DnDDataDecoder  *targetDecoder;  /* current decoder */

  /* for help on drop sites */
  int                 timerEnabled;
  int                 siteHelpStatus;
  int                 siteType;         /* widget or item */
  void                *site;
  char                *siteHelpString;
  EZ_Timer            *timer;

  /* message header. used as a tmp */
  char                theHeader[EZ_DND_MSG_HEADER_SIZE];
  /* a short message send by the drag src along with the */
  /* drag is droped message. */
  char                *dropMessage;
  int                 dropMessageLength;

  EZ_Widget           *dragIcon;  /* drag icon */
} EZ_DnDInfoStruct;

/***********************************************************************
 *  This structure is used in decode DnDMessageHeader.
 */
typedef struct _EZ_DnDMessage_ {
  int                 type;        /* message type  */
  unsigned long       id;          /* id of this message */
  unsigned long       length;      /* length of this message */

  unsigned long       toType;      /* receiver type, src or target */
  unsigned long       toCommWin;   /* receiver info */
  unsigned long       toWin;
  unsigned long       toItem;
  unsigned long       toId;

  unsigned long       fmCommWin;   /* sender info  */
  unsigned long       fmWin;
  unsigned long       fmItem;
  unsigned long       fmId;

  unsigned long       thisAtom;

  unsigned long       ptWin;
  int                 px, py;      /* pointer position */
  int                 iconType;    /* drag icon type   */

  int                 x,y,w,h,x1,y1;  /* extra info */
  
  char                *message;    /* the message */
  
} EZ_DnDMessage;

/***********************************************************************/
#define EZ_DND_DRAG_ICON_OUTLINE               0
#define EZ_DND_DRAG_ICON_PIXMAP                1
#define EZ_DND_DRAG_ICON_CURSOR                2
#define EZ_DND_DRAG_ICON_ICON                  3
#define EZ_DND_DRAG_ICON_SPECIAL               4
/***********************************************************************/
#define EZ_DND_OBJ_IS_UNKNOWN                  0
#define EZ_DND_OBJ_IS_ITEM                     1
#define EZ_DND_OBJ_IS_WIDGET                   2
/***********************************************************************/
#define EZ_DRAG_FINISHED                       0
#define EZ_DRAG_STARTED                        1
#define EZ_DRAG_RELEASED                       2
#define EZ_DRAG_CONVERTING                     3
#define EZ_DRAG_SENDING_DATA                   4
#define EZ_DRAG_WAITING_FOR_ACK                5
#define EZ_DRAG_EXECUTING_CALLBACK             6
#define EZ_DRAG_ABORTING                       7

#define EZ_DROP_FINISHED                       0
#define EZ_DROP_WAITING_FOR_DATA               1
#define EZ_DROP_TRY_NEXT_TARGET                2
#define EZ_DROP_CONVERTING                     3
#define EZ_DROP_EXECUTING_CALLBACK             4
#define EZ_DROP_ABORTING                       5

/***********************************************************************/
#define EZ_DND_FAILURE                         0
#define EZ_DND_SUCCESS                         1
/***********************************************************************/
#define EZ_DND_SENDER_IS_SRC                   1
#define EZ_DND_SENDER_IS_TARGET                2
#define EZ_DND_RECEIVER_IS_SRC                 1
#define EZ_DND_RECEIVER_IS_TARGET              2

/***********************************************************************
 *
 * Internal DND message types.
 *
 ***********************************************************************/


/***********************************************************************
 * When an item is picked up by the left button, it broadcast a message of type
 *  ... to all EZWGL applications.
 */
#define    EZ_DND_DRAG_START         1 

/*
 * when an item is draged, (position of SnapShot changed),
 * it broadcast a message of the type ... to all applications
 */
#define    EZ_DND_DRAG_MOTION        2

/*
 * when  a drag is released (button 1 release), it broadcast a message
 * of type ... to all EZWGL applications. The message includes 
 * the list of data types the src can convert. Only the one of these 
 * applications is the drop site, which should respond.
 */
#define    EZ_DND_DRAG_DROPPED       3

/*
 * When a drop site receives an EZ_DND_DRAG_DROPPED message,
 * it examines the target list to see if the src is capable of
 * converting a data type it wants. If yes, it sends a message
 * of type EZ_DND_REQUEST_CONVERSION to the drag src and 
 * waits for further correspondence. If not, it sends a
 * message of type EZ_DND_ABORT to the drag src.
 */
#define    EZ_DND_REQUEST_CONVERSION  4
#define    EZ_DND_CONVERSION_FAILED   5
#define    EZ_DND_CONVERSION_REFUSED  6

/*
 * When the src finishes converting, it sends to the target
 * a message of type EZ_DND_DATA. The actual data type is
 * encoded in the message. It's drop target's responsibility
 * to decode the actual data.
 */
#define    EZ_DND_DATA                7

/* 
 * When a drop is completed, the drop target send a message
 * of the following type to its drag src to acknowlege the
 * completion of the current drop.
 */
#define    EZ_DND_DROP_DONE           8

/*
 *  When a drop is in progess and another drag is requested
 *  by user, send a message of the following type to the
 *  current dropsite to request unlock. The current drop target
 *  is responsible to answer the request.
 */
#define    EZ_DND_SRC_REQUEST_UNLOCK      9
#define    EZ_DND_TARGET_REQUEST_UNLOCK   10

/* 
 * If something went wrong with the current transaction,
 * send to the party the following message.
 */

#define    EZ_DND_ABORT_TO_SRC            12
#define    EZ_DND_ABORT_TO_TARGET         13

/* 
 *  If user press the F1 key when a drag is over a drop site, the
 *  src sends a message of the next type to the party to request
 *  help.
 */
#define    EZ_DND_SRC_REQUEST_HELP        14
#define    EZ_DND_SRC_CANCEL_HELP         15

#define    EZ_DND_DRAG_CANCELLED          16
#define    EZ_DND_DRAG_LEFT               17

/*******************************************************************/
