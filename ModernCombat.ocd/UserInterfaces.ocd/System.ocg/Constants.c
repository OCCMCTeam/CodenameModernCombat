
/* --- Margins --- */

/* Margin for left and right sides of the screen.
   Given in pixel-scaled-em. */
static const GUI_CMC_Margin_Screen_H = 10;

/* Margin for top and bottom sides of the screen.
   Given in pixel-scaled-em. */
static const GUI_CMC_Margin_Screen_V = 10;

/* Vertical margin between elements.
   Given in pixel-scaled-em. */
static const GUI_CMC_Margin_Element_V = 10;

/* Vertical margin between elements, smaller distance.
   Given in pixel-scaled-em. */
static const GUI_CMC_Margin_Element_Small_V = 5;

/* Vertical margin between status icons in the ally info.
   Given in pixel-scaled-em. */
static const GUI_Margin_StatusIcon_H = 3;


/* --- Element dimensions / width and height --- */

/*
 Width for info elements:
 - health bar
 - breath bar
 - item status
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Info_Width = 205;


/*
 Heigh for info elements:
 - item status
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Info_Height = 90;

/*
 Height of default element.
 
 Given in pixel-scaled-em. 
 */
static const GUI_CMC_Element_Default_Height = 30;

/*
 Height of default progress bar element.
 
 Given in pixel-scaled-em. 
 */
static const GUI_CMC_Element_ProgressBar_Height = 10;

/*
 Width for player info.
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Player_Width = 300;

/*
 Width for player info.
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Inventory_Width = 55;

/*
 Width/Height of icons
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Icon_Size = 45;

/*
 Width/Height of status icons in ally info.
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_StatusIcon_Size = 27;

/*
 Width/Height of list menu entry elements.
 
 Given in pixel-scaled-em. 
 */
static const GUI_CMC_Element_ListIcon_Size = 35;

/*
 Width/Height of selection list menu entry elements.
 
 Given in pixel-scaled-em. 
 */
static const GUI_CMC_Element_SelectionList_Width = 320;


/* --- Colors --- */

/* Default background for GUI elements: dark grey (38, 38, 38), 60% transparancy = 102 alpha. */
static const GUI_CMC_Background_Color_Default =   0x66262626;

/* Background for highlighted/selected GUI elements: white, 25% transparency = 64 alpha. */
static const GUI_CMC_Background_Color_Highlight = 0x40ffffff;

/* Default background for GUI elements: dark grey (38, 38, 38), 30% transparancy = 66 alpha. */
static const GUI_CMC_Background_Color_Inactive =   0x33262626;

/* Background for hovered GUI elements: gold, 50% transparency = 128 alpha. */
static const GUI_CMC_Background_Color_Hover = 0x81ffcc00;

/* Breath bar colors */
static const GUI_CMC_Color_BreathBar_Transparent = 0x4000ccff; // Light blue, transparency 25%
static const GUI_CMC_Color_BreathBar_Opaque =      0xff00ccff; // Light blue, opaque

/* Health bar colors */
static const GUI_CMC_Color_HealthBar_Transparent = 0x40cc0000; // Red, transparency 25%
static const GUI_CMC_Color_HealthBar_Opaque =      0xffcc0000; // Red, opaque
static const GUI_CMC_Color_HealthBar_White =       0xffffffff; // White, opaque

/* --- Text colors --- */

static const GUI_CMC_Text_Color_Highlight = 0xffffcc00; // Gold

static const GUI_CMC_Text_Color_Inactive = 0xff777777; // Grey

