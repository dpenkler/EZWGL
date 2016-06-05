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
/*******************************************************************
 *
 *  Configuration options for EZ_Widgets.
 *
 */

#ifndef _EZ_WIDGET_CONFIG_H_
#define _EZ_WIDGET_CONFIG_H_
/*
 *  This is the list of options for EZ_Widgets. 
 *
 *   !!!!! Don't Change the order !!!!
 *
 *  When adding new options, make sure add an entry in
 *  the macro table below, it is used in a few places.
 *
 * if modified, need to modify  EZ_WidgetAttr.h, EZ_ResourceGet.c also
 * touch EZ_CreateWidget.c EZ_CreateItem.c EZ_WidgetConfigure.c EZ_Resource.c
 */
#define EZ_NOTHING                            0
#define EZ_X                                  1
#define EZ_Y                                  2
#define EZ_WIDTH                              3
#define EZ_HEIGHT                             4
#define EZ_WIDTH_HINT                         5
#define EZ_HEIGHT_HINT                        6
#define EZ_BORDER_WIDTH                       7
#define EZ_BORDERWIDTH                        EZ_BORDER_WIDTH
#define EZ_BORDER_TYPE                        8
#define EZ_BORDER_STYLE                       EZ_BORDER_TYPE
#define EZ_PADX                               9
#define EZ_PADY                               10
#define EZ_IPADX                              11
#define EZ_IPADY                              12
#define EZ_STACKING                           13
#define EZ_ORIENTATION                        EZ_STACKING
#define EZ_SIDE                               14
#define EZ_LABEL_POSITION                     15
#define EZ_LABELPOSITION                      EZ_LABEL_POSITION
#define EZ_JUSTIFICATION                      16
#define EZ_LABEL_JUSTIFICATION                EZ_JUSTIFICATION
#define EZ_FILL_MODE                          17
#define EZ_PROPAGATE                          18
#define EZ_CALL_BACK                          19
#define EZ_CALLBACK                           EZ_CALL_BACK
#define EZ_FOREGROUND                         20
#define EZ_BACKGROUND                         21
#define EZ_BITMAP_FILE                        22
#define EZ_PIXMAP_FILE                        EZ_BITMAP_FILE
#define EZ_BITMAP_DATA                        23   
#define EZ_PIXMAP_DATA                        24
#define EZ_X_PIXMAP                           25
#define EZ_LABEL_PIXMAP                       26
#define EZ_LABELPIXMAP                        EZ_LABEL_PIXMAP
#define EZ_FONT_ID                            27
#define EZ_FONT_NAME                          28
#define EZ_TEXT_LINE_LENGTH                   29
#define EZ_SLIDER_LENGTH                      30
#define EZ_SLIDERLENGTH                       EZ_SLIDER_LENGTH
#define EZ_SLIDER_WIDTH                       31
#define EZ_SLIDERWIDTH                        EZ_SLIDER_WIDTH
#define EZ_SLIDER_BORDER_WIDTH                32
#define EZ_SLIDER_BORDERWIDTH                 EZ_SLIDER_BORDER_WIDTH
#define EZ_SLIDER_RESOLUTION                  33
#define EZ_SLIDER_RANGE                       34
#define EZ_INDICATOR_SIZE_ADJUST              35
#define EZ_IMAGE_FILE                         36
#define EZ_BUTTON_SHORTCUT                    37
#define EZ_SHORTCUT_KEY                       EZ_BUTTON_SHORTCUT  
#define EZ_EVENT_HANDLE                       38
#define EZ_EVENT_HANDLER                      EZ_EVENT_HANDLE
#define EZ_LABEL_STRING                       39
#define EZ_RESERVE_MENU_BUTTON                40
#define EZ_BACKING_STORE                      41
#define EZ_EXPAND                             42  
#define EZ_TRANSIENT                          43
#define EZ_SCROLLBAR_WIDTH                    44
#define EZ_SCROLLBAR_BORDER_WIDTH             45
#define EZ_TEXT_SPACING                       46
#define EZ_UNDERLINE                          47
#define EZ_BUBBLE_STRING                      48
#define EZ_BUBBLE_HELP                        EZ_BUBBLE_STRING
#define EZ_MOVABLE                            49
#define EZ_FREELABEL_MOVABLE                  EZ_MOVABLE
#define EZ_ITEM_MOVABLE                       EZ_FREELABEL_MOVABLE
#define EZ_INDICATOR_TYPE                     50
#define EZ_INDICATOR_COLOR                    51
#define EZ_TEXT_BACKGROUND                    52  
#define EZ_SLIDER_DISPLAY_VALUE               53
#define EZ_HIGHLIGHT_FOREGROUND               54
#define EZ_HIGHLIGHT_FG                       EZ_HIGHLIGHT_FOREGROUND
#define EZ_HIGHLIGHT_BACKGROUND               55
#define EZ_HIGHLIGHT_BG                       EZ_HIGHLIGHT_BACKGROUND
#define EZ_ATTACH_PTR_DATA                    56
#define EZ_ATTACH_INT_DATA                    57
#define EZ_USER_PTR_DATA                      EZ_ATTACH_PTR_DATA    
#define EZ_USER_INT_DATA                      EZ_ATTACH_INT_DATA
#define EZ_CLIENT_PTR_DATA                    EZ_ATTACH_PTR_DATA    
#define EZ_CLIENT_INT_DATA                    EZ_ATTACH_INT_DATA
#define EZ_DESTROY_CALLBACK                   58
#define EZ_EMBEDER                            59
#define EZ_IS_EMBEDER                         EZ_EMBEDER
#define EZ_DND_DRAG_CURSOR                    60
#define EZ_DND_BUBBLE_STRING                  61
#define EZ_DND_BUBBLE_HELP                    EZ_DND_BUBBLE_STRING
#define EZ_SHAPED_WINDOW                      62
#define EZ_MOTION_CALLBACK                    63
#define EZ_CHECK_BUTTON_ON_VALUE              64
#define EZ_CHECK_BUTTON_OFF_VALUE             65
#define EZ_CHECK_BUTTON_ON_OFF                66
#define EZ_RADIO_BUTTON_GROUP                 67
#define EZ_RADIO_BUTTON_VALUE                 68
#define EZ_OPTIONAL_HSCROLLBAR                69
#define EZ_OPTIONAL_VSCROLLBAR                70
#define EZ_FANCY_LISTBOX_TYPE                 71
#define EZ_FANCY_LIST_BOX_TYPE                EZ_FANCY_LISTBOX_TYPE 
#define EZ_FANCY_LISTBOX_COLUMNS              72
#define EZ_FANCY_LIST_BOX_COLUMNS             EZ_FANCY_LISTBOX_COLUMNS
#define EZ_GLOB_PATTERN                       73
#define EZ_TEXT_WIDGET_EDITABLE               74
#define EZ_ENTRY_STRING                       75
#define EZ_RETURN_VALUE                       76
#define EZ_SLIDER_INIT_VALUE                  77
#define EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY    78
#define EZ_OPTIONAL_ENTRY_HISTORY             78
#define EZ_ENTRY_EDITABLE                     79
#define EZ_OPTIONAL_ENTRY_EDITABLE            EZ_ENTRY_EDITABLE
#define EZ_WIDGET_ITEM_WIDGET                 80
#define EZ_ITEM_WIDGET_WIDGET                 EZ_WIDGET_ITEM_WIDGET
#define EZ_RAW_RGB_DATA                       81
#define EZ_MENU_TEAR_OFF                      82
#define EZ_GRID_CELL_GEOMETRY                 83
#define EZ_GRID_CELL_PLACEMENT                84
#define EZ_GRID_CONSTRAINS                    85
#define EZ_GRID_ROW_CONSTRAINS                86
#define EZ_GRID_COLUMN_CONSTRAINS             87
#define EZ_LOCATION                           88
#define EZ_SIZE                               89
#define EZ_DIMENSION                          EZ_SIZE
#define EZ_GEOMETRY                           90
#define EZ_SIZE_HINT                          91
#define EZ_PADB                               92
#define EZ_HIGHLIGHT_PAD                      EZ_PADB
#define EZ_OPAD                               EZ_PADB
#define EZ_FOCUS_PAD                          EZ_PADB
#define EZ_NAME                               93
#define EZ_LCD_NDIGITS                        94
#define EZ_LCD_FONT_SIZE                      95
#define EZ_LCD_BACKGROUND                     96
#define EZ_LCD_FOREGROUND                     97
#define EZ_LED_BACKGROUND                     98
#define EZ_LED_PIXEL_COLOR                    99
#define EZ_LED_WIDTH                          100
#define EZ_LED_HEIGHT                         101
#define EZ_LED_PIXEL_SIZE                     102
#define EZ_LED_COLOR_FUNCTION                 103
#define EZ_WMHINTS                            104
#define EZ_WM_HINTS                           EZ_WMHINTS
#define EZ_GEOMETRY_MANAGER                   105
#define EZ_CURSOR                             106
#define EZ_CLASS                              107
#define EZ_POPUP_NAME                         108
#define EZ_LABELED_ICON                       109
#define EZ_HSCROLL_INCREMENT                  110
#define EZ_VSCROLL_INCREMENT                  111
#define EZ_SMETER_VALUE                       112
#define EZ_SMETER_RANGE                       113
#define EZ_SMETER_STOP                        114
#define EZ_SMETER_STOPED                      EZ_SMETER_STOP
#define EZ_SMETER_BACKGROUND                  115
#define EZ_SMETER_FOREGROUND                  116
#define EZ_SMETER_FOREGROUND_N                117
#define EZ_SMETER_SHOW_VALUE                  118
#define EZ_SMETER_VALUE_N                     119
#define EZ_SMETER_RANGE_N                     120
#define EZ_SMETER_STYLE                       121
#define EZ_SMETER_ORIENTATION                 122
#define EZ_FORGET_X                           123
#define EZ_FORGET_Y                           124
#define EZ_FORGET_W                           125
#define EZ_FORGET_H                           126
#define EZ_FORGET_LOCATION                    127
#define EZ_FORGET_POSITION                    EZ_FORGET_LOCATION
#define EZ_FORGET_SIZE                        128
#define EZ_FORGET_GEOMETRY                    129
#define EZ_WM_WINDOW_NAME                     130
#define EZ_WM_INITIAL_STATE                   131	
#define EZ_WM_ICON_NAME                       132
#define EZ_WM_ICON_XPM_FILE                   133
#define EZ_WM_ICON_PIXMAP                     134
#define EZ_WM_ICON_LOCATION                   135
#define EZ_WM_LOCATION_HINT                   136
#define EZ_WM_SIZE_HINT                       137
#define EZ_WM_MAX_SIZE_HINT                   138
#define EZ_WM_MIN_SIZE_HINT                   139
#define EZ_MARK_CLEAN                         140
#define EZ_HIGHLIGHT_MODE                     141
#define EZ_RUBBER_BAND                        142
#define EZ_BG_IMAGE_FILE                      143
#define EZ_BG_PIXMAP                          144
#define EZ_BG_IMAGE_FILE_B                    145
#define EZ_BG_PIXMAP_B                        146
#define EZ_DRAG_HANDLE                        147
#define EZ_DOCKABLE                           148
#define EZ_SLIDER_STYLE                       149
#define EZ_ROW_BG                             150
#define EZ_ARROW_LABEL                        151
#define EZ_ARROW_TYPE                         EZ_ARROW_LABEL
#define EZ_INTERIOR_BORDER                    152
#define EZ_OPTIONAL_ENTRY_STYLE               153
#define EZ_TILE_ORIGIN                        154
#define EZ_WM_IGNORE                          155
#define EZ_LABEL_SHADOW                       156
#define EZ_TERM_COLOR_N                       157
#define EZ_TERM_CURSOR_COLOR                  158
#define EZ_TERM_SAVE_LINES                    159
#define EZ_TERM_REVERSE_VIDEO                 160
#define EZ_SELECTION_BACKGROUND               161
#define EZ_SELECTION_FOREGROUND               162
#define EZ_RULER_DISPLAY_VALUE                163
#define EZ_RULER_TICK_UNIT                    164
#define EZ_RULER_TICK_LENGTH                  165
#define EZ_RULER_POSITION                     166
#define EZ_RULER_ORIENTATION                  EZ_RULER_POSITION
#define EZ_RULER_TICK_SCALE                   167
#define EZ_RULER_TICK_OFFSET                  168
#define EZ_OVERRIDE_REDIRECT                  169
#define EZ_RESOURCES_HANDLER                  170
#define EZ_RESOURCES_HANDLE                   EZ_RESOURCES_HANDLER
#define EZ_SCROLL_TYPE                        171
#define EZ_SPIN_FUNCTION                      172
#define EZ_SPIN_VALUE                         173
#define EZ_LOCATOR_XY                         174
#define EZ_SPECIAL_EFFECT                     175
#define EZ_SHOW_TICK                          176
#define EZ_DIAL_RANGE                         177
#define EZ_DIAL_RESOLUTION                    178
#define EZ_DIAL_SPAN                          179
#define EZ_DIAL_DISPLAY_VALUE                 180
#define EZ_DIAL_STYLE                         181
#define EZ_DIAL_COLOR                         182
#define EZ_DIAL_VALUE                         183
#define EZ_CURSOR_NAME                        184
#define EZ_DND_DRAG_CURSOR_NAME               185
#define EZ_SHAPE_FILE                         186
#define EZ_SHAPE_PIXMAP                       187
#define EZ_WRAP_TEXT                          188

