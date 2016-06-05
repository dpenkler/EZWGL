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
/*****************************************************************/

/* the initial table size is 68 */
#ifndef EZ_SYM_Q_TAB_INIT_SIZE
#define EZ_SYM_Q_TAB_INIT_SIZE 100
/* application installed symbols starts here */
#define EZ_SYM_Q_TAB_START_IDX 69
#endif

/* make sure all names are distinct !! */
#ifdef EZ_ConstIdQkPair

EZ_ConstIdQkPair(0, "no",                  0)
EZ_ConstIdQkPair(1, "yes",                 1)
EZ_ConstIdQkPair(2, "No",                  0)
EZ_ConstIdQkPair(3, "Yes",                 1)
EZ_ConstIdQkPair(4, "null",                0)
EZ_ConstIdQkPair(5, "Null",                0)
EZ_ConstIdQkPair(6, "false",               0)
EZ_ConstIdQkPair(7, "true",                1)
EZ_ConstIdQkPair(8, "False",               0)
EZ_ConstIdQkPair(9, "True",                1)
EZ_ConstIdQkPair(10, "on",                 1)
EZ_ConstIdQkPair(11, "off",                0)
EZ_ConstIdQkPair(12, "On",                 1)
EZ_ConstIdQkPair(13, "Off",                0)
EZ_ConstIdQkPair(14, "none",               0)
EZ_ConstIdQkPair(15, "None",               0)

EZ_ConstIdQkPair(16, "nil",                0)

EZ_ConstIdQkPair(17, "horizontal",          1)
EZ_ConstIdQkPair(18, "horizontalLeft",      2)
EZ_ConstIdQkPair(19, "horizontalRight",     3)
EZ_ConstIdQkPair(20, "horizontalCenter",    4)
EZ_ConstIdQkPair(21, "vertical",            5)
EZ_ConstIdQkPair(22, "verticalTop",         6)
EZ_ConstIdQkPair(23, "verticalBottom",      7)
EZ_ConstIdQkPair(24, "verticalCenter",      8)

EZ_ConstIdQkPair(25, "fillNone",            0)
EZ_ConstIdQkPair(26, "fillHorizontally",    1)
EZ_ConstIdQkPair(27, "fillVertically",      2)
EZ_ConstIdQkPair(28, "fillBoth",            3)

EZ_ConstIdQkPair(29, "center",              0)
EZ_ConstIdQkPair(30, "left",                1)
EZ_ConstIdQkPair(31, "right",               2)
EZ_ConstIdQkPair(32, "top",                 3)
EZ_ConstIdQkPair(33, "bottom",              4)
EZ_ConstIdQkPair(34, "topLeft",             5)
EZ_ConstIdQkPair(35, "topRight",            6)
EZ_ConstIdQkPair(36, "bottomLeft",          7)
EZ_ConstIdQkPair(37, "bottomRight",         8)

EZ_ConstIdQkPair(38, "empty",               0)
EZ_ConstIdQkPair(39, "square",              1)
EZ_ConstIdQkPair(40, "sunkenSquare",        2)
EZ_ConstIdQkPair(41, "diamond",             3)
EZ_ConstIdQkPair(42, "sunkenDiamond",       4)
EZ_ConstIdQkPair(43, "circle",              5)
EZ_ConstIdQkPair(44, "check",               6)
EZ_ConstIdQkPair(45, "rectangle",           7)
EZ_ConstIdQkPair(46, "mbutton",             8)
EZ_ConstIdQkPair(47, "tick",                9)


EZ_ConstIdQkPair(48, "row",                 1)
EZ_ConstIdQkPair(49, "column",              2)
EZ_ConstIdQkPair(50, "allRows",             4)
EZ_ConstIdQkPair(51, "allColumns",          8)


EZ_ConstIdQkPair(52, "flat",                1)
EZ_ConstIdQkPair(53, "ridge",               2)
EZ_ConstIdQkPair(54, "groove",              3)
EZ_ConstIdQkPair(55, "raised",              4)
EZ_ConstIdQkPair(56, "sunken",              5)
EZ_ConstIdQkPair(57, "embossed",            8)
EZ_ConstIdQkPair(58, "engraved",            9)
EZ_ConstIdQkPair(59, "shadow",              22)
EZ_ConstIdQkPair(60, "shadow3",             23)

EZ_ConstIdQkPair(61, "normalState",         1)
EZ_ConstIdQkPair(62, "iconicState",         3)
EZ_ConstIdQkPair(63, "NormalState",         1)
EZ_ConstIdQkPair(64, "IconicState",         3)
EZ_ConstIdQkPair(65, "up",                  10)
EZ_ConstIdQkPair(66, "down",                11)
EZ_ConstIdQkPair(67, "leftRight",           12)
EZ_ConstIdQkPair(68, "upDown",              13)

/*****************************************************************/
#endif

