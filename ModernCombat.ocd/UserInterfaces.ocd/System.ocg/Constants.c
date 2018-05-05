
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
 Width/Height of icons
 
 Given in pixel-scaled-em.
 */
static const GUI_CMC_Element_Icon_Size = 45;


/* --- Colors --- */

/* Default background for GUI elements: dark grey (38, 38, 38), 60% transparancy = 102 alpha. */
static const GUI_CMC_Background_Color_Default =   0x66262626;

/* Background for highlighted/selected GUI elements: white, 25% transparency = 64 alpha. */
static const GUI_CMC_Background_Color_Highlight = 0x40ffffff;

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