#define EZ_BAR_EDITABLE                       189
#define EZ_BAR_ORIENTATION                    190
#define EZ_BAR_COLOR_N                        191
#define EZ_BAR_RANGE                          192
#define EZ_BAR_WIDTH                          193
#define EZ_HISTOGRAM_SAMPLE                   194
#define EZ_SEPARATOR_STYLE                    195

#define EZ_ROW_COLUMN_MAX_SIZE                196 

#define EZ_RESOURCE_STRING                    197 

#define EZ_SHEET_HEADER_SIZE                  198
#define EZ_SHEET_CELL_SIZE                    199

#define EZ_WIDGET_CONFIGURE_LAST_OPTION       199
#endif
/**************************************************************************/
#ifdef EZ_OptionIdNames
EZ_OptionIdNames( EZ_NOTHING,  "i", "EZ_NOTHING", "nothing", "Nothing")
EZ_OptionIdNames( EZ_X,  "i", "EZ_X", "x", "X")
EZ_OptionIdNames( EZ_Y,  "i", "EZ_Y", "y", "Y")
EZ_OptionIdNames( EZ_WIDTH, "i",  "EZ_WIDTH", "width", "Width")
EZ_OptionIdNames( EZ_HEIGHT, "i",  "EZ_HEIGHT", "height", "Height")
EZ_OptionIdNames( EZ_WIDTH_HINT, "i",  "EZ_WIDTH_HINT", "widthHint", "WidthHint")
EZ_OptionIdNames( EZ_HEIGHT_HINT, "i",  "EZ_HEIGHT_HINT", "heightHint", "HeightHint")
EZ_OptionIdNames( EZ_BORDER_WIDTH, "i",  "EZ_BORDER_WIDTH", "borderWidth", "BorderWidth")
EZ_OptionIdNames( EZ_BORDER_TYPE,  "e", "EZ_BORDER_TYPE", "borderType", "BorderType")
EZ_OptionIdNames( EZ_PADX, "i",  "EZ_PADX", "padx", "Padx")
EZ_OptionIdNames( EZ_PADY, "i",  "EZ_PADY", "pady", "Pady")
EZ_OptionIdNames( EZ_IPADX, "i",   "EZ_IPADX", "iPadx", "IPadx")
EZ_OptionIdNames( EZ_IPADY, "i",  "EZ_IPADY", "iPady", "IPady")
EZ_OptionIdNames( EZ_ORIENTATION, "e",   "EZ_ORIENTATION", "orientation", "Orientation")
EZ_OptionIdNames( EZ_SIDE, "e",  "EZ_SIDE", "side", "Side")
EZ_OptionIdNames( EZ_LABEL_POSITION, "e",  "EZ_LABEL_POSITION", "labelPosition", "LabelPosition")
EZ_OptionIdNames( EZ_JUSTIFICATION, "e",  "EZ_JUSTIFICATION", "justification", "Justification")
EZ_OptionIdNames( EZ_FILL_MODE, "e",  "EZ_FILL_MODE", "fillMode", "FillMode")
EZ_OptionIdNames( EZ_PROPAGATE, "b",  "EZ_PROPAGATE", "propagate", "Propagate")
EZ_OptionIdNames( EZ_CALLBACK,  "hp", "EZ_CALLBACK", "callback", "Callback")
EZ_OptionIdNames( EZ_FOREGROUND, "r",  "EZ_FOREGROUND", "foreground", "Foreground")
EZ_OptionIdNames( EZ_BACKGROUND,  "r", "EZ_BACKGROUND", "background", "Background")
EZ_OptionIdNames( EZ_PIXMAP_FILE,  "F", "EZ_PIXMAP_FILE", "pixmapFile", "PixmapFile")
EZ_OptionIdNames( EZ_BITMAP_DATA,  "p", "EZ_BITMAP_DATA", "bitmapData", "BitmapData")
EZ_OptionIdNames( EZ_PIXMAP_DATA,  "p", "EZ_PIXMAP_DATA", "pixmapData", "PixmapData")
EZ_OptionIdNames( EZ_X_PIXMAP,  "p", "EZ_X_PIXMAP", "xPixmap", "XPixmap")
EZ_OptionIdNames( EZ_LABELPIXMAP, "p",  "EZ_LABELPIXMAP", "labelPixmap", "LabelPixmap")
EZ_OptionIdNames( EZ_FONT_ID, "i",  "EZ_FONT_ID", "fontID", "FontID")
EZ_OptionIdNames( EZ_FONT_NAME, "S",  "EZ_FONT_NAME", "fontName", "FontName")
EZ_OptionIdNames( EZ_TEXT_LINE_LENGTH, "i",   "EZ_TEXT_LINE_LENGTH", "textLineLength", "TextLineLength")
EZ_OptionIdNames( EZ_SLIDER_LENGTH, "i",  "EZ_SLIDER_LENGTH", "sliderLength", "SliderLength")
EZ_OptionIdNames( EZ_SLIDER_WIDTH, "i",  "EZ_SLIDER_WIDTH", "sliderWidth", "SliderWidth")
EZ_OptionIdNames( EZ_SLIDER_BORDER_WIDTH, "i",  "EZ_SLIDER_BORDER_WIDTH", "sliderBorderWidth", "SliderBorderWidth")
EZ_OptionIdNames( EZ_SLIDER_RESOLUTION,  "f", "EZ_SLIDER_RESOLUTION", "sliderResolution", "SliderResolution")
EZ_OptionIdNames( EZ_SLIDER_RANGE,  "ff", "EZ_SLIDER_RANGE", "sliderRange", "SliderRange")
EZ_OptionIdNames( EZ_INDICATOR_SIZE_ADJUST, "i", "EZ_INDICATOR_SIZE_ADJUST", "indicatorSizeAdjust", "IndicatorSizeAdjust")
EZ_OptionIdNames( EZ_IMAGE_FILE,  "F", "EZ_IMAGE_FILE", "imageFile", "ImageFile")
EZ_OptionIdNames( EZ_SHORTCUT_KEY, "S",  "EZ_SHORTCUT_KEY", "shortcutKey", "ShortcutKey")
EZ_OptionIdNames( EZ_EVENT_HANDLER, "hp",  "EZ_EVENT_HANDLER", "eventHandler", "EventHandler")
EZ_OptionIdNames( EZ_LABEL_STRING, "S",  "EZ_LABEL_STRING", "labelString", "LabelString")
EZ_OptionIdNames( EZ_RESERVE_MENU_BUTTON, "b",  "EZ_RESERVE_MENU_BUTTON", "reserveMenuButton", "ReserveMenuButton")
EZ_OptionIdNames( EZ_BACKING_STORE, "b",  "EZ_BACKING_STORE", "backingStore", "BackingStore")
EZ_OptionIdNames( EZ_EXPAND,  "b", "EZ_EXPAND", "expand", "Expand")
EZ_OptionIdNames( EZ_TRANSIENT, "b",  "EZ_TRANSIENT", "transient", "Transient")
EZ_OptionIdNames( EZ_SCROLLBAR_WIDTH,"i",   "EZ_SCROLLBAR_WIDTH", "scrollbarWidth", "ScrollbarWidth")
EZ_OptionIdNames( EZ_SCROLLBAR_BORDER_WIDTH, "i",  "EZ_SCROLLBAR_BORDER_WIDTH", "scrollbarBorderWidth", "ScrollbarBorderWidth")
EZ_OptionIdNames( EZ_TEXT_SPACING, "i",  "EZ_TEXT_SPACING", "textSpacing", "TextSpacing")
EZ_OptionIdNames( EZ_UNDERLINE, "i",  "EZ_UNDERLINE", "underline", "Underline")
EZ_OptionIdNames( EZ_BUBBLE_STRING, "S",  "EZ_BUBBLE_STRING", "bubbleString", "BubbleString")
EZ_OptionIdNames( EZ_MOVABLE, "b",  "EZ_MOVABLE", "movable", "Movable")
EZ_OptionIdNames( EZ_INDICATOR_TYPE, "e",   "EZ_INDICATOR_TYPE", "indicatorType", "IndicatorType")
EZ_OptionIdNames( EZ_INDICATOR_COLOR, "r",   "EZ_INDICATOR_COLOR", "indicatorColor", "IndicatorColor")
EZ_OptionIdNames( EZ_TEXT_BACKGROUND,  "r", "EZ_TEXT_BACKGROUND", "textBackground", "TextBackground")
EZ_OptionIdNames( EZ_SLIDER_DISPLAY_VALUE, "b",   "EZ_SLIDER_DISPLAY_VALUE", "sliderDisplayValue", "SliderDisplayValue")
EZ_OptionIdNames( EZ_SELECTION_BACKGROUND, "r",  "EZ_SELECTION_BACKGROUND", "selectionBackground", "SelectionBackground")
EZ_OptionIdNames( EZ_SELECTION_FOREGROUND, "r",  "EZ_SELECTION_FOREGROUND", "selectionForeground", "SelectionForeground")
EZ_OptionIdNames( EZ_CLIENT_PTR_DATA, "p",  "EZ_CLIENT_PTR_DATA", "clientPtrData", "ClientPtrData")
EZ_OptionIdNames( EZ_CLIENT_INT_DATA, "i",  "EZ_CLIENT_INT_DATA", "clientIntData", "ClientIntData")
EZ_OptionIdNames( EZ_DESTROY_CALLBACK, "hp",  "EZ_DESTROY_CALLBACK", "destroyCallback", "DestroyCallback")
EZ_OptionIdNames( EZ_IS_EMBEDER, "b",  "EZ_IS_EMBEDER", "isEmbeder", "IsEmbeder")
EZ_OptionIdNames( EZ_DND_DRAG_CURSOR, "p",  "EZ_DND_DRAG_CURSOR", "dndDragCursor", "DndDragCursor")
EZ_OptionIdNames( EZ_DND_BUBBLE_STRING, "S",   "EZ_DND_BUBBLE_STRING", "dndBubbleString", "DndBubbleString")
EZ_OptionIdNames( EZ_SHAPED_WINDOW, "b",  "EZ_SHAPED_WINDOW", "shapedWindow", "ShapedWindow")
EZ_OptionIdNames( EZ_MOTION_CALLBACK, "pp",  "EZ_MOTION_CALLBACK", "motionCallback", "MotionCallback")
EZ_OptionIdNames( EZ_CHECK_BUTTON_ON_VALUE, "i",   "EZ_CHECK_BUTTON_ON_VALUE", "checkButtonOnValue", "CheckButtonOnValue")
EZ_OptionIdNames( EZ_CHECK_BUTTON_OFF_VALUE, "i",  "EZ_CHECK_BUTTON_OFF_VALUE", "checkButtonOffValue", "CheckButtonOffValue")
EZ_OptionIdNames( EZ_CHECK_BUTTON_ON_OFF, "b",  "EZ_CHECK_BUTTON_ON_OFF", "checkButtonOnOff", "CheckButtonOnOff")
EZ_OptionIdNames( EZ_RADIO_BUTTON_GROUP, "i",  "EZ_RADIO_BUTTON_GROUP", "radioButtonGroup", "RadioButtonGroup")
EZ_OptionIdNames( EZ_RADIO_BUTTON_VALUE, "i",  "EZ_RADIO_BUTTON_VALUE", "radioButtonValue", "RadioButtonValue")
EZ_OptionIdNames( EZ_OPTIONAL_HSCROLLBAR, "b",  "EZ_OPTIONAL_HSCROLLBAR", "optionalHScrollbar", "OptionalHScrollbar")
EZ_OptionIdNames( EZ_OPTIONAL_VSCROLLBAR, "b",   "EZ_OPTIONAL_VSCROLLBAR", "optionalVScrollbar", "OptionalVScrollbar")
EZ_OptionIdNames( EZ_FANCY_LISTBOX_TYPE, "e",  "EZ_FANCY_LISTBOX_TYPE", "fancyListBoxType", "FancyListBoxType")
EZ_OptionIdNames( EZ_FANCY_LISTBOX_COLUMNS, "i",   "EZ_FANCY_LISTBOX_COLUMNS", "fancyListBoxColumns", "FancyListBoxColumns")
EZ_OptionIdNames( EZ_GLOB_PATTERN, "S",  "EZ_GLOB_PATTERN", "globPattern", "GlobPattern")
EZ_OptionIdNames( EZ_TEXT_WIDGET_EDITABLE, "b",  "EZ_TEXT_WIDGET_EDITABLE", "textWidgetEditable", "TextWidgetEditable")
EZ_OptionIdNames( EZ_ENTRY_STRING, "S",   "EZ_ENTRY_STRING", "entryString", "entrystring")
EZ_OptionIdNames( EZ_RETURN_VALUE,  "i",  "EZ_RETURN_VALUE", "returnValue", "ReturnValue")
EZ_OptionIdNames( EZ_SLIDER_INIT_VALUE, "f",   "EZ_SLIDER_INIT_VALUE", "sliderInitialValue", "SliderInitialValue")
EZ_OptionIdNames( EZ_OPTIONAL_ENTRY_HISTORY, "b",   "EZ_OPTIONAL_ENTRY_HISTORY", "optionalEntryHistory", "OptionalEntryHistory")
EZ_OptionIdNames( EZ_ENTRY_EDITABLE, "b",  "EZ_ENTRY_EDITABLE", "entryEditable", "EntryEditable")
EZ_OptionIdNames( EZ_WIDGET_ITEM_WIDGET, "p",   "EZ_WIDGET_ITEM_WIDGET", "widgetItemWidget", "WidgetItemWidget")
EZ_OptionIdNames( EZ_RAW_RGB_DATA, "p",  "EZ_RAW_RGB_DATA", "rawRGBData", "RawRGBData")
EZ_OptionIdNames( EZ_MENU_TEAR_OFF, "b",  "EZ_MENU_TEAR_OFF", "menuTearOff", "MenuTearOff")
EZ_OptionIdNames( EZ_GRID_CELL_GEOMETRY, "iiii",  "EZ_GRID_CELL_GEOMETRY", "gridCellGeometry", "GridCellGeometry")
EZ_OptionIdNames( EZ_GRID_CELL_PLACEMENT, "ee",  "EZ_GRID_CELL_PLACEMENT", "gridCellPlacement", "GridCellPlacement")
EZ_OptionIdNames( EZ_GRID_CONSTRAINS, "iiiii.",  "EZ_GRID_CONSTRAINS", "gridConstrains", "GridConstrains")
EZ_OptionIdNames( EZ_GRID_ROW_CONSTRAINS, "iiii.",  "EZ_GRID_ROW_CONSTRAINS", "gridRowConstrains", "GridRowConstrains")
EZ_OptionIdNames( EZ_GRID_COLUMN_CONSTRAINS, "iiii.",  "EZ_GRID_COLUMN_CONSTRAINS", "gridColumnConstrains", "GridColumnConstrains")
EZ_OptionIdNames( EZ_LOCATION, "ii",  "EZ_LOCATION", "location", "Location")
EZ_OptionIdNames( EZ_SIZE, "ii",  "EZ_SIZE", "size", "Size")
EZ_OptionIdNames( EZ_GEOMETRY, "iiii",  "EZ_GEOMETRY", "geometry", "Geometry")
EZ_OptionIdNames( EZ_SIZE_HINT, "ii",   "EZ_SIZE_HINT", "sizeHint", "SizeHint")
EZ_OptionIdNames( EZ_FOCUS_PAD, "i",  "EZ_FOCUS_PAD", "focusPad", "FocusPad")
EZ_OptionIdNames( EZ_NAME, "S",  "EZ_NAME", "name", "Name")
EZ_OptionIdNames( EZ_LCD_NDIGITS, "i",   "EZ_LCD_NDIGITS", "lcdNDigits", "LcdNDigits")
EZ_OptionIdNames( EZ_LCD_FONT_SIZE,"i",   "EZ_LCD_FONT_SIZE", "lcdFontSize", "LcdFontSize")
EZ_OptionIdNames( EZ_LCD_BACKGROUND, "r",  "EZ_LCD_BACKGROUND", "lcdBackground", "LcdBackground")
EZ_OptionIdNames( EZ_LCD_FOREGROUND, "r",  "EZ_LCD_FOREGROUND", "lcdForeground", "LcdForeground")
EZ_OptionIdNames( EZ_LED_BACKGROUND, "r",  "EZ_LED_BACKGROUND", "ledBackground", "LedBackground")
EZ_OptionIdNames( EZ_LED_PIXEL_COLOR, "r",  "EZ_LED_PIXEL_COLOR", "ledPixelColor", "LedPixelColor")
EZ_OptionIdNames( EZ_LED_WIDTH, "i",  "EZ_LED_WIDTH", "ledWidth", "LedWidth")
EZ_OptionIdNames( EZ_LED_HEIGHT,"i",   "EZ_LED_HEIGHT", "ledHeight", "LedHeight")
EZ_OptionIdNames( EZ_LED_PIXEL_SIZE, "ii",  "EZ_LED_PIXEL_SIZE", "ledPixelSize", "LedPixelSize")
EZ_OptionIdNames( EZ_LED_COLOR_FUNCTION, "h",  "EZ_LED_COLOR_FUNCTION", "ledColorFunction", "LedColorFunction")
EZ_OptionIdNames( EZ_WMHINTS,  "i", "EZ_WMHINTS", "wmHints", "WMHints")
EZ_OptionIdNames( EZ_GEOMETRY_MANAGER, "i",  "EZ_GEOMETRY_MANAGER", "geometryManager", "GeometryManager")
EZ_OptionIdNames( EZ_CURSOR,  "p", "EZ_CURSOR", "cursor", "Cursor")
EZ_OptionIdNames( EZ_CLASS, "S",   "EZ_CLASS", "class", "Class")
EZ_OptionIdNames( EZ_POPUP_NAME, "S",  "EZ_POPUP_NAME", "popupName", "PopupName")
EZ_OptionIdNames( EZ_LABELED_ICON, "b",  "EZ_LABELED_ICON", "labeledIcon", "LabeledIcon")
EZ_OptionIdNames( EZ_HSCROLL_INCREMENT, "i",  "EZ_HSCROLL_INCREMENT", "hScrollIncrement", "HScrollIncrement")
EZ_OptionIdNames( EZ_VSCROLL_INCREMENT, "i",  "EZ_VSCROLL_INCREMENT", "vScrollIncrement", "VScrollIncrement")
EZ_OptionIdNames( EZ_SMETER_VALUE, "f",  "EZ_SMETER_VALUE", "statusMeterValue", "StatusMeterValue")
EZ_OptionIdNames( EZ_SMETER_RANGE, "ff",  "EZ_SMETER_RANGE", "statusMeterRange", "StatusMeterRange")
EZ_OptionIdNames( EZ_SMETER_STOP, "b",  "EZ_SMETER_STOP", "statusMeterStop", "StatusMeterStop")
EZ_OptionIdNames( EZ_SMETER_BACKGROUND, "r",   "EZ_SMETER_BACKGROUND", "statusMeterBackground", "StatusMeterBackground")
EZ_OptionIdNames( EZ_SMETER_FOREGROUND,  "r",  "EZ_SMETER_FOREGROUND", "statusMeterForeground", "StatusMeterForeground")
EZ_OptionIdNames( EZ_SMETER_FOREGROUND_N, "ir.",  "EZ_SMETER_FOREGROUND_N", "statusMeterForegroundN", "StatusMeterForegroundN")
EZ_OptionIdNames( EZ_SMETER_SHOW_VALUE, "b",  "EZ_SMETER_SHOW_VALUE", "statusMeterShowValue", "StatusMeterShowValue")
EZ_OptionIdNames( EZ_SMETER_VALUE_N, "if.",  "EZ_SMETER_VALUE_N", "statusMeterValueN", "StatusMeterValueN")
EZ_OptionIdNames( EZ_SMETER_RANGE_N, "if.",  "EZ_SMETER_RANGE_N", "statusMeterRangeN", "StatusMeterRangeN")
EZ_OptionIdNames( EZ_SMETER_STYLE,  "ii", "EZ_SMETER_STYLE", "statusMeterStyle", "StatusMeterStyle")
EZ_OptionIdNames( EZ_SMETER_ORIENTATION, "e",   "EZ_SMETER_ORIENTATION", "statusMeterOrientation", "StatusMeterOrientation")
EZ_OptionIdNames( EZ_FORGET_X, "b",  "EZ_FORGET_X", "forgetX", "ForgetX")
EZ_OptionIdNames( EZ_FORGET_Y, "b",  "EZ_FORGET_Y", "forgetY", "ForgetY")
EZ_OptionIdNames( EZ_FORGET_W, "b",  "EZ_FORGET_W", "forgetW", "ForgetW")
EZ_OptionIdNames( EZ_FORGET_H, "b",  "EZ_FORGET_H", "forgetH", "ForgetH")
EZ_OptionIdNames( EZ_FORGET_LOCATION, "b",   "EZ_FORGET_LOCATION", "forgetLocation", "ForgetLocation")
EZ_OptionIdNames( EZ_FORGET_SIZE, "b",  "EZ_FORGET_SIZE", "forgetSize", "ForgetSize")
EZ_OptionIdNames( EZ_FORGET_GEOMETRY, "b",  "EZ_FORGET_GEOMETRY", "forgetGeometry", "ForgetGeometry")
EZ_OptionIdNames( EZ_WM_WINDOW_NAME, "S",  "EZ_WM_WINDOW_NAME", "wmWindowName", "WmWindowName")
EZ_OptionIdNames( EZ_WM_INITIAL_STATE, "e",  "EZ_WM_INITIAL_STATE", "wmInitialState", "WmInitialState")
EZ_OptionIdNames( EZ_WM_ICON_NAME, "S",  "EZ_WM_ICON_NAME", "wmIconName", "WmIconName")
EZ_OptionIdNames( EZ_WM_ICON_XPM_FILE, "F",  "EZ_WM_ICON_XPM_FILE", "wmIconXpmFile", "WmIconXpmFile")
EZ_OptionIdNames( EZ_WM_ICON_PIXMAP, "p",  "EZ_WM_ICON_PIXMAP", "wmIconPixmap", "WmIconPixmap")
EZ_OptionIdNames( EZ_WM_ICON_LOCATION, "ii",  "EZ_WM_ICON_LOCATION", "wmIconLocation", "WmIconLocation")
EZ_OptionIdNames( EZ_WM_LOCATION_HINT,  "ii", "EZ_WM_LOCATION_HINT", "wmLocation", "WmLocation")
EZ_OptionIdNames( EZ_WM_SIZE_HINT, "ii",  "EZ_WM_SIZE_HINT", "wmSizeHint", "WmSizeHint")
EZ_OptionIdNames( EZ_WM_MIN_SIZE_HINT, "ii",  "EZ_WM_MIN_SIZE_HINT", "wmMinSizeHint", "WmMinSizeHint")
EZ_OptionIdNames( EZ_WM_MAX_SIZE_HINT, "ii",  "EZ_WM_MAX_SIZE_HINT", "wmMaxSizeHint", "WmMaxSizeHint")
EZ_OptionIdNames( EZ_MARK_CLEAN, "b",  "EZ_MARK_CLEAN", "markClean", "MarkClean")
EZ_OptionIdNames( EZ_HIGHLIGHT_MODE, "e",  "EZ_HIGHLIGHT_MODE", "highlightMode", "HighlightMode")
EZ_OptionIdNames( EZ_RUBBER_BAND, "b",  "EZ_RUBBER_BAND", "rubberBand", "RubberBand")
EZ_OptionIdNames( EZ_BG_IMAGE_FILE, "F",  "EZ_BG_IMAGE_FILE", "bgImageFile", "BgImageFile")
EZ_OptionIdNames( EZ_BG_PIXMAP, "p",  "EZ_BG_PIXMAP", "bgPixmap", "BgPixmap")
EZ_OptionIdNames( EZ_BG_IMAGE_FILE_B, "F",   "EZ_BG_IMAGE_FILE_B", "bgImageFileB", "BgImageFileB")
EZ_OptionIdNames( EZ_BG_PIXMAP_B, "p",  "EZ_BG_PIXMAP_B", "bgPixmapB", "BgPixmapB")
EZ_OptionIdNames( EZ_DRAG_HANDLE, "e",  "EZ_DRAG_HANDLE", "dragHandle", "DragHandle")
EZ_OptionIdNames( EZ_DOCKABLE, "b",  "EZ_DOCKABLE", "dockable", "Dockable")
EZ_OptionIdNames( EZ_SLIDER_STYLE, "e",   "EZ_SLIDER_STYLE", "sliderStyle", "SliderStyle")
EZ_OptionIdNames( EZ_ROW_BG,  "brr",  "EZ_ROW_BG", "rowBg", "RowBg")
EZ_OptionIdNames( EZ_ARROW_LABEL, "e",   "EZ_ARROW_LABEL", "arrowLabel", "ArrowLabel")
EZ_OptionIdNames( EZ_INTERIOR_BORDER, "i",   "EZ_INTERIOR_BORDER", "interiorBorder", "InteriorBorder")
EZ_OptionIdNames( EZ_OPTIONAL_ENTRY_STYLE, "e",  "EZ_OPTIONAL_ENTRY_STYLE", "optionalEntryStyle", "OptionalEntryStyle")
EZ_OptionIdNames( EZ_TILE_ORIGIN,  "bii", "EZ_TILE_ORIGIN", "tileOrigin", "TileOrigin")
EZ_OptionIdNames( EZ_WM_IGNORE,  "b", "EZ_WM_IGNORE", "wmIgnore", "WmIgnore")
EZ_OptionIdNames( EZ_LABEL_SHADOW, "ii",  "EZ_LABEL_SHADOW", "labelShadow", "LabelShadow")
EZ_OptionIdNames( EZ_TERM_COLOR_N, "ir.",  "EZ_TERM_COLOR_N", "termColorN", "TermColorN")
EZ_OptionIdNames( EZ_TERM_CURSOR_COLOR, "r",   "EZ_TERM_CURSOR_COLOR", "termCursorColor", "TermCursorColor")
EZ_OptionIdNames( EZ_TERM_SAVE_LINES, "i",  "EZ_TERM_SAVE_LINES", "termSaveLines", "TermSaveLines")
EZ_OptionIdNames( EZ_TERM_REVERSE_VIDEO, "b",  "EZ_TERM_REVERSE_VIDEO", "termReverseVideo", "TermReverseVideo")
EZ_OptionIdNames( EZ_HIGHLIGHT_BG, "r",  "EZ_HIGHLIGHT_BG", "highlightBackground", "HighlightBackground")
EZ_OptionIdNames( EZ_HIGHLIGHT_FG,  "r", "EZ_HIGHLIGHT_FG", "highlightForeground", "HighlightForeground")
EZ_OptionIdNames( EZ_RULER_DISPLAY_VALUE, "b",   "EZ_RULER_DISPLAY_VALUE", "rulerDisplayValue", "RulerDisplayValue")
EZ_OptionIdNames( EZ_RULER_TICK_UNIT, "i",  "EZ_RULER_TICK_UNIT", "rulerTickUnit", "RulerTickUnit")
EZ_OptionIdNames( EZ_RULER_TICK_LENGTH, "i",  "EZ_RULER_TICK_LENGTH", "rulerTickLength", "RulerTickLength")
EZ_OptionIdNames( EZ_RULER_POSITION, "e",  "EZ_RULER_POSITION", "rulerPosition", "RulerPosition")
EZ_OptionIdNames( EZ_RULER_TICK_SCALE, "i",  "EZ_RULER_TICK_SCALE", "rulerTickScale", "RulerTickScale")
EZ_OptionIdNames( EZ_RULER_TICK_OFFSET, "i",  "EZ_RULER_TICK_OFFSET", "rulerTickOffset", "RulerTickOffset")
EZ_OptionIdNames( EZ_OVERRIDE_REDIRECT, "b",  "EZ_OVERRIDE_REDIRECT", "overrideRedirect", "OverrideRedirect")
EZ_OptionIdNames( EZ_RESOURCES_HANDLER, "iph",  "EZ_RESOURCES_HANDLER", "resourcesHandler", "Resourceshandler")
EZ_OptionIdNames( EZ_SCROLL_TYPE,  "e", "EZ_SCROLL_TYPE", "scrollType", "ScrollType")
EZ_OptionIdNames( EZ_SPIN_FUNCTION, "hp",  "EZ_SPIN_FUNCTION", "spinFunction", "SpinFunction")
EZ_OptionIdNames( EZ_SPIN_VALUE,  "ip", "EZ_SPIN_VALUE", "spinValue", "SpinValue")
EZ_OptionIdNames( EZ_LOCATOR_XY,  "ii", "EZ_LOCATOR_XY", "locatorXY", "LocatorXY")
EZ_OptionIdNames( EZ_SPECIAL_EFFECT, "ii",  "EZ_SPECIAL_EFFECT", "specialEffect", "SpecialEffect")
EZ_OptionIdNames( EZ_SHOW_TICK, "ii",  "EZ_SHOW_TICK", "showTick", "ShowTick")
EZ_OptionIdNames( EZ_DIAL_RANGE, "ff",  "EZ_DIAL_RANGE", "dialRange", "DialRange")
EZ_OptionIdNames( EZ_DIAL_RESOLUTION, "f",   "EZ_DIAL_RESOLUTION", "dialResolution", "DialResolution")
EZ_OptionIdNames( EZ_DIAL_SPAN, "f",  "EZ_DIAL_SPAN", "dialSpan", "DialSpan")
EZ_OptionIdNames( EZ_DIAL_DISPLAY_VALUE, "b",   "EZ_DIAL_DISPLAY_VALUE", "dialDisplayValue", "DialDisplayValue")
EZ_OptionIdNames( EZ_DIAL_STYLE, "e",  "EZ_DIAL_STYLE", "dialStyle", "DialStyle")
EZ_OptionIdNames( EZ_DIAL_COLOR, "r",  "EZ_DIAL_COLOR", "dialColor", "DialColor")
EZ_OptionIdNames( EZ_DIAL_VALUE, "f",  "EZ_DIAL_VALUE", "dialValue", "DialValue")
EZ_OptionIdNames( EZ_CURSOR_NAME,  "C", "EZ_CURSOR_NAME", "cursorName", "CursorName")
EZ_OptionIdNames( EZ_DND_DRAG_CURSOR_NAME,"C",   "EZ_DND_DRAG_CURSOR_NAME", "dndDragCursorName", "DndDrageCursorName")

