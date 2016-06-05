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
/***************************************************************
 *
 *  tracking where the malloc calls are made
 */
#define _PIXEL_VALUE_SWAP_       1
#define _ROSTER_                 2
#define _REGIS_DATA_             3
#define _DPY_LIST_               4
#define _DPY_TOKEN_              5
#define _DPY_TOKENS_             6
#define _GIF_IMAGE_              7
#define _ROW_IMAGE_DATA_         8
#define _MATRIX_STACK1_          9
#define _MATRIX_STACK2_          10
#define _MATRIX_STACK3_          11
#define _MATERIAL1_              12
#define _LIGHT_SRC_              13
#define _LMODEL_                 14
#define _MATRIX_STACK_ENTRY_     15
#define _SPHERE_P_               16
#define _SPHERE_V3_              17
#define _SPHERE_TRIANGLE_        18
#define _SPHERE_DATA_            19
#define _SPHERE_CUBE_            20
#define _TEXT_PROP_              21
#define _TP_HT_ENTRY_            22
#define _ITEXT_LINE_             23
#define _ITEXT_LINE_D_           24
#define _ITEXT_LINE_P_           25
#define _KILL_BUF_D_             26
#define _KILL_BUF_P_             27
#define _TIMER_                  28
#define _TUBE_DATA_              29
#define _TUBE_V3_                30
#define _NEW_WIDGET_             31
#define _CALL_BACK_              32
#define _GL_CONTEXT_             33
#define _Z_BUFF_D_               34
#define _B_BUFF_DATA_            35
#define _R_BUTTON_GP_            36
#define _EMBEDER_IMAGE_          37
#define _EXECUTOR_COMMAND_       38
#define _EXECUTOR_ARGS_          39
#define _KEY_EVENT_HT_ENTRY_     40
#define _FONT_TABLE_ENTRY_       41
#define _FONT_NAME_              42
#define _WIDGET_ALL_HT_ENTRY_    43
#define _WIDGET_MAPPED_HT_ENTRY_ 44
#define _BITMAP_HT_ENTRY_        45
#define _BITMAP_NAME_            46
#define _ILIST_BOX_D_            47
#define _ITEXT_D_                48
#define _PPM_TMP_D_              49
#define _PPM_TMP_IMAGE_D_        50
#define _DITHER_TMP_D_           51
#define _ENTRY_I_D_              52
#define _ALLOC_COPY_STR_         53
#define _XPM_COLOR_              54
#define _XPM_TMP_HT_             55
#define _XPM_TMP_HT_E_           56
#define _XPM_TMP_IMAGE_D_        57
#define _XPM_TMP_MASK_D_         58
#define _XPM_CONTENTS_           59
#define _XPM_CONTENTS_D_         60
#define _LABEL_STRING_           61
#define _BUBBLE_STRING_          62
#define _CONVERT_POLY_           63
#define _PPM_HASH_ENTRY_         64
#define _DISPLAY_ITEM_           65
#define _ITEM_PROPERTY_          66
#define _IFLISTBOX_DATA_         67
#define _TREE_NODE_              68
#define _IWORK_AREA_DATA_        69
#define _PROPERTY_DATA_          70
#define _DND_LIST_               71
#define _DND_DROP_MESSAGE_       72
#define _DND_DATA_ENCODER        73
#define _DND_DATA_DECODER        74
#define _SIMPLE_MENU             75
#define _INPUT_ID_               76
#define _CBTN_DATA_              77
#define _BMPM_TMP_D_             78
#define _GRID_BAG_D_             79
#define _FOCUS_RECORD_           80
#define _JPEG_IMAGE_             81
#define _EZ_MALLOC_              82
#define _EZ_LED_DATA_            83
#define _LED_AT_                 84
#define _GEOM_MANAGER_           85
#define _U_DIR_TREE_             86
#define _SYMB_CONST_TB_          87
#define _ITEM_HTABLE_            88
#define _GARBAGE_OBJ_            89
#define _C_MESSAGE_              90
#define _C_MESSAGE_HANDLE_       91
#define _GL_IMAGE_               92
#define _DIR_TNI_INFO_           93
#define _CLIENT_DATA_            94
#define _WM_HINTS_DATA_          95
#define _MISC_DATA_              96
#define _IWH_ITEM_DATA_          97
#define _ITRH_ITEM_DATA_         98
#define _IFLBH_ITEM_DATA_        99
#define _FIG_ITEM_DATA_          100
#define _PNG_IMAGE_              101
#define _TIFF_IMAGE_             102
#define _TTERM_DATA_             103
#define _HISTOGRAM_              104
#define _RES_EDIT_               105
#define _SSHEET_DATA_            106

#if defined(EZ_MEM_DEBUG) || defined(EZ_DEBUG)
EXTERN void initialize_mem_debug_data MY_ANSIARGS((void));
EXTERN void *my_malloc                MY_ANSIARGS((int n, int info));
EXTERN void *my_realloc               MY_ANSIARGS((void *ptr, int n, int info));
EXTERN void my_free                   MY_ANSIARGS((void *ptr));
#else
#define my_malloc(_a_, _info_)  malloc(_a_)
#define my_realloc(_a_,_b_, _info_) realloc((_a_),(_b_))
#define my_free(_a_) free(_a_)
#endif
