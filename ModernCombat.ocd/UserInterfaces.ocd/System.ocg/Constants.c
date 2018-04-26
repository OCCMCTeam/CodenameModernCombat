
/* --- Margins --- */

/* Margin for left and right sides of the screen.
   Given in percent, with factor 10. */
static const GUI_CMC_Margin_Screen_H = 15;

/* Margin for top and bottom sides of the screen.
   Given in percent, with factor 10. */
static const GUI_CMC_Margin_Screen_V = 15;

/* Vertical margin between elements.
   Given in percent, with factor 10. */
static const GUI_CMC_Margin_Element_V = 15;

/* Vertical margin between elements, smaller distance.
   Given in percent, with factor 10. */
static const GUI_CMC_Margin_Element_Small_V = 10;


/* --- Element dimensions / width and height --- */

/*
 Width for info elements:
 - health bar
 - breath bar
 - weapon / item info field
 
 Width is given in percent, with factor 10;
 */
static const GUI_CMC_Element_Info_Width = 200;

/*
 Height of default element.
 
 Height is given in percent, with factor 10;
 */
static const GUI_CMC_Element_Default_Height = 40;

/*
 Height of vertically larger elements, 1.5 times
 the height of a default element.
 
 Height is given in percent, with factor 10;
 */
static const GUI_CMC_Element_Large_Height = 60;


/* --- Colors --- */

/* Default background for GUI elements: black, 30% transparancy = 77 alpha. */
static const GUI_CMC_Background_Color_Default =   0x4d000000;

/* Background for highlighted/selected GUI elements: white, 25% transparency = 64 alpha. */
static const GUI_CMC_Background_Color_Highlight = 0x40ffffff;

/* Breath bar colors */
static const GUI_CMC_Color_BreathBar_Transparent = 0x4000ccff; // Light blue, transparency 25%
static const GUI_CMC_Color_BreathBar_Opaque =      0xff00ccff; // Light blue, opaque

/* Health bar colors */
static const GUI_CMC_Color_HealthBar_Transparent = 0x40cc0000; // Red, transparency 25%
static const GUI_CMC_Color_HealthBar_Opaque =      0xffcc0000; // Red, opaque
static const GUI_CMC_Color_HealthBar_White =       0xffffffff; // White, opaque
