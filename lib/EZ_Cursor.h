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
 /**********************************************************************
  *
  * list of all X cursors fonts
  *
  *********************************************************************/

#ifndef EZ_C_NUM_GLYPHS
#include <X11/cursorfont.h>
#define EZ_C_NUM_GLYPHS            XC_num_glyphs
#define EZ_TOTAL_NUM_OF_CURSORS    XC_num_glyphs

#define EZ_C_X_CURSOR              XC_X_cursor
#define EZ_C_ARROW                 XC_arrow
#define EZ_C_BASED_ARROW_DOWN      XC_based_arrow_down
#define EZ_C_BASED_ARROW_UP        XC_based_arrow_up
#define EZ_C_BOAT                  XC_boat
#define EZ_C_BOGOSITY              XC_bogosity
#define EZ_C_BOTTOM_LEFT_CORNER    XC_bottom_left_corner
#define EZ_C_BOTTOM_RIGHT_CORNER   XC_bottom_right_corner
#define EZ_C_BOTTOM_SIDE           XC_bottom_side
#define EZ_C_BOTTOM_TEE            XC_bottom_tee
#define EZ_C_BOX_SPIRAL            XC_box_spiral
#define EZ_C_CENTER_PTR            XC_center_ptr
#define EZ_C_CIRCLE                XC_circle
#define EZ_C_CLOCK                 XC_clock
#define EZ_C_COFFEE_MUG            XC_coffee_mug
#define EZ_C_CROSS                 XC_cross
#define EZ_C_CROSS_REVERSE         XC_cross_reverse
#define EZ_C_CROSSHAIR             XC_crosshair
#define EZ_C_DIAMOND_CROSS         XC_diamond_cross
#define EZ_C_DOT                   XC_dot
#define EZ_C_DOTBOX                XC_dotbox
#define EZ_C_DOUBLE_ARROW          XC_double_arrow
#define EZ_C_DRAFT_LARGE           XC_draft_large
#define EZ_C_DRAFT_SMALL           XC_draft_small
#define EZ_C_DRAPED_BOX            XC_draped_box
#define EZ_C_EXCHANGE              XC_exchange
#define EZ_C_FLEUR                 XC_fleur
#define EZ_C_GOBBLER               XC_gobbler
#define EZ_C_GUMBY                 XC_gumby
#define EZ_C_HAND1                 XC_hand1
#define EZ_C_HAND2                 XC_hand2
#define EZ_C_HEART                 XC_heart
#define EZ_C_ICON                  XC_icon
#define EZ_C_IRON_CROSS            XC_iron_cross
#define EZ_C_LEFT_PTR              XC_left_ptr
#define EZ_C_LEFT_SIDE             XC_left_side
#define EZ_C_LEFT_TEE              XC_left_tee
#define EZ_C_LEFTBUTTON            XC_leftbutton
#define EZ_C_LL_ANGLE              XC_ll_angle
#define EZ_C_LR_ANGLE              XC_lr_angle
#define EZ_C_MAN                   XC_man
#define EZ_C_MIDDLEBUTTON          XC_middlebutton
#define EZ_C_MOUSE                 XC_mouse
#define EZ_C_PENCIL                XC_pencil
#define EZ_C_PIRATE                XC_pirate
#define EZ_C_PLUS                  XC_plus
#define EZ_C_QUESTION_ARROW        XC_question_arrow
#define EZ_C_RIGHT_PTR             XC_right_ptr
#define EZ_C_RIGHT_SIDE            XC_right_side
#define EZ_C_RIGHT_TEE             XC_right_tee
#define EZ_C_RIGHTBUTTON           XC_rightbutton
#define EZ_C_RTL_LOGO              XC_rtl_logo
#define EZ_C_SAILBOAT              XC_sailboat
#define EZ_C_SB_DOWN_ARROW         XC_sb_down_arrow
#define EZ_C_SB_H_DOUBLE_ARROW     XC_sb_h_double_arrow
#define EZ_C_SB_LEFT_ARROW         XC_sb_left_arrow
#define EZ_C_SB_RIGHT_ARROW        XC_sb_right_arrow
#define EZ_C_SB_UP_ARROW           XC_sb_up_arrow
#define EZ_C_SB_V_DOUBLE_ARROW     XC_sb_v_double_arrow
#define EZ_C_SHUTTLE               XC_shuttle
#define EZ_C_SIZING                XC_sizing
#define EZ_C_SPIDER                XC_spider
#define EZ_C_SPRAYCAN              XC_spraycan
#define EZ_C_STAR                  XC_star
#define EZ_C_TARGET                XC_target
#define EZ_C_TCROSS                XC_tcross
#define EZ_C_TOP_LEFT_ARROW        XC_top_left_arrow
#define EZ_C_TOP_LEFT_CORNER       XC_top_left_corner
#define EZ_C_TOP_RIGHT_CORNER      XC_top_right_corner
#define EZ_C_TOP_SIDE              XC_top_side
#define EZ_C_TOP_TEE               XC_top_tee
#define EZ_C_TREK                  XC_trek
#define EZ_C_UL_ANGLE              XC_ul_angle
#define EZ_C_UMBRELLA              XC_umbrella
#define EZ_C_UR_ANGLE              XC_ur_angle
#define EZ_C_WATCH                 XC_watch
#define EZ_C_XTERM                 XC_xterm                      
#endif
/*********************************************************/
#ifndef _EZ_CURSOR_H_
#define _EZ_CURSOR_H_
#endif
/*********************************************************/
#ifdef EZ_DefineCursor
{None, "XC_X_cursor"},
{None, NULL},
{None, "XC_arrow"},
{None, NULL},
{None, "XC_based_arrow_down"},
{None, NULL},
{None, "XC_based_arrow_up"},
{None, NULL},
{None, "XC_boat"},
{None, NULL},
{None, "XC_bogosity"},
{None, NULL},
{None, "XC_bottom_left_corner"},
{None, NULL},
{None, "XC_bottom_right_corner"},
{None, NULL},
{None, "XC_bottom_side"},
{None, NULL},
{None, "XC_bottom_tee"},
{None, NULL},
{None, "XC_box_spiral"},
{None, NULL},
{None, "XC_center_ptr"},
{None, NULL},
{None, "XC_circle"},
{None, NULL},
{None, "XC_clock"},
{None, NULL},
{None, "XC_coffee_mug"},
{None, NULL},
{None, "XC_cross"},
{None, NULL},
{None, "XC_cross_reverse"},
{None, NULL},
{None, "XC_crosshair"},
{None, NULL},
{None, "XC_diamond_cross"},
{None, NULL},
{None, "XC_dot"},
{None, NULL},
{None, "XC_dotbox"},
{None, NULL},
{None, "XC_double_arrow"},
{None, NULL},
{None, "XC_draft_large"},
{None, NULL},
{None, "XC_draft_small"},
{None, NULL},
{None, "XC_draped_box"},
{None, NULL},
{None, "XC_exchange"},
{None, NULL},
{None, "XC_fleur"},
{None, NULL},
{None, "XC_gobbler"},
{None, NULL},
{None, "XC_gumby"},
{None, NULL},
{None, "XC_hand1"},
{None, NULL},
{None, "XC_hand2"},
{None, NULL},
{None, "XC_heart"},
{None, NULL},
{None, "XC_icon"},
{None, NULL},
{None, "XC_iron_cross"},
{None, NULL},
{None, "XC_left_ptr"},
{None, NULL},
{None, "XC_left_side"},
{None, NULL},
{None, "XC_left_tee"},
{None, NULL},
{None, "XC_leftbutton"},
{None, NULL},
{None, "XC_ll_angle"},
{None, NULL},
{None, "XC_lr_angle"},
{None, NULL},
{None, "XC_man"},
{None, NULL},
{None, "XC_middlebutton"},
{None, NULL},
{None, "XC_mouse"},
{None, NULL},
{None, "XC_pencil"},
{None, NULL},
{None, "XC_pirate"},
{None, NULL},
{None, "XC_plus"},
{None, NULL},
{None, "XC_question_arrow"},
{None, NULL},
{None, "XC_right_ptr"},
{None, NULL},
{None, "XC_right_side"},
{None, NULL},
{None, "XC_right_tee"},
{None, NULL},
{None, "XC_rightbutton"},
{None, NULL},
{None, "XC_rtl_logo"},
{None, NULL},
{None, "XC_sailboat"},
{None, NULL},
{None, "XC_sb_down_arrow"},
{None, NULL},
{None, "XC_sb_h_double_arrow"},
{None, NULL},
{None, "XC_sb_left_arrow"},
{None, NULL},
{None, "XC_sb_right_arrow"},
{None, NULL},
{None, "XC_sb_up_arrow"},
{None, NULL},
{None, "XC_sb_v_double_arrow"},
{None, NULL},
{None, "XC_shuttle"},
{None, NULL},
{None, "XC_sizing"},
{None, NULL},
{None, "XC_spider"},
{None, NULL},
{None, "XC_spraycan"},
{None, NULL},
{None, "XC_star"},
{None, NULL},
{None, "XC_target"},
{None, NULL},
{None, "XC_tcross"},
{None, NULL},
{None, "XC_top_left_arrow"},
{None, NULL},
{None, "XC_top_left_corner"},
{None, NULL},
{None, "XC_top_right_corner"},
{None, NULL},
{None, "XC_top_side"},
{None, NULL},
{None, "XC_top_tee"},
{None, NULL},
{None, "XC_trek"},
{None, NULL},
{None, "XC_ul_angle"},
{None, NULL},
{None, "XC_umbrella"},
{None, NULL},
{None, "XC_ur_angle"},
{None, NULL},
{None, "XC_watch"},
{None, NULL},
{None, "XC_xterm"},
{None, NULL},
{None, NULL},
#endif