EZ_OptionIdNames( EZ_SHAPE_FILE,   "F", "EZ_SHAPE_FILE", "shapeFile", "ShapeFile")
EZ_OptionIdNames( EZ_SHAPE_PIXMAP, "p",   "EZ_SHAPE_PIXMAP", "shapePixmap", "ShapePixmap")
EZ_OptionIdNames( EZ_WRAP_TEXT,    "b",  "EZ_WRAP_TEXT", "wrapText", "WrapText")

EZ_OptionIdNames( EZ_BAR_EDITABLE,   "b", "EZ_BAR_EDITABLE", "barEditable", "BarEditable")
EZ_OptionIdNames( EZ_BAR_ORIENTATION, "e",  "EZ_BAR_ORIENTATION", "barOrientation", "BarOrientation")
EZ_OptionIdNames( EZ_BAR_COLOR_N,   "ir.",  "EZ_BAR_COLOR_N",  "barColorN", "BarColorN")
EZ_OptionIdNames( EZ_BAR_RANGE,    "ff",  "EZ_BAR_RANGE",  "barRange", "BarRange")
EZ_OptionIdNames( EZ_BAR_WIDTH,    "i",  "EZ_BAR_WIDTH",  "barWidth", "BarWidth")
EZ_OptionIdNames( EZ_HISTOGRAM_SAMPLE, "i",    "EZ_HISTOGRAM_SAMPLE",  "histogramSample", "HistogramSample")
EZ_OptionIdNames( EZ_SEPARATOR_STYLE, "e",    "EZ_SEPARATOR_STYLE",  "separatorStyle", "SeparatorStyle")
EZ_OptionIdNames( EZ_ROW_COLUMN_MAX_SIZE, "i", "EZ_ROW_COLUMN_MAX_SIZE", "rowColumnMaxSize",  "RowColumnMaxSize")
EZ_OptionIdNames( EZ_RESOURCE_STRING, "S", "EZ_RESOURCE_STRING", "resourceString",  "ResourceString")
EZ_OptionIdNames( EZ_SHEET_HEADER_SIZE, "ii", "EZ_SHEET_HEADER_SIZE", "sheetHeaderSize",  "SheetHeaderSize")
EZ_OptionIdNames( EZ_SHEET_CELL_SIZE, "ii", "EZ_SHEET_CELL_SIZE", "sheetCellSize",  "SheetCellSize")
#endif

