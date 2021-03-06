/* See LICENSE file for copyright and license details. */

#include "tcl.c"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static const int swallowfloating    = 0;
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Fira Code:pixelsize=12:antialias=true:autohint=true",
                                        "JoyPixels:pixelsize=12:antialias=true:autohint=true"};
static const char dmenufont[]       = "Fira Code:size=12";
static const char normbg[]          = "#24292e";
static const char normbd[]          = "#5190ff";
static const char normfg[]          = "#f6f8fa";
static const char selfg[]           = "#24292e";
static const char selbd[]           = "#abe885";
static const char selbg[]           = "#85e89d";
static const char titlefg[]         = "#f6f8fa";
static const char titlebd[]         = "#5190ff";
static const char titlebg[]         = "#5190ff";

static const char *colors[][3]      = {

	/*               fg         bg         border   */
    [SchemeNorm]   = { normfg, normbg, normbd },
    [SchemeSel]    = { selfg, selbg,  selbd  },
    [SchemeTitle]  = { titlefg, titlebd,  titlebg  },
};

static const Bool viewontag         = True;     /* Switch view on tag switch */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", NULL,     NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "st-256color", NULL, NULL,           0,         0,          1,           0,        -1 },
    { NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
    { "|||",      tcl },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn",
    dmenufont, "-nb", normbg, "-nf", normfg, "-sb", selbd, "-sf", selfg, NULL
};
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY,		            	XK_w,	   spawn,		   SHCMD("$BROWSER") },
    { MODKEY|ShiftMask,				XK_w,	   spawn,		   SHCMD("$TERMINAL -e iwctl") },
    { MODKEY,						XK_e,	   spawn,		   SHCMD("$TERMINAL -e neomutt") },
    { MODKEY,						XK_r,      spawn,          SHCMD("$TERMINAL -e lf") },
	{ MODKEY,                       XK_grave,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,	                	XK_Escape, spawn,          SHCMD("togglekb") },

	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, /* tile */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} }, /* 3-column */
	{ MODKEY,						XK_y,      setlayout,      {.v = &layouts[2]} }, /* monocle */
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[1]} }, /* floating */
	{ MODKEY,						XK_Tab,    view,		   {0} },

	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_o,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,				XK_o,      incnmaster,     {.i = -1 } },

	{ MODKEY,			            XK_minus,  spawn,	       SHCMD("amixer -c 0 -- sset Master playback 1dB-;") },
    { MODKEY|ShiftMask,			    XK_minus,  spawn,	       SHCMD("amixer -c 0 -- sset Master playback 5dB-;") },
    { MODKEY,			            XK_equal,  spawn,	       SHCMD("amixer -c 0 -- sset Master playback 1dB+;") },
    { MODKEY|ShiftMask,			    XK_equal,  spawn,	       SHCMD("amixer -c 0 -- sset Master playback 5dB+;") },
	{ MODKEY,						XK_m,      spawn,          SHCMD("amixer -c 0 -- sset Master playback mute") },
	{ MODKEY|ShiftMask,				XK_m,      spawn,          SHCMD("amixer -c 0 -- sset Master playback unmute && amixer -c 0 -- sset Speaker playback unmute") },

	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	{ 0,							XK_Print,  spawn,		   SHCMD("flameshot gui") },
	{ ShiftMask,                    XK_Print,  spawn,          SHCMD("scrot") },

	{ MODKEY,						XK_z,      incrgaps,	   {.i = +3 } },
	{ MODKEY,						XK_x,      incrgaps,       {.i = -3 } },
	{ MODKEY,						XK_c,      defaultgaps,    {0} },
	{ MODKEY|ShiftMask,             XK_c,      togglegaps,     {0} },
	{ MODKEY,						XK_b,      togglebar,      {0} },
    { MODKEY,						XK_n,      spawn,          SHCMD("$TERMINAL -e nvim $NOTES") },
    { MODKEY,						XK_p,      spawn,          SHCMD("$TERMINAL -e nvim $PROJECTS") },

    { MODKEY,                       XK_s,      togglesticky,   {0} },
    { MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },

    { MODKEY,                       XK_F1,     spawn,          SHCMD("telegram-desktop")},
    { MODKEY,                       XK_F2,     spawn,          SHCMD("spotify")},
    { MODKEY,                       XK_F3,     spawn,          SHCMD("discord")},
    { MODKEY,                       XK_F4,     spawn,          SHCMD("xmind-2020")},


    /*
	{ MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
    { MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
	*/
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
