#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include "amiga.h"
#include "../ezsdl/ezsdl.h"

#ifdef DEBUG
#define DPRINTF(...) dprintf(2, __VA_ARGS__)
#else
#define DPRINTF(...) do { } while(0)
#endif

#define SCALE 2
#define SU(x) ((x)*SCALE)
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(X[0]))

#define cursor(X,Y) cursorit(X,Y,__FUNCTION__, __LINE__)
#define Move(X,Y,Z) Moveit(X,Y,Z,__FUNCTION__, __LINE__)

#define vmode vmodeitt
#define getkb getkbitt

#define NONE 0
#define NOT_FOUND -1
#define COMMAND 1
#define STOCK 2
#define ALL -2

#define BONDVALUE 1000
#define STOCKLIMIT 9		/* max number of stocks one can hold */
#define AUTOLIMIT  9		/* max number of autos and options */

#define SPLITPRICE 200
#define GAMMA1 1.5
#define GAMMA2  .3
#define GAMMA3  .3

#define TRUE 1
#define FALSE 0
#define ESC '\033'
#define CR 13
#define BACKSP '\010'
#define BREAK '\t'
#define BEEP '\07'
#define ERASE_CH "'\08' '\08'"

#define AUTOBUY 20
#define AUTOSELL 21
#define NON_AUTO 1
#define ON 1
#define OFF 2
#define CHANGEOVERTIME 10	/* 10 seconds to complete command */
#define CLICK_CHANGE '\021'
#define NO_YRLY_MESSAGES 15
#define NO_STD_MESSAGES 30
#define YRLY 0
#define STD 1
#define GOLD 26
#define REALESTATE 25
#define MMMS 30
#define WEEKLENGTH 25
#define DIFFERENCE 48
#define STOCKSUP 1
#define AUTOSUP 2
#define RANKINGSUP 3
#define GRAPHUP 4

#define MAXPLAYERS 4
#define LOTLIMIT 9999
#define TICK_SIZE 3
#define LONG 1
#define SHORT 0


#define F10 -80		 /** for JUMPing.  Added 11/22/85 **/

#define SCREENH 240
#define SCREENW 320

static bmp4*bmp_font, *bmp_black_font;
static struct spritesheet ss_font, ss_black_font;

#define BOTTOM_START SCREENH/2
#define FONT "topaz.raw"
#define FONT_W 8
#define FONT_H 8
static void init_gfx() {
	bmp_font = bmp4_from_file(FONT);
	bmp_black_font = bmp4_from_file(FONT);
	unsigned x;
	for(x=0;x<bmp_black_font->width * bmp_black_font->height;x++)
		bmp_black_font->data[x] = bmp_black_font->data[x] == RGB(0,0,0) ? RGB(255,255,255) : RGB(0,0,0);
	if(!spritesheet_init(&ss_font, bmp_font, FONT_W, FONT_H)) dprintf(2, "oops\n");
	if(!spritesheet_init(&ss_black_font, bmp_black_font, FONT_W, FONT_H)) dprintf(2, "oops\n");
}

static int get_font_width(char letter) {
	return ss_font.sprite_w;
}

static unsigned get_font_render_length(const char* text) {
	unsigned x=0;
	for(;*text && *text != '\n';x+=get_font_width(*(text++)));
	return x;
}

static void draw_font(const char* text, struct spritesheet *font, unsigned x, unsigned y) {
	for(;*text;x+=get_font_width(*(text++))) if(*text != '\n') ezsdl_draw_sprite(font, *text, SU(x), SU(y), SCALE);
}

typedef struct stock {
	char name[4];
	float b1;
	float b2;
	float b3;
	float e;
	int price;
} ASTOCK;

typedef struct {
	int price[20];
	char splits[20];
} HISTORY;

static ASTOCK stock_array[] = {
	[ 0] = {.price = 100, .name = "KM", .b1 = 1.4, .b2 = .8, .b3 = 0, .e = .2},
	[ 1] = {.price = 100, .name = "GE", .b1 = 1.3, .b2 = .4, .b3 = -.2, .e = .3},
	[ 2] = {.price = 100, .name = "AMR", .b1 = 2.1, .b2 = .9, .b3 = .2, .e = .5},
	[ 3] = {.price = 100, .name = "ZE", .b1 = 1.7, .b2 = .0, .b3 = 0, .e = .5},
	[ 4] = {.price = 100, .name = "MYG", .b1 = 1.4, .b2 = .5, .b3 = 0, .e = .3},
	[ 5] = {.price = 100, .name = "GM", .b1 = 1.3, .b2 = .8, .b3 = .3, .e = .2},
	[ 6] = {.price = 100, .name = "SOI", .b1 = 1., .b2 = .4, .b3 = -.3, .e = .3},
	[ 7] = {.price = 100, .name = "USS", .b1 = 1.1, .b2 = .6, .b3 = 0, .e = .3},
	[ 8] = {.price = 100, .name = "ITT", .b1 = 1.6, .b2 = .0, .b3 = 0, .e = 1.2},
	[ 9] = {.price = 100, .name = "MOB", .b1 = .9, .b2 = .2, .b3 = .8, .e = .2},
	[10] = {.price = 100, .name = "PG", .b1 = .85, .b2 = .3, .b3 = 0, .e = 1.2},
	[11] = {.price = 100, .name = "EK", .b1 = 1.2, .b2 = 0., .b3 = 0, .e = .4},
	[12] = {.price = 100, .name = "AH", .b1 = 1., .b2 = 1., .b3 = .2, .e = .3},
	[13] = {.price = 100, .name = "KMB", .b1 = .7, .b2 = .1, .b3 = 0, .e = .1},
	[14] = {.price = 100, .name = "DE", .b1 = 1.3, .b2 = 1.3, .b3 = 0, .e = .4},
	[15] = {.price = 100, .name = "ED", .b1 = .65, .b2 = 1.1, .b3 = 0, .e = .1},
	[16] = {.price = 100, .name = "AA", .b1 = 1.7, .b2 = 1.2, .b3 = .1, .e = .3},
	[17] = {.price = 100, .name = "PGL", .b1 = .2, .b2 = 0., .b3 = -.4, .e = 1.5},
	[18] = {.price = 100, .name = "MD", .b1 = 2., .b2 = .2, .b3 = -.5, .e = .8},
	[19] = {.price = 100, .name = "T", .b1 = 1.2, .b2 = 0., .b3 = 0., .e = .7},
	[20] = {.price = 100, .name = "MTC", .b1 = 1., .b2 = .2, .b3 = 0, .e = .4},
	[21] = {.price = 100, .name = "UNP", .b1 = 1.7, .b2 = 1.4, .b3 = 0, .e = .9},
	[22] = {.price = 100, .name = "HM", .b1 = -.2, .b2 = .3, .b3 = -.5, .e = .8},
	[23] = {.price = 100, .name = "SMC", .b1 = 2.2, .b2 = 1.4, .b3 = 0, .e = 1.1},
	[24] = {.price = 100, .name = "NOM", .b1 = 1.3, .b2 = 1., .b3 = -.3, .e = 1.6},
	[25] = {.price = 100, .name = "RES", .b1 = .05, .b2 = .9, .b3 = 0, .e = .25},
	[26] = {.price = 100, .name = "GLD", .b1 = -.1, .b2 = 1.1, .b3 = -.3, .e = .6},
	[27] = {.price = 100, .name = "WU", .b1 = -.2, .b2 = -.4, .b3 = 0, .e = .6},
	[28] = {.price = 100, .name = "BND", .b1 = .05, .b2 = .7, .b3 = 0, .e = .0},
	[29] = {.price = 100, .name = "IBM", .b1 = .9, .b2 = .3, .b3 = 1.2, .e = .6},
	[30] = {.price = 100, .name = "IMF", .b1 = .0, .b2 = .0, .b3 = 0, .e = .0},
};

#define NO_OF_STOCKS ARRAY_SIZE(stock_array)
static HISTORY history[NO_OF_STOCKS];

static char alpha[] =
    { 16, 12, 2, 28, 14, 15, 11, 1, 26, 5, 22, 29, 8, 0, 13, 18, 9, 20, 4, 24, 10, 17, 25, 23, 6, 19, 21, 7, 27, 3, 30 };

typedef struct {
	int stock_no;
	int units;
	int price;
	float margin_debt;
} SCREEN;

static SCREEN screen[STOCKLIMIT];
static SCREEN *scr_ptr[STOCKLIMIT];
static int screen_count;

typedef struct {
	char string[82];
	float intr;
	float gnp;
	float pol;
	float var;
	char used;

} YMESS;

typedef struct {
	YMESS item[NO_YRLY_MESSAGES];
} YMESSS;

static YMESSS yrly_mssgs;

static int y_mssg_count;

typedef struct {
	char string[82];
	float intr;
	float gnp;
	float pol;
	float var;

} SMESS;

typedef struct {
	SMESS item[NO_STD_MESSAGES];
} SMESSS;

static SMESSS std_mssgs;

#define JUMP 13
#define LIMIT 12
#define EXERCISE 11
#define GRAPH 10
#define CASH 9
#define SAVE 8
#define BONDS 7
#define MARGIN 6
#define QUIT 5
#define DELETE 4
#define PUT 3
#define CALL 2
#define SELL 1
#define BUY 0
static const char commands[][9] = {
	"BUY",
	"SELL",
	"CALL",
	"PUT",
	"DELETE",
	"QUIT",
	"MARGIN",
	"T-BILLS",
	"SAVE",
	"CASH",
	"GRAPH",
	"EXERCISE",
	"LIMIT",
	"JUMP"
};

static float ndis[24] = {
	.5,
	.53983,
	.57926,
	.61791,
	.65542,
	.69146,
	.72575,
	.75804,
	.78814,
	.81594,
	.84134,
	.86433,
	.88493,
	.90320,
	.91924,
	.93319,
	.94520,
	.95543,
	.96407,
	.97128,
	.97725,
	.98214,
	.98610,
	.98928
};

typedef struct tick_item {
	char item[11];
	struct tick_item *next;
	struct tick_item *previous;
} TICK_ITEM;

static TICK_ITEM *cur_tick;
static TICK_ITEM ticker[TICK_SIZE];

static int load_stock = TICK_SIZE;	/* the number of the next stock to be loaded into
					   the ticker. */

static char newsline[82];	/* holds current news message */

static int cur_news_line = -1;	/* the number of the last position in the newline
				   array to be put on screen */
typedef struct autos {
	int minus_price;	/* price to exercise auto buy */
	int plus_price;		/* price to exercise auto sell */
	int option_type;	/* CALL or PUT option?  */
	int option_price;	/* price option is exerciseable at */
	int units;		/* number of shares */
	int stock_no;		/* stock id number */
	int cur_price;		/* the last price posted */
	int pur_price;		/* how much the option cost per lot */
} AUTOS;


typedef struct node {
	int year;
	int week;
	int shares;
	struct node *next;
	struct node *previous;
	int price;
} NODE;

typedef struct {
	int shares;
	int limit;
	float margin_debt;
	NODE *purchases;
} STOCKS;

typedef struct player {
	double cash;
	double net_worth;
	STOCKS portfolio[NO_OF_STOCKS];
	int stock_count;
	int auto_count;
	AUTOS *auto_ptr[AUTOLIMIT];	/* pointers to the auto exec structures */
	AUTOS auto_array[AUTOLIMIT];	/* the actual auto exec structures */
	char name[21];
	int name_length;	/* characters in the person's name */
	double taxes;
	float short_term;
	float long_term;
	float other_earnings;
	int bonds;

} PLAYER;

static PLAYER players[MAXPLAYERS];
static PLAYER *cur_player;

typedef struct {
	int status;		/* 1 if ON 2 if OFF */
	int count;		/* seconds since hour when initialized */
	int hour_changed;	/* flag set if timer approaching hour change */
} TIMER;

static TIMER timer1;		/* for game time timing */
static TIMER timer2;		/* for changing player timing */
static TIMER timer3;		/* for event gap timing */

static char message;		/* true if program message displayed */
static char scr_status = STOCKSUP;	/* either stocks,autos,or rankings are up */
static char in_progress;	/* in the middle of a news event ? */
static char mess_type;		/* yearly or standard message in use */
static char q_break;		/* true at end of quarter */
static int no_of_players;	/* how many players we have, set at beginning */
static int player = 1;		/* the current player */
static int tmp_player;		/* holds the new player while we wait */
static float prime_rate = .5;	/* actually will be read in initially */
static float high_prime;	/* highest prime rate allowed */
static float low_prime;		/* lowest prime rate allowed */
static float q_int_rate = .049;	/* the quarterly interest rate */
static float wk_int_rate = .001;	/* quarterly divided by 13 */
static float bond_rate;		/* quarterly rate at beginning of quarter */
static int quarter = 1;		/* what the current quarter is */
static int week = 1;		/* the current week of the year */
static int year = 1938;		/* actually will be read in initially */
static char stop;		/* flag to indicate end of quarter break */
static int com_char_count;	/* how many chars in the buffer */
static char delay;		/* scroll another news char or not */
static char year_over;		/* paused at end of year ?  */
static int event_week = 1;	/* the week of the last event */
static char com_array[30];	/* holds user input */
static int time_count;		/* 0 to 3600 replaces real time */
static int make_call;		/* do a margin call at end of week ? */

static float factor1, factor2, factor3, factor4;
static float x1, x2, x3, instd1, instd2, instd3, instd4, the_interest, gnp, pol, intr, var;
static int weeks;

static int jump_weeks;		/* number of weeks left in JUMP */
				   /* added: 11/22/85              */

static char newsch;		/* the next newsline char to scroll */
static int units;		/* how many to buy,sell,etc. */
static int com_no;		/* the digit corres. to the command */
static int stock_no;		/* the number corres. to the stock */
static int trans_price;		/* the price at which the transaction
				   is to be executed */
static int autobuy;		/* the price to execute an auto buy */
static int autosell;		/* the price to execute an auto sell */
static int tick_position;	/* the position in the current ticker
				   element */
static int click_on;		/* flag to turn on clicking */
static int time;		/* current time seconds + minutes */
static int messno;		/* message selected */
static int loop_to;		/* when to end the event */
static int half_way = -2;	/* half thru reset message */
static int g;			/* junk */
static char emphasis;

static float vx1, vx2, vx3, vx4;	/* variances for option use */
//input wave for audio ticker
//static signed char si[] = {0,49,90,117,127,117,90,49,0,-49,-90,-117,-127,-117,-90,-49};

static struct RastPort *rp;

static void save();
static void quit(void);
static void graph(int stockno, int year);
static void bonds(int units);
static void add_auto_exec(int auto_minus, int auto_plus, int option, int opt_price, int units, int stockno, int player,
			  int pur_price);
static void write_auto(int position);
static float opt_value(int player, int position);
static void del_auto(int position, int player);
static void upd_coh(void);
static void upd_netw(void);
static void upd_bonds(void);
static void upd_quarter(void);
static void upd_year(void);
static void upd_prime(void);
static void upd_stock(int stockno);
static void write_stock(int line_inc, int element);
static void clear_stock(int line_inc);
static void load_screen(void);
static void screen_upd(void);
static double calc_netw(int player);

#define STUB(X) static long X () { dprintf(2, "warning: %s unimplemented\n", __FUNCTION__); return 0;}

/* clib/dos_protos.h */
static void Delay( long timeout ) {
	ezsdl_refresh();
	ezsdl_sleep(timeout*10);
}
/* clib/intuition_protos.h */
    STUB(DisplayBeep)
//void DisplayBeep( struct Screen *screen );
/* clib/graphics_protos.h */
    STUB(SetDrMd)
//void SetDrMd( struct RastPort *rp, unsigned long drawMode );
static void Moveit( struct RastPort *rp, long x, long y, const char*fn, int line ) {
#ifdef DEBUG_CALLS
	dprintf(2, "%s called from %s:%d\n", __FUNCTION__, fn, line);
#endif
	assert(x<SCREENW);
	assert(y<SCREENH);
	rp->custom.x = x;
	rp->custom.y = y;
}
#define MIN(a,b) ((a)<(b)) ? (a) : (b)
#define ABS(a) ((a) < 0) ? -(a) : (a)

/* Set the primary drawing pen for lines, fills, and text. */
static void SetAPen( struct RastPort *rp, unsigned long pen ) {
	rp->FgPen = pen;
}
/* Set the secondary drawing pen for lines, fills, and text. */
static void SetBPen( struct RastPort *rp, unsigned long pen ) {
	rp->BgPen = pen;
}
static unsigned GetAPen(struct RastPort *rp) {
	return rp->FgPen;
}
static unsigned GetBPen(struct RastPort *rp) {
	return rp->BgPen;
}
static unsigned GetPenColor(struct RastPort *rp, unsigned pen) {
	assert(pen < sizeof(rp->custom.pencolors) / sizeof (rp->custom.pencolors[0]));
	return rp->custom.pencolors[pen];
}
/* original uses ViewPort instead of RastPort */
static void SetRGB4( struct RastPort *vp, long index, unsigned long red,  unsigned long green, unsigned long blue ) {
	assert(index < sizeof(rp->custom.pencolors) / sizeof (rp->custom.pencolors[0]));
	rp->custom.pencolors[index] = RGB(red*17, green*17, blue*17);
}
void RectFill( struct RastPort *rp, long xMin, long yMin, long xMax, long yMax ) {
	ezsdl_fill_rect(SU(xMin), SU(yMin), xMax - xMin, yMax-yMin, GetPenColor(rp, GetAPen(rp)), SCALE);
}

// draw a line and move there
static void Draw( struct RastPort *rp, long x, long y ) {
	//only implement straight lines
	assert(x == rp->custom.x || y == rp->custom.y);

	if(y != rp->custom.y)
		ezsdl_draw_vline(SU(rp->custom.x), SU(MIN(y,rp->custom.y)), ABS(y-rp->custom.y), GetPenColor(rp, GetAPen(rp)), SCALE);
	else
		ezsdl_draw_hline(SU(MIN(x,rp->custom.x)), SU(rp->custom.y), ABS(x-rp->custom.x), GetPenColor(rp, GetAPen(rp)), SCALE);

	assert(x<SCREENW);
	assert(y<SCREENH);

	rp->custom.x = x;
	rp->custom.y = y;
	ezsdl_refresh();
}

long Text( struct RastPort *rp, STRPTR string, unsigned long count ) {
	char buf[80];
	snprintf(buf, sizeof buf, "%*s", (int) count, string);
	unsigned rl = get_font_render_length(buf);
	assert(rp->custom.x+rl<=SCREENW);
	ezsdl_fill_rect(SU(rp->custom.x), SU(rp->custom.y), rl, FONT_H, GetPenColor(rp, GetBPen(rp)), SCALE);
	struct spritesheet *font = GetPenColor(rp, GetAPen(rp)) == RGB(0,0,0) ? &ss_black_font : &ss_font;
	draw_font(buf, font, rp->custom.x, rp->custom.y);
	rp->custom.x+=rl;
	ezsdl_refresh();
	return 0;
}
    STUB(calc_taxes)

/* supposed to clear either the actual line or the line in which commands can be entered */
static void clear_line(void) {
	ezsdl_fill_rect(0, SU(rp->custom.y), SCREENW, FONT_H, RGB(0,0,0), SCALE);
	rp->custom.x = 0;
}

static void clear_top(void) {
	ezsdl_fill_rect(0, 0, SCREENW, BOTTOM_START, RGB(0,0,0), SCALE);
	ezsdl_refresh();
}
static void clear_bottom(void) {
	ezsdl_fill_rect(0, SU(BOTTOM_START), SCREENW, SCREENH-BOTTOM_START, RGB(0,0,0), SCALE);
	ezsdl_refresh();
}

    STUB(display_rankings)
    STUB(earnings_display)
    STUB(click)

static size_t word(char* s, int*end) {
	size_t c = 0;
	while(s[c] && !isspace(s[c])) c++;
	*end = s[c] == 0;
	s[c] = 0;
	return c;
}
static char* skip_whitespace(char *s, int*end) {
	while(*s && isspace(*s)) s++;
	*end = !*s;
	return s;
}

static int find_stock(char* sym) {
	unsigned i;
	for(i=0;i<ARRAY_SIZE(stock_array);i++)
		if(!strcmp(sym, stock_array[i].name)) return i;
	return -1;
}

static void TxWrite(struct RastPort *rp, const char *ch);

static void TxWritef(struct RastPort *rp, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char buf[100];
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	TxWrite(rp, buf);
}

static int TxWriteNoMove(struct RastPort *rp, const char *s) {
	int x = rp->custom.x;
	TxWrite(rp, s);
	rp->custom.x = x;
	return FALSE;
}

static int TxWriteMsg(struct RastPort *rp, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char buf[100];
	vsnprintf(buf, sizeof buf, fmt, ap);
	va_end(ap);
	message = TRUE;
	return TxWriteNoMove(rp, buf);
}

//validity_check: supposed to check whether a user-entered command can be executed
// translates the command number
// prints "INVALID COMMAND WORD" if command is not known
// prints  "YOU HAVEN'T GOT ENOUGH CASH" if not enough money
// returns TRUE or FALSE
// calls clear_line
static int validity_check(char *cmd, int *com_char_count,
                          int *com_no, int *units, int *stock_no,
                          int *trans_price, int *autobuy, int *autosell) {
	// for weird reasons, the first char is always 0
	cmd++;
	clear_line();
	if(!*com_char_count) return FALSE;
	*com_char_count = 0;
	unsigned i,l;
	int end, si;
	l = word(cmd, &end);

	for(i=0;i<ARRAY_SIZE(commands);i++) if(!strcmp(cmd, commands[i])) goto found;
	return TxWriteMsg(rp, "INVALID COMMAND WORD");
	found:
	*com_no = i;
	if(!end) {
		cmd+=l+1;
		cmd=skip_whitespace(cmd, &end);
	}
	switch(*com_no) {
		case GRAPH:
			if(!q_break) goto end_qu;
			if(end) goto incompl;
			if((si = find_stock(cmd)) == -1) goto inv_stock;
			*stock_no = si;
		case SAVE:
		case QUIT:
			return TRUE;
		case SELL:
		case BUY:
			if(end) goto incompl;
			if(isdigit(*cmd)) {
				l = word(cmd, &end);
				if(end) goto incompl;
				*units = atoi(cmd);
				cmd += l+1;
				cmd = skip_whitespace(cmd, &end);
				if(end) goto incompl;
			} else *units = 1;
			if((si = find_stock(cmd)) == -1) goto inv_stock;
			*stock_no = si;
			*trans_price = stock_array[si].price * *units;
			return TRUE;
	}
	return TxWriteMsg(rp, "UNHANDLED COMMAND GIVEN");
	incompl:
	return TxWriteMsg(rp, "INCOMPLETE COMMAND GIVEN");
	inv_stock:
	return TxWriteMsg(rp, "INVALID STOCK NAME GIVEN");
	end_qu:
	return TxWriteMsg(rp, "DATA AVAILABLE AT END OF QUARTER ONLY");
}

    STUB(change_player)
    STUB(add_purchase)
    STUB(del_purchase)

static double sqr(double x) {
	return (x * x);
}

static void TxWrite(struct RastPort *rp, const char *ch) {
	Text(rp, ch, strlen(ch));
}


/* expecting a scrollbuf of 41 chars */
static void scroll(char* scrollbuf, char u, int reset, int y, int bgpen) {
	if(reset) memset(scrollbuf, ' ', sizeof scrollbuf-1);
	scrollbuf[41-1] = 0;
	memmove(scrollbuf, scrollbuf+1, 41-1);
	scrollbuf[41-2] = u;
	unsigned sx = rp->custom.x, sy = rp->custom.y;
	SetAPen(rp, bgpen);
	RectFill(rp, 0, y, 320, y+23);
	Move(rp, 0, y+8);
	SetBPen(rp, bgpen);
	SetAPen(rp, 0);
	TxWrite(rp, scrollbuf);
	Move(rp, sx, sy);
	SetAPen(rp, 1);
	SetBPen(rp, 0);
}

/* scroll news */
static void scrollne(char u) {
	static char scrollbuf[40+1];
	scroll(scrollbuf, u, cur_news_line == -1, 32, 3);
}

static void scrollit(char u) {
	static char scrollbuf[40+1];
	static int initdone;
	scroll(scrollbuf, u, !initdone, 32-24, 2);
	initdone = 1;
}

static void load_tick_element(TICK_ITEM* t, int stockno) {
	int l = snprintf(t->item, sizeof(t->item), "%s-%d   ", stock_array[stockno].name, stock_array[stockno].price);
	//memset(t->item+l, ' ', sizeof(t->item)-l);
	//t->item[sizeof(t->item)-1]=0;
}

static long long timeval2utime(struct timeval *t) {
	return (t->tv_sec * 1000LL * 1000LL) + t->tv_usec;
}

static long long getutime64(void) {
	struct timeval t;
	gettimeofday(&t, 0);
	return timeval2utime(&t);
}

static int getmicros() {
//   ULONG Seconds, Micros;
//   CurrentTime(&Seconds,&Micros);
//   return((int) (Micros % 100) + 1);
	return getutime64() % 100 + 1;
}


static int gethundredths() {
//  ULONG Seconds, Micros;
//  CurrentTime(&Seconds,&Micros);
//  return((int)Micros/10000);
	return getutime64() / 10000;
}

#if 1
float frand() {
	return ((float) getmicros() / 101.0);
}
#else
float frand() {
	return (float) rand() / RAND_MAX;
}
#endif

static float pi2 = 6.28;
static float myrandom(float stdev) {
	float holder;
	holder = (sqrt(-2.0 * log(frand())) * (cos(pi2 * frand()))) * stdev;
	return (holder);
}

static void cursorit(int y, int x, const char*fn, int line) {
#ifdef DEBUG_CALLS
	dprintf(2, "%s called from %s:%d\n", __FUNCTION__, fn, line);
#endif
	//Move(rp, x * 8, y * 8 + 6);
	Move(rp, x*8, y*8);
}

static float get_float(FILE * infile) {
	char innum[20], c;
	int i;

	while(((c = getc(infile)) == ' ') || (c == '\015') || (c == '\012')) ;
	innum[0] = c;
	i = 1;
	while(((c = getc(infile)) != ' ') && (c != '\015') && (c != '\012'))
		innum[i++] = c;
	innum[i] = '\0';
	return ((float) atof(innum));
}

static int get_int(FILE * infile) {
	char innum[10], c;
	int i;

	while(((c = getc(infile)) == ' ') || (c == '\015') || (c == '\012')) ;
	innum[0] = c;
	i = 1;
	while(((c = getc(infile)) != ' ') && (c != '\015') && (c != '\012'))
		innum[i++] = c;
	innum[i] = '\0';
	return (atoi(innum));
}

static float normal(float value) {
	int i, flag;
	float holder;

	flag = FALSE;
	value *= 10;
	if(value < 0) {
		value *= -1;
		flag = TRUE;
	}
	i = (int) value;
	if(i > 22)
		holder = .999;
	else
		holder = ((value - ((float) i)) * ndis[i + 1]) - ((value - ((float) (i + 1))) * ndis[i]);
	if(flag)
		holder = 1.0 - holder;

	DPRINTF("value is %f i is %d\n",value,i);
	DPRINTF("normal returns %f\n",holder);

	return (holder);
}

float variance(int stockno) {
	float tvx1, tvx2, tvx3, tvx4;
	tvx1 = vx1 + factor1;
	tvx2 = vx2 + factor2;
	tvx3 = vx3 + factor3;
	tvx4 = vx4 + factor4;
	DPRINTF("ran %f q_int %f\n",myrandom(2.0 * q_int_rate),q_int_rate);
	DPRINTF("4 tvx1 %f tvx2 %f tvx3 %f tvx4 %f\n",tvx1,tvx2,tvx3,tvx4);
	DPRINTF("st %d b1 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b1,sqr(stock_array[stockno].b1),
	         (sqr(stock_array[stockno].b1) * tvx1));
	DPRINTF("st %d b2 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b2,sqr(stock_array[stockno].b2),
	         (sqr(stock_array[stockno].b2) * tvx2));
	DPRINTF("st %d b3 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b3,sqr(stock_array[stockno].b3),
        	 (sqr(stock_array[stockno].b3) * tvx3));
	DPRINTF("st %d e %f sqr %f sq* %f\n",stockno,stock_array[stockno].e,sqr(stock_array[stockno].e),
        	 (sqr(stock_array[stockno].e) * tvx4));

	return (((sqr(stock_array[stockno].b1) * tvx1) +
		 (sqr(stock_array[stockno].b2) * tvx2) +
		 (sqr(stock_array[stockno].b3) * tvx3) + (sqr(stock_array[stockno].e) * tvx4)) * 10.0);
}

static int kbhit() {
	enum eventtypes e;
	struct event myevent;
	e = ezsdl_getevent(&myevent);
	if(e != EV_KEYDOWN) return 0;
	else return myevent.which;
}

static void end_year(void) {
	int player, counter;
	PLAYER *play_ptr;

	clear_bottom();
	scr_status = RANKINGSUP;
	cursor(8, 13);
	TxWrite(rp, "Taxes Assessed");
	cursor(9, 13);
	TxWrite(rp, "______________");

	for(player = 0; player < no_of_players; ++player) {
		play_ptr = &players[0] + player;
		for(counter = 0; counter < play_ptr->auto_count; ++counter) {
			if((play_ptr->auto_ptr[counter]->option_type == CALL) ||
			   (play_ptr->auto_ptr[counter]->option_type == PUT)) {
				players[player].cash += (opt_value(player + 1, counter) * .985);
				players[player].other_earnings += ((opt_value(player + 1, counter) * .985) -
								   players[player].auto_ptr[counter]->pur_price);
				del_auto(counter, player + 1);
				--counter;
			}
		}		/* end for counter */


		play_ptr->taxes += calc_taxes(player);

		cursor(11 + player, 0);
		TxWritef(rp, "%20s  $%-10.0f", play_ptr->name, play_ptr->taxes);
		if(play_ptr->taxes < 0) {
			cursor(11 + player, 37);
			TxWrite(rp, "CR");
		} else if(play_ptr->taxes <= play_ptr->cash) {
			play_ptr->cash -= play_ptr->taxes;
			play_ptr->taxes = 0;
		} else {
			play_ptr->taxes -= play_ptr->cash;
			play_ptr->cash = 0;
		}
		play_ptr->short_term = play_ptr->long_term = play_ptr->other_earnings = 0;
	}			/* end for players */

	++year;
	week = 1;
	upd_year();
	year_over = TRUE;
	clear_line();
	TxWriteMsg(rp, "END OF YEAR"); // changed from TxWrite
}

static void execute(int com_no, int units, int stock_no, int price, int auto_minus, int auto_plus, int player) {
	AUTOS *ptr;
	int counter;

	switch (com_no) {
		case BUY:
			if(price == NOT_FOUND) {
				if(players[player - 1].auto_count < AUTOLIMIT) {
					add_auto_exec(auto_minus, auto_plus, AUTOBUY, NONE, units, stock_no, player,
						      NOT_FOUND);
				} else {
					TxWriteMsg(rp, "YOU ALREADY HAVE %d AUTO TRANSACTIONS", AUTOLIMIT);
				}

			} else if(round(((float) units) * ((float) price) * 101.5) <= players[player - 1].cash) {
				if((players[player - 1].portfolio[stock_no].shares != 0) ||
				   (players[player - 1].stock_count < STOCKLIMIT)) {
					/* add a record node for tax purposes */

					add_purchase(player, stock_no, units, price);

					/* if its a new stock, note that his portfolio has grown */

					if(players[player - 1].portfolio[stock_no].shares == 0)
						++players[player - 1].stock_count;

					/* charge the player */

					players[player - 1].cash -= round(((float) units) * ((float) price) * 101.5);

					/* give him the stock */

					players[player - 1].portfolio[stock_no].shares += units;

					upd_coh();
					upd_stock(stock_no);

				} else {	/* reached limit of holdings */
					TxWriteMsg(rp, "YOU ALREADY HAVE %d STOCKS", STOCKLIMIT);
				}
			}
			/* end if enough cash */
			else {	/* too little cash */
				TxWriteMsg(rp, "YOU HAVEN'T GOT ENOUGH CASH");
			}

			break;

		case SELL:
			if(price == NOT_FOUND) {
				if(players[player - 1].auto_count < AUTOLIMIT) {
					add_auto_exec(auto_minus, auto_plus, AUTOSELL, NONE, units, stock_no, player, NOT_FOUND);
				} else {
					TxWriteMsg(rp, "YOU ALREADY HAVE %d AUTO TRANSACTIONS", AUTOLIMIT);
				}
			} else if((players[player - 1].portfolio[stock_no].shares >= units) && (units > 0)) {

				/* pay the player for the stock he sold, first deducting */
				/* any margin debt owed on those shares.                 */

				if(round((double) players[player - 1].portfolio[stock_no].margin_debt) >=
				   round(((float) units) * ((float) stock_array[stock_no].price * 98.5)))
					players[player - 1].portfolio[stock_no].margin_debt -=
					    round(98.5 * ((float) units) * ((float) stock_array[stock_no].price));
				else {
					players[player - 1].cash +=
					    round(((float) units) * ((float) stock_array[stock_no].price) * 98.5) -
					    round((double) players[player - 1].portfolio[stock_no].margin_debt);
					players[player - 1].portfolio[stock_no].margin_debt = 0;
				}

				if(round((double) players[player - 1].portfolio[stock_no].margin_debt) < 1)
					players[player - 1].portfolio[stock_no].margin_debt = 0;

				/* remove or update the tax record node */

				del_purchase(player, stock_no, units);

				/* remove the shares from his portfolio */

				players[player - 1].portfolio[stock_no].shares -= units;

				/* if he sold all his shares note his fewer holdings */

				if((players[player - 1].portfolio[stock_no].shares == 0) &&
				   (players[player - 1].portfolio[stock_no].margin_debt == 0)) {
					players[player - 1].portfolio[stock_no].limit = 0;
					--players[player - 1].stock_count;
				}

				upd_coh();
				upd_stock(stock_no);
			} else {	/* if he doesn't have that many shares */
				if(players[player - 1].portfolio[stock_no].shares == 0)
					TxWriteMsg(rp, "YOU DON'T OWN ANY!");
				else
					TxWriteMsg(rp, "YOU DON'T HAVE THAT MANY SHARES");
			}
			break;
		case CALL:
			if(round(((float) price) * ((float) units) * 1.015) <= players[player - 1].cash) {
				if(players[player - 1].auto_count < AUTOLIMIT) {
					add_auto_exec(NONE, NONE, CALL, stock_array[stock_no].price, units, stock_no,
						      player, price);
					players[player - 1].cash -= round(((float) price) * ((float) units) * 1.015);
					upd_coh();
					upd_netw();

				} else {
					TxWriteMsg(rp, "YOU ALREADY HAVE  %d AUTO TRANSACTIONS", AUTOLIMIT);
				}
			} else {	/* if he hasn't got the money */
				TxWriteMsg(rp, "YOU NEED SOME MORE CASH");
			}
			break;
		case PUT:
			if(round(((float) price) * ((float) units) * 1.015) <= players[player - 1].cash) {
				if(players[player - 1].auto_count < AUTOLIMIT) {
					add_auto_exec(NONE, NONE, PUT, stock_array[stock_no].price, units, stock_no,
						      player, price);
					players[player - 1].cash -= round(((float) price) * ((float) units) * 1.015);
					upd_coh();
					upd_netw();

				} else {
					TxWriteMsg(rp, "YOU ALREADY HAVE  %d AUTO TRANSACTIONS", AUTOLIMIT);
				}
			} else {	/* if he hasn't got the money */
				TxWriteMsg(rp, "YOU NEED SOME MORE CASH");
			}
			break;

		case MARGIN:
			if(round((((float) units) * ((float) price) * 103.0) / 2.0) <= players[player - 1].cash) {
				if((players[player - 1].portfolio[stock_no].shares != 0) ||
				   (players[player - 1].stock_count < STOCKLIMIT)) {

					/* update or add a tax record node */

					add_purchase(player, stock_no, units, price);

					/* if its a new stock, note that his portfolio has grown */

					if(players[player - 1].portfolio[stock_no].shares == 0)
						++players[player - 1].stock_count;

					/* charge the player half the cost */

					players[player - 1].cash -=
					    round((((float) units) * ((float) price) * 103.0) / 2.0);

					/* put the other half on margin */

					players[player - 1].portfolio[stock_no].margin_debt +=
					    round((100.0 * ((float) units) * ((float) price)) / 2.0);

					/* give him the stock */

					players[player - 1].portfolio[stock_no].shares += ((int) units);

					upd_coh();
					upd_stock(stock_no);

				} else {	/* reached limit of holdings */
					TxWriteMsg(rp, "YOU ALREADY HAVE %d STOCKS", STOCKLIMIT);
				}

			}
			/* end if enough cash */
			else {	/* too little cash */
				TxWriteMsg(rp, "YOU HAVEN'T GOT ENOUGH CASH");
			}

			break;

		case DELETE:
			del_auto(units - 1, player);
			break;

		case QUIT:
			quit();
			break;

		case BONDS:
			bonds(units);
			break;

		case SAVE:
			save();
			break;

		case CASH:
			cur_player->cash += ((float) units) * 1000.0;
			cur_player->bonds -= units;
			upd_coh();
			upd_bonds();
			break;
		case GRAPH:
			graph(stock_no, year);
			if(newsline[cur_news_line] != '\n')
				cur_news_line = 0;
			break;

		case EXERCISE:
			ptr = cur_player->auto_ptr[units - 1];
			if((ptr->option_type == CALL) || (ptr->option_type == PUT)) {
				price = (int) round(.985 * ((float) price));
				cur_player->cash += (((float) price) * ((float) ptr->units));
				cur_player->other_earnings += ((((float) price) - ((float) ptr->pur_price)) *
							       ((float) ptr->units));
				del_auto(units - 1, player);
				upd_coh();
			} else {
				TxWriteMsg(rp, "You Can't Exercise a Limit Order");
			}
			break;

		case LIMIT:
			cur_player->portfolio[stock_no].limit = units;
			for(counter = 0; counter < screen_count; ++counter)
				if((scr_status == STOCKSUP) && (scr_ptr[counter]->stock_no == stock_no)) {
					cursor(12 + counter, 36);
					if(units != 0) {
						TxWritef(rp, "%3d", units);
					} else {
						TxWrite(rp, "   ");
					}
				}
			break;

		case JUMP:
			jump_weeks = units;
			break;

	}
	calc_netw(player);
	if((scr_status == STOCKSUP) || (scr_status == AUTOSUP))
		upd_netw();
}

static void shutdown() {
	Delay(300);
	clear_line();
	TxWrite(rp, "Hit any key to continue");
	while(kbhit() == 0) ezsdl_sleep(20);
	exit(TRUE);
}

static void quit(void) {
	int tplayer;
	int counter;
	PLAYER *play_ptr;

	for(tplayer = 0; tplayer < no_of_players; ++tplayer) {
		play_ptr = &players[0] + tplayer;
		for(counter = 0; counter < NO_OF_STOCKS; ++counter) {
			if(play_ptr->portfolio[counter].shares > 0)
				execute(SELL, play_ptr->portfolio[counter].shares, counter, stock_array[counter].price,
					NONE, NONE, tplayer + 1);
		}
		for(counter = 0; counter < play_ptr->auto_count; ++counter) {
			if((play_ptr->auto_ptr[counter]->option_type == CALL) ||
			   (play_ptr->auto_ptr[counter]->option_type == PUT)) {
				play_ptr->cash += opt_value(tplayer + 1, counter);
				del_auto(counter, tplayer + 1);
			}
		}
	}
	display_rankings();
	clear_line();
	TxWrite(rp, "All Holdings Liquidated");
	TxWrite(rp, " That's It!");
	shutdown();
}

static void set_graph() {
	int i;
	/* Set up graph initialization */
	SetAPen(rp, 5);
	Move(rp, 280, 176);
	Draw(rp, 116, 176);
	Draw(rp, 116, 16);
	SetAPen(rp, 1);
	for(i = 0; i < 5; ++i) {
		Move(rp, 124 + (i * 32), 176);
		Draw(rp, 124 + (i * 32), 183);
	}
	SetAPen(rp, 0);
	SetBPen(rp, 5);
	cursor(5, 4);
	Text(rp, "X", 1);
	cursor(6, 4);
	Text(rp, "X", 1);
	cursor(7, 4);
	Text(rp, "X", 1);
	SetAPen(rp, 1);
	SetBPen(rp, 0);
	cursor(2, 10);
	TxWrite(rp, "200-");
	cursor(7, 10);
	TxWrite(rp, "150-");
	cursor(12, 10);
	TxWrite(rp, "100-");
	cursor(17, 11);
	TxWrite(rp, "50-");
	cursor(21, 11);
	TxWrite(rp, "10-");
}




/***************************************************/
/* Draws a line up from the bottom of the graph    */
/* starting at horizontal position, POSITION, and  */
/* continuing NUMBER of times in the COLOR.        */
/*                                                 */
/* line_draw(position,number,color)                */
/* int position;                                   */
/* int number;                                     */
/* int color;                                      */
/***************************************************/


static void line_draw(int position, int number, int color) {
	if((number > 0) && (number <= 200)) {
		SetAPen(rp, color);
		RectFill(rp, 112 + (position * 8), 175 - number, 119 + (position * 8), 175);
		SetAPen(rp, 1);
	}
}

/*

#asm
          public reset_it
reset_it  proc near
          push bp
          mov  bp,sp
          mov  bx,4[bp]
          mov  ax,6[bp]
   lbp11: cmp  ax,9
          jl   lbp12
          sub  ax,8
          inc  bx
          jmp  lbp11
   lbp12: dec  ax
          mov  cx,ax
          mov  dl, byte ptr [bx]
          cmp  cl,1
          jl   lbp13
          ror  dl,cl
   lbp13: and  dl,feh
          cmp  cl,1
          jl   lbp14
          mov  cx,ax
          rol  dl,cl
   lbp14: mov byte ptr [bx],dl
          pop bp
          ret
reset_it endp
#endasm

*/

#define RED 2
#define GREEN 4
#define BLUE 5

static void graph(int stockno, int year) {
	int x;
	int color, tcolor;

	clear_top();
	clear_bottom();
	set_graph();
	year -= 4;
	color = RED;
	tcolor = NOT_FOUND;

	for(x = 0; x < 20; ++x) {

		if(history[stockno].splits[x]) {
			tcolor = color;
			color = BLUE;
		}

		line_draw(x + 1, (int) round((((float) history[stockno].price[x]) * 0.8) + .49), color);
		SetDrMd(rp, JAM1);
		if(history[MMMS].price[x] != 0) {
			cursor((int)
			       (round((((double) 21.51) - (((double) history[MMMS].price[x])) / ((double) 10.0)))),
			       15 + x);
			TxWrite(rp, "*");
		}
		SetDrMd(rp, JAM2);
		if(tcolor != NOT_FOUND) {
			if(color == RED)
				color = GREEN;
			else
				color = RED;
			tcolor = NOT_FOUND;
		}
		if(color == RED)
			color = GREEN;
		else
			color = RED;
		if((x % 4) == 0) {
			cursor(23, 15 + x);
			if(year > 1900)
				TxWritef(rp, "%2d", year - 1900);
			else
				TxWritef(rp, "%2d", year);
			++year;
		}
	}
	cursor(2, 32);
	TxWrite(rp, stock_array[stockno].name);
	cursor(1, 1);
	TxWrite(rp, "Stock");
	cursor(3, 1);
	TxWrite(rp, "Splits");
	scr_status = GRAPHUP;
}

#define OPTIONS 1
#define TAXES 2

static void margin() {
	int player;
	int counter;
	static char owes[15];
	int found, passby;
	float dif, y;
	PLAYER *ptr;

	make_call = FALSE;
	owes[0] = 0;
	found = FALSE;
	passby = FALSE;

	for(player = 0; player < no_of_players; ++player) {
		ptr = &players[0] + player;

		for(counter = 0; counter < NO_OF_STOCKS; ++counter) {
			if((round((double) ptr->portfolio[counter].margin_debt) >
			    (dif = (round((((float) ptr->portfolio[counter].shares) * 100.0 *
					   ((float) stock_array[counter].price)) / 2.0))))) {
				dif = round((double) ptr->portfolio[counter].margin_debt) - dif;
				if(ptr->cash >= dif) {
					ptr->cash -= dif;
					ptr->portfolio[counter].margin_debt -= dif;
					passby = TRUE;
				} else {
					ptr->portfolio[counter].margin_debt -= ptr->cash;
					ptr->cash = 0;
					timer1.status = OFF;
					stop = TRUE;
					make_call = TRUE;
					owes[player] = 1;
					found = OPTIONS;
				}
				if(round((double) ptr->portfolio[counter].margin_debt) < 1.0) {
					ptr->portfolio[counter].margin_debt = 0;
					if(ptr->portfolio[counter].shares == 0)
						--ptr->stock_count;
				}
				if(cur_player == ptr) {
					upd_stock(counter);
					upd_coh();
				}
			}
		}

		if(ptr->taxes > 0)
			if(ptr->cash < ptr->taxes) {

				ptr->taxes -= ptr->cash;
				ptr->cash = 0;
				found = TAXES;
				stop = TRUE;
				make_call = TRUE;
				owes[player + 4] = 1;
			} else {
				ptr->cash -= ptr->taxes;
				ptr->taxes = 0;
			}

		if(cur_player == ptr) {
			calc_netw(player + 1);
			upd_netw();
			upd_coh();
		}
	}

	if(found || passby) {
		clear_line();
		message = TRUE; // can probably be removed
		if(passby && !found) {
			TxWriteMsg(rp, "MARGIN CALL ISSUED");
		} else if(stop) {
			int x = 0;
			if(found == OPTIONS) {
				clear_bottom();
				scr_status = RANKINGSUP;
				for(player = 0; player < no_of_players; ++player) {
					if(owes[player]) {
						cursor(11 + x, 4);
						++x;
						TxWriteMsg(rp, "Player %d Must Settle Margin Debts", player + 1);
					}
				}
			} else {
				clear_bottom();
				scr_status = RANKINGSUP;
				for(player = 0; player < no_of_players; ++player) {
					if(owes[4 + player]) {
						cursor(11 + x, 5);
						++x;
						TxWriteMsg(rp, "Player %d Must Settle Tax Debts", player + 1);
					}
				}
			}

			for(player = 0; player < no_of_players; ++player) {
				ptr = &players[0] + player;
				/* FIXME: the original calc_netw function was missing a return statement, so this assignment is probably a bug */
				y = calc_netw(player + 1);
				if(!q_break)
					y -= (((float) ptr->bonds) * 1000.0);
				for(x = 0; x < players[player].auto_count; ++x) {
					if((players[player].auto_ptr[x]->option_type == CALL) ||
					   (players[player].auto_ptr[x]->option_type == PUT))
						y += 1;
				}

				if(y <= 0) {
					ptr->cash = 0;
					ptr->bonds = 0;
					ptr->net_worth = 0;
					ptr->auto_count = 0;
					ptr->stock_count = 0;
					ptr->taxes = 0;
					for(x = 0; x < NO_OF_STOCKS; ++x) {
						ptr->portfolio[x].shares = 0;
						ptr->portfolio[x].margin_debt = 0;
					}
					display_rankings();
					clear_line();
					TxWriteMsg(rp, "%s IS BANKRUPT", ptr->name);
				}
			}	/* end going through players */
		}
	}

	if(stop == FALSE) {
		q_break = FALSE;
		year_over = FALSE;
	}
}

static void shift_it(char *str) {
	int i;
	for(i = 0; i < 16; ++i)
		str[i] = str[i + 4];
}

static void end_quarter() {
	int player, x, y;
	float earnings;

	timer1.status = OFF;
	timer2.status = OFF;
	timer3.status = OFF;
	in_progress = FALSE;

	for(player = 0; player < no_of_players; ++player) {
		earnings = round((double) ((((float) players[player].bonds) * BONDVALUE) * bond_rate));
		players[player].cash += earnings;
		players[player].other_earnings += earnings;
		players[player].net_worth +=
		    round((double) ((((float) players[player].bonds) * BONDVALUE) * bond_rate));
	}
	for(x = 0; x < NO_OF_STOCKS; ++x)
		history[x].price[15 + quarter] = ((int) stock_array[x].price);
	if(quarter == 4)
		for(x = 0; x < NO_OF_STOCKS; ++x) {
			for(y = 0; y < 16; ++y)
				history[x].price[y] = history[x].price[y + 4];
			for(y = 16; y < 20; ++y)
				history[x].price[y] = 0;
			shift_it(history[x].splits);
		}
	bond_rate = (q_int_rate * .85);
	upd_coh();
	upd_netw();
	q_break = TRUE;
	stop = TRUE;
	++quarter;
	if(quarter == 5)
		quarter = 1;
	upd_quarter();
	clear_line();
	TxWriteMsg(rp, "END OF QUARTER");
	factor1 = sqr(0.2 * q_int_rate);
	factor2 = sqr(0.02 * q_int_rate);
	factor3 = sqr(0.04 * q_int_rate);
	factor4 = sqr(0.3 * q_int_rate);
}

static void getsave() {
	FILE *saved;
	AUTOS *aptr;
	int stock, shares, price;
	int num, junk, tyear, player, counter, tweek;

	saved = fopen("save", "r");
	if(saved == NULL) {
		cursor(24, 0);
		TxWrite(rp, "NO SAVED GAME FOUND");
		exit(1);
	} else {
		clear_top();
		clear_bottom();
		no_of_players = get_int(saved);
		year = get_int(saved);
		quarter = (int) get_int(saved);
		week = (int) get_int(saved);
		if(year < 10)
			emphasis = (char) get_int(saved);
		prime_rate = get_float(saved);
		q_int_rate = get_float(saved);
		wk_int_rate = get_float(saved);
		bond_rate = get_float(saved);

		vx1 = get_float(saved);
		vx2 = get_float(saved);
		vx3 = get_float(saved);
		vx4 = get_float(saved);

		for(player = 0; player < no_of_players; ++player) {
			players[player].name_length = (int) get_int(saved);
			fgets(players[player].name, 21, saved);
			players[player].name[players[player].name_length] = '\0';
			players[player].cash = get_float(saved);
			players[player].net_worth = get_float(saved);
			players[player].taxes = get_float(saved);
			players[player].short_term = get_float(saved);
			players[player].long_term = get_float(saved);
			players[player].other_earnings = get_float(saved);
			players[player].bonds = (int) get_int(saved);
			players[player].stock_count = (int) get_int(saved);
			players[player].auto_count = (int) get_int(saved);

			for(counter = 0; counter < players[player].stock_count; ++counter) {
				stock = (int) get_int(saved);

				players[player].portfolio[stock].shares = (int) get_int(saved);
				players[player].portfolio[stock].margin_debt = get_float(saved);
				num = get_int(saved);
				tyear = year;
				tweek = week;
				for(junk = 0; junk < num; ++junk) {
					year = get_int(saved);
					week = (int) get_int(saved);
					shares = (int) get_int(saved);
					price = (int) get_int(saved);
					add_purchase(player + 1, stock, shares, price);
				}
				year = tyear;
				week = tweek;
			}
			for(counter = 0; counter < players[player].auto_count; ++counter) {
				aptr = players[player].auto_ptr[counter];
				aptr->minus_price = (int) get_int(saved);
				aptr->plus_price = (int) get_int(saved);
				aptr->option_type = (int) get_int(saved);
				aptr->option_price = (int) get_int(saved);
				aptr->units = (int) get_int(saved);
				aptr->stock_no = (int) get_int(saved);
				aptr->cur_price = (int) get_int(saved);
				aptr->pur_price = get_int(saved);
			}
		}
		for(counter = 0; counter < NO_OF_STOCKS; ++counter) {
			stock_array[counter].price = (int) get_int(saved);
			for(junk = 0; junk < 20; ++junk)
				history[counter].price[junk] = (int) get_int(saved);
			for(junk = 0; junk < 20; ++junk)
				history[counter].splits[junk] = (int) get_int(saved);
		}
		fclose(saved);
		//unlink("save");
		load_screen();
		screen_upd();
		load_tick_element(&ticker[0], 0);
		load_tick_element(&ticker[0] + 1, 1);
		load_tick_element(&ticker[0] + 2, 2);
	}
}

static void headers2() {
	char inmoney[5];
	int money;
	int junk1_counter, junk2_counter, junk3_counter;
	char c;
	char *fgets();


	clear_top();
	clear_bottom();
	cursor(10, 6);
	TxWrite(rp, "How Many Will Be Playing?");
	re:
	c = kbhit();
	no_of_players = c;
	if(isdigit(no_of_players))
		no_of_players -= DIFFERENCE;
	else
		no_of_players = -1;

	if((no_of_players <= 0) || (no_of_players > 4))
		goto re;
	for(junk1_counter = 0; junk1_counter < no_of_players; ++junk1_counter) {
		clear_top();
		clear_bottom();
		cursor(10, 9);
		TxWritef(rp, "Enter Player %d's Name", junk1_counter + 1);
		cursor(12, 6);
		TxWrite(rp, "(Maximum Of 20 Characters)");
		cursor(14, 6);
		TxWrite(rp, "> ");
		junk3_counter = -1;
		while(junk3_counter < 20) {
			char c;

			c = (char) kbhit();
			if(c == 0)
				goto re2;
			if(isupper(c) || islower(c) || (c == ' ') || (c == '.') || (c == ',')) {
				++junk3_counter;
				if(junk3_counter == 20) {
					DisplayBeep();
					--junk3_counter;
				} else {
					players[junk1_counter].name[junk3_counter] = c;
					TxWritef(rp, "%c", c);
				}
			} else if(c == (char) CR) {
				players[junk1_counter].name[++junk3_counter] = '\0';
				junk3_counter = 20;
			} else if(c == BACKSP) {
				if(junk3_counter >= 0) {
					--junk3_counter;
					cursor(14, 9 + junk3_counter);
					Text(rp, " ", 1);
					cursor(14, 9 + junk3_counter);
				}
			} else if(c != 0) {
				DisplayBeep();
			}
	re2:
			c = c;
		}
		junk2_counter = 0;
		while(players[junk1_counter].name[junk2_counter] != '\0')
			++junk2_counter;
		if(junk2_counter > 20) {
			junk2_counter = 20;
			players[junk1_counter].name[19] = '\0';
		}
		players[junk1_counter].name_length = junk2_counter;

	}
	money = 0;
	while((money != 25) && (money != 100) && (money != 50)) {
		junk2_counter = 0;
		clear_top();
		clear_bottom();
		cursor(10, 5);
		TxWrite(rp, "How Much Money Would You Like?");
		cursor(11, 13);
		TxWrite(rp, "(In Thousands)");
		cursor(13, 10);
		TxWrite(rp, "Enter 25, 50, or 100");
		cursor(15, 18);
		TxWrite(rp, "> ");
		c = ' ';
		while(c != CR) {
			c = kbhit();
			if((isdigit(c)) && (junk2_counter < 3)) {
				inmoney[junk2_counter++] = c;
				inmoney[junk2_counter] = '\0';
				TxWritef(rp, "%c", c);
			} else if((c == BACKSP) && (junk2_counter != 0)) {
				inmoney[junk2_counter--] = '\0';
				cursor(15, 20 + junk2_counter);
				Text(rp, " ", 1);
				cursor(15, 20 + junk2_counter);
			} else if(c == CR)
				money = atoi(inmoney);
		}
	}

	for(junk1_counter = 0; junk1_counter < no_of_players; ++junk1_counter) {
		players[junk1_counter].cash = money * 1000.0;
		players[junk1_counter].net_worth = players[junk1_counter].cash;
		players[junk1_counter].stock_count = 0;
	}
	clear_top();
	clear_bottom();
}

static void getyear(int year) {
	FILE *in;
	int i, x, counter, found;
	char datfile[11];

	x = 1000;
	counter = 0;
	found = FALSE;
	while(x > 0) {
		i = year / x;
		if(i > 0)
			found = TRUE;
		year -= (i * x);
		x /= 10;
		if(found)
			datfile[counter++] = i + DIFFERENCE;
	}
	datfile[counter++] = '.';
	datfile[counter++] = 'd';
	datfile[counter++] = 'a';
	datfile[counter++] = 't';
	datfile[counter++] = '\0';


	in = fopen(datfile, "r");
	y_mssg_count = 0;
	i = 0;

	high_prime = get_float(in);
	low_prime = get_float(in);
	x = get_int(in);

	if(quarter > 2) {
		for(i = 0; i < (2 * x); ++i) {
			while(getc(in) != '\n') ;
		}
		x = get_int(in);
	}
	y_mssg_count = x;

	for(i = 0; i < x; ++i) {
		fgets(yrly_mssgs.item[i].string, 82, in);
		yrly_mssgs.item[i].intr = get_float(in);
		yrly_mssgs.item[i].gnp = get_float(in);
		yrly_mssgs.item[i].pol = get_float(in);
		yrly_mssgs.item[i].var = get_float(in);

		yrly_mssgs.item[i].used = FALSE;
		DPRINTF("i is %d\n",i);
		DPRINTF("message %s \n",yrly_mssgs.item[i].string);
	}

	if(quarter == 1) {
		vx1 = vx2 = vx3 = vx4 = 0;
		for(i = 0; i < y_mssg_count; ++i) {
			vx1 += sqr(yrly_mssgs.item[i].intr);
			vx2 += sqr(yrly_mssgs.item[i].gnp);
			vx3 += sqr(yrly_mssgs.item[i].pol);
			vx4 += sqr(yrly_mssgs.item[i].var - 1.0);
		}
		x = get_int(in);
		for(i = 0; i < x; ++i) {

			while(getc(in) != '\n') ;
			vx1 += sqr(get_float(in));
			vx2 += sqr(get_float(in));
			vx3 += sqr(get_float(in));
			vx4 += sqr(get_float(in) - 1.0);
		}
		x += y_mssg_count;

		vx1 /= (NO_STD_MESSAGES + x);
		vx2 /= (NO_STD_MESSAGES + x);
		vx3 /= (NO_STD_MESSAGES + x);
		vx4 /= (NO_STD_MESSAGES + x);
	}

	fclose(in);
	DPRINTF("final messcount is %d\n",y_mssg_count);
}



/***************************************************/
/************  begin main body of event()  *********/
/***************************************************/
static float x4, E, R;
static float gamma1 = 1.2;
static float gamma2 = .24;
static float gamma3 = .24;

static void event(int stockno, float news) {
	int i, x, y;
	int newshares;
	NODE *temp;
	if(stockno != MMMS) {



		x4 = ((((float) gethundredths()) - 50.0) / 5000.0) * news * stock_array[stockno].e;


/* calculated expected rate of return using method 2 as outlined */


		E = ((gamma1 * stock_array[stockno].b1) +
		     (gamma2 * stock_array[stockno].b2) + (gamma3 * stock_array[stockno].b3)) * the_interest;




/* generate percent change in stock price   */


		R = (((stock_array[stockno].b1 * x1) +
		      (stock_array[stockno].b2 * x2) +
		      (stock_array[stockno].b3 * x3) + (stock_array[stockno].e * x4) + E) * 1.15) + 1.0;



/* check value of R to prevent negative stock prices */

		if(R < .01)
			R = .01;


/* calculate new stock price  */


		stock_array[stockno].price = (int) round((stock_array[stockno].price * R));

		if(stock_array[stockno].price >= SPLITPRICE) {
			history[stockno].splits[quarter + 15] = 1;
			stock_array[stockno].price /= 2;
			clear_line();
			TxWriteMsg(rp, "%s STOCK SPLITS 2 FOR 1", stock_array[stockno].name);
			cursor(24, 0);
			for(y = 0; y < no_of_players; ++y) {
				newshares = (int) players[y].portfolio[stockno].shares;

				if(((int) players[y].portfolio[stockno].shares) >= 5000) {
					players[y].portfolio[stockno].shares *= 2;
					players[y].cash += (((float) players[y].portfolio[stockno].shares) - 9999.0) *
					    ((float) stock_array[stockno].price);
					players[y].other_earnings +=
					    (((float) players[y].portfolio[stockno].shares) -
					     9999.0) * ((float) stock_array[stockno].price);
					players[y].portfolio[stockno].shares = (int) 9999;
				} else
					players[y].portfolio[stockno].shares *= 2;

				newshares = players[y].portfolio[stockno].shares - newshares;

				if(((int) players[y].portfolio[MMMS].shares) > 0) {
					players[y].cash += ((float) players[y].portfolio[MMMS].shares) * 333.0;
					players[y].other_earnings +=
					    ((float) players[y].portfolio[MMMS].shares) * 333.0;
				}


		   /*** adjust purchase record for splits *****/


				temp = players[y].portfolio[stockno].purchases;

				if(temp != NULL) {
					while(temp->next != NULL)
						temp = temp->next;
				}

				while(temp != NULL) {
					if(((int) temp->shares) < newshares) {
						newshares -= (int) temp->shares;
						temp->shares *= 2;
					} else {
						temp->shares += (int) newshares;
						newshares = 0;
					}
					temp->price /= 2;
					temp = temp->previous;
				}


			}

	      /*** show splits on screen ***/

			upd_stock(stockno);
		}

	} else {
		stock_array[MMMS].price = 0;
		x = 0;

		for(i = 0; i < NO_OF_STOCKS; ++i) {
			x += stock_array[i].price;
		}

		x -= stock_array[GOLD].price;
		x -= stock_array[REALESTATE].price;
		stock_array[MMMS].price = x / (NO_OF_STOCKS - 3);
	}

}				/* end procedure event() */


static void stall(int x) {
	int staller;
	int flag, y, z;

	flag = FALSE;
	staller = y = gethundredths();
	if(x == LONG)
		staller += 10;
	else
		staller += 8;

	if(staller > 100) {
		staller -= 99;
		flag = TRUE;
	}
	while(((z = gethundredths()) < staller) || (flag))
		if(z < y) {
			z = y;
			flag = FALSE;
			if(staller > 90)
				staller = 0;
		}
}

static void save() {
	int player, stock, temp, count;
	FILE *saveto;
	AUTOS *aptr;
	NODE *ptr;

	saveto = fopen("save", "w");
	if(saveto == NULL) {
		clear_line();
		TxWriteMsg(rp, "FAILED ATTEMPT TO SAVE");
	} else {
		fprintf(saveto, " %d %d %d %d ", no_of_players, year, quarter, week);
		if(year < 10)
			fprintf(saveto, " %d ", (int) emphasis);
		fprintf(saveto, "%6.4f %6.4f %6.4f %6.4f\n", prime_rate, q_int_rate, wk_int_rate, bond_rate);
		fprintf(saveto, "%8.6f %8.6f %8.6f %8.6f\n", vx1, vx2, vx3, vx4);
		for(player = 0; player < no_of_players; ++player) {
			fprintf(saveto, " %d %-20s\n", players[player].name_length, players[player].name);
			fprintf(saveto, "%f %f %f\n", players[player].cash, players[player].net_worth,
				players[player].taxes);
			fprintf(saveto, "%f %f %f\n", players[player].short_term, players[player].long_term,
				players[player].other_earnings);
			temp =
			    fprintf(saveto, "%d %d %d\n", players[player].bonds, players[player].stock_count,
				    players[player].auto_count);

			for(stock = 0; stock < NO_OF_STOCKS; ++stock) {
				if(players[player].portfolio[stock].shares > 0) {
					temp =
					    fprintf(saveto, "%d %d %f\n", stock,
						    players[player].portfolio[stock].shares,
						    players[player].portfolio[stock].margin_debt);
					count = 1;
					ptr = players[player].portfolio[stock].purchases;
					while(ptr->next != NULL) {
						++count;
						ptr = ptr->next;
					}
					fprintf(saveto, "%d\n", count);
					ptr = players[player].portfolio[stock].purchases;
					while(ptr != NULL) {
						fprintf(saveto, "%d %d %d %d\n", ptr->year, ptr->week, ptr->shares,
							ptr->price);
						ptr = ptr->next;
					}
				}
			}
			if(temp == -1)
				goto jump1;
			for(count = 0; count < players[player].auto_count; ++count) {
				aptr = players[player].auto_ptr[count];
				fprintf(saveto, " %d %d %d ", aptr->minus_price, aptr->plus_price, aptr->option_type);
				fprintf(saveto, "%d %d %d %d %d\n", aptr->option_price, aptr->units, aptr->stock_no,
					aptr->cur_price, aptr->pur_price);
			}
		}

		for(stock = 0; stock < NO_OF_STOCKS; ++stock) {
			fprintf(saveto, "%d\n", stock_array[stock].price);
			for(count = 0; count < 20; ++count)
				fprintf(saveto, "%d\n", history[stock].price[count]);
			for(count = 0; count < 20; ++count)
				fprintf(saveto, "%d\n", history[stock].splits[count]);
		}
		temp = fclose(saveto);
	jump1:
		if(temp == -1) {
			clear_line();
			TxWriteMsg(rp, "FAILED ATTEMPT TO SAVE");
		} else {
			clear_line();
			TxWriteMsg(rp, "GAME OVER");
			shutdown();
		}
	}
}


/********************************************/
/*** begin main body of procedure OPTION ****/
/********************************************/
/* unused - maybe was called from one of the missing functions */
static float option(int com_no, int stockno, int ex_price) {
	float d1, d2, vari, factor1, factor2, holder;
	float t;

	vari = variance(stockno);

	DPRINTF("stockno %d com_no %d\n",stockno,com_no);
	DPRINTF("variance is %f\n",vari);

	t = (53.0 - ((float) week)) / 13.0;
	DPRINTF("\n t is %d\nlog is %f\n",t,log10(1.0));

	factor1 = log10((float) stock_array[stockno].price / (float) ex_price);
	factor2 = (sqrt(vari) * sqrt((double) t));
	DPRINTF("factor 1 %f factor 2 %f\n",factor1,factor2);
	DPRINTF("sqrt vari %f sqrt t %f\n",sqrt(vari),sqrt((double) t));

	d1 = (factor1 + ((q_int_rate + (vari / 2.0)) * t)) / factor2;
	d2 = (factor1 + ((q_int_rate - (vari / 2.0)) * t)) / factor2;
	DPRINTF("d1 is %f d2 is %f\n",d1,d2);
	DPRINTF("vari / 2 * t %f\n",(vari/2.0) * t);

	if(com_no == CALL)
		holder =
		    100.0 * ((stock_array[stockno].price * normal(d1)) -
			     ((ex_price / exp((double) q_int_rate * t)) * normal(d2)));
	else
		holder =
		    100.0 * (((ex_price / exp((double) q_int_rate * t)) * normal(-d2)) -
			     (stock_array[stockno].price * normal(-d1)));

	DPRINTF("price calculated %f\n",holder);
	DPRINTF("stock %s price %d ow %f\n",stock_array[stockno].name,stock_array[stockno].price,exp(q_int_rate * t));

	if(holder < ((53.0 - ((float) week)) * 4.0))
		holder = (53.0 - ((float) week)) * 4.0;
	if((holder < ((stock_array[stockno].price - ex_price) * 100)) && (com_no == CALL))
		holder = ((stock_array[stockno].price - ex_price) * 100.0);
	if((holder < ((ex_price - stock_array[stockno].price) * 100)) && (com_no == PUT))
		holder = ((ex_price - stock_array[stockno].price) * 100.0);

	DPRINTF("price %f\n",holder);

	return (round(holder));
}


static void bonds(int units) {
	if(units + ((int) players[player - 1].bonds) > 99) {
		clear_line();
		TxWriteMsg(rp, "Maximum of 99 bonds can be held");
	} else if((((float) units) * 1000.0) <= players[player - 1].cash) {
		players[player - 1].cash -= (((float) units) * 1000.0);
		upd_coh();
		players[player - 1].bonds += ((int) units);
		if(scr_status != RANKINGSUP)
			upd_bonds();
	} else {
		clear_line();
		TxWriteMsg(rp, "INSUFFICIENT FUNDS");
	}
}

static void init() {
	int i, x;

/* initialize first quarter history */

	for(x = 0; x < NO_OF_STOCKS; ++x)
		history[x].price[15] = 100;

/* link the ticker elements together */

	for(i = 0; i < TICK_SIZE; ++i) {
		cur_tick = &ticker[0] + 1;
		if(i != TICK_SIZE - 1)
			ticker[i].next = &ticker[0] + i + 1;
		else
			ticker[i].next = &ticker[0];
		if(i != 0)
			ticker[i].previous = &ticker[0] + i - 1;
		else
			ticker[i].previous = &ticker[0] + TICK_SIZE - 1;

		load_tick_element(&ticker[0] + i, i);

	}			/* for loop setting up ticker */

/* set up auto execute array and pointers */

	for(i = 0; i < MAXPLAYERS; ++i)
		for(x = 0; x < AUTOLIMIT; ++x)
			players[i].auto_ptr[x] = &players[i].auto_array[0] + x;

/* set up screen array pointers */

	for(i = 0; i < STOCKLIMIT; ++i) {
		scr_ptr[i] = &screen[0] + i;
	}


/* initialize timers, timer1 in use, timer2 not in use */

	timer1.status = OFF;
	timer2.status = OFF;
	timer3.status = OFF;
	timer1.hour_changed = FALSE;
	timer2.hour_changed = FALSE;
	timer3.hour_changed = FALSE;

	cursor(24, 0);

}				/* procedure initialize */

static void getstd(char *infile) {
	int i;
	FILE *in = fopen(infile, "r");
	for(i = 0; i < 30; ++i) {
		fgets(std_mssgs.item[i].string, 75, in);
		std_mssgs.item[i].intr = get_float(in);
		std_mssgs.item[i].gnp = get_float(in);
		std_mssgs.item[i].pol = get_float(in);
		std_mssgs.item[i].var = get_float(in);

	}
	fclose(in);
}

/* a procedure to write the credits etc. on the screen.  It pauses */
/* then clears the screen */

static int headers1(void) {
	int stall;
	char file_name[8];
	int yorn, forh;
	int temp_year;
	char c;

	cursor(7, 7);
	TxWrite(rp, "The Financial Time Machine");
	cursor(9, 0);
	TxWrite(rp, "(C) 1985 by Lehner Communications, Inc.");
	cursor(14, 11);
	TxWrite(rp, "programmed by NMc");
	cursor(16, 6);
	TxWrite(rp, "special thanks to Ed Friedman");
	Delay(300);
	yorn = (int) ' ';
	while((yorn != (int) 'y') && (yorn != (int) 'Y') && (yorn != (int) 'n') && (yorn != (int) 'N')) {
		clear_top();
		clear_bottom();
		cursor(10, 7);
		TxWrite(rp, "Continue A Previous Game?");
		cursor(12, 13);
		TxWrite(rp, "(Type Y or N)");
		cursor(10, 33);
	re2:
		yorn = kbhit();
		if((yorn == (int) 'Y') || (yorn == (int) 'y')) {
			return (TRUE);
		} else if((yorn == (int) 'N') || (yorn == (int) 'n')) {
			temp_year = 0;
			forh = ' ';
			clear_top();
			clear_bottom();
			cursor(10, 0);
			TxWrite(rp, "Enter Historical Mode or Forecast Mode?");
			cursor(12, 13);
			TxWrite(rp, "(Type H or F)");
			while((forh != (int) 'F') && (forh != (int) 'f') && (forh != (int) 'H') && (forh != (int) 'h')) {
				cursor(24, 0);
				forh = kbhit();
			}

			if((forh == (int) 'F') || (forh == (int) 'f')) {
				temp_year = 1;
				emphasis = '\0';
				clear_top();
				clear_bottom();
				cursor(6, 7);
				TxWrite(rp, "Forecast Mode Emphasis On:");
				cursor(10, 10);
				TxWrite(rp, "1) GNP Factors");
				cursor(12, 10);
				TxWrite(rp, "2) Political Factors");
				cursor(14, 10);
				TxWrite(rp, "3) Interest Rate Factors");
				cursor(18, 12);
				TxWrite(rp, "Type 1, 2, or 3");
				while((emphasis < '1') || (emphasis > '3')) {
					cursor(24, 0);
					emphasis = kbhit();
				}

				switch (emphasis) {
					case '1':
						getstd("gnp.dat");
						break;
					case '2':
						getstd("pol.dat");
						break;
					default:
						getstd("int.dat");
						break;
				}

			} else {
				getstd("std.dat");
				while((temp_year < 1930) || (temp_year > 1980)) {
					clear_top();
					clear_bottom();
					cursor(10, 10);
					TxWrite(rp, "Enter The Game Year ");
					cursor(12, 13);
					TxWrite(rp, "(1930 to 1980)");
					cursor(14, 17);
					TxWrite(rp, "> ");
					stall = 0;
					while(stall < 5) {
						c = kbhit();
						if((isdigit(c)) && (stall < 4)) {
							file_name[stall++] = c;
							TxWritef(rp, "%c", c);
						}
						if((c == BACKSP) && (stall != 0)) {
							--stall;
							cursor(14, 18 + stall);
							Text(rp, " ", 1);
							cursor(14, 18 + stall);
						}
						if((c == CR) && (stall == 4))
							stall = 5;
					}
					temp_year = 0;
					file_name[5] = '\0';
					temp_year = atoi(file_name);
				}
			}
		} else
			goto re2;
		year = temp_year;
		clear_top();
		clear_bottom();
	}

	return (FALSE);
}

static void set_top_screen(void) {
	upd_quarter();

	cursor(0, 27);
	TxWrite(rp, "PRIME");
	upd_prime();

	upd_year();

	SetAPen(rp, 2);
	RectFill(rp, 0, 8, 320, 31);

	SetAPen(rp, 3);
	RectFill(rp, 0, 32, 320, 55);

	SetAPen(rp, 1);
}


static void set1_bottom_screen() {
	cursor(8, 0);
	TxWrite(rp, "HOLDINGS");
	cursor(8, 34);
	TxWrite(rp, "PAGE 1");
	cursor(8, (39 - cur_player->name_length)/2);
	TxWrite(rp, cur_player->name);
	cursor(10, 1);
	TxWrite(rp, " S  LOTS   VALUE   CP    MARGIN  LIMIT");
	cursor(22, 0);
	TxWrite(rp, "COH $");
	cursor(22, 17);
	TxWrite(rp, "NETW $");
	cursor(22, 36);
	TxWrite(rp, "T-");
	upd_bonds();
	upd_netw();
	upd_coh();

	SetAPen(rp, 5);
/* Draw appropriate rectangles */
	Move(rp, 4, 92);
	Draw(rp, 4, 172);
	Draw(rp, 36, 172);
	Draw(rp, 36, 76);
	Draw(rp, 4, 76);
	Draw(rp, 4, 92);
	Draw(rp, 36, 92);
	Draw(rp, 36, 172);
	Draw(rp, 76, 172);
	Draw(rp, 76, 76);
	Draw(rp, 36, 76);
	Draw(rp, 36, 92);
	Draw(rp, 76, 92);
	Draw(rp, 76, 172);
	Draw(rp, 156, 172);
	Draw(rp, 156, 76);
	Draw(rp, 76, 76);
	Draw(rp, 76, 92);
	Draw(rp, 156, 92);
	Draw(rp, 156, 172);
	Draw(rp, 188, 172);
	Draw(rp, 188, 76);
	Draw(rp, 156, 76);
	Draw(rp, 156, 92);
	Draw(rp, 188, 92);
	Draw(rp, 188, 172);
	Draw(rp, 268, 172);
	Draw(rp, 268, 76);
	Draw(rp, 188, 76);
	Draw(rp, 188, 92);
	Draw(rp, 268, 92);
	Draw(rp, 268, 172);
	Draw(rp, 316, 172);
	Draw(rp, 316, 76);
	Draw(rp, 268, 76);
	Draw(rp, 268, 92);
	Draw(rp, 316, 92);
	SetAPen(rp, 1);

	cursor(23, 0);
}


/* unused - maybe was called from one of the missing funcs */
static void set2_bottom_screen() {
	int x;

	cursor(8, 0);
	TxWrite(rp, "HOLDINGS");
	cursor(8, 34);
	TxWrite(rp, "PAGE 2");
	cursor(8, (39 - cur_player->name_length)/2);
	TxWrite(rp, cur_player->name);
	cursor(10, 1);
	TxWrite(rp, " S  LOTS   VALUE   OPTION CP  BUY SELL");
	cursor(22, 0);
	TxWrite(rp, "COH $");
	cursor(22, 17);
	TxWrite(rp, "NETW $");
	cursor(22, 36);
	TxWrite(rp, "T-");
	upd_bonds();
	upd_netw();
	upd_coh();


/************************************************************************/
/********** begin assembler setup of screen characteristics *************/
/************************************************************************/

	SetAPen(rp, 5);
/* Draw appropriate rectangles */

	Move(rp, 4, 92);
	Draw(rp, 4, 172);
	Draw(rp, 36, 172);
	Draw(rp, 36, 76);
	Draw(rp, 4, 76);
	Draw(rp, 4, 92);
	Draw(rp, 36, 92);
	Draw(rp, 36, 172);
	Draw(rp, 76, 172);
	Draw(rp, 76, 76);
	Draw(rp, 36, 76);
	Draw(rp, 36, 92);
	Draw(rp, 76, 92);
	Draw(rp, 76, 172);
	Draw(rp, 156, 172);
	Draw(rp, 156, 76);
	Draw(rp, 76, 76);
	Draw(rp, 76, 92);
	Draw(rp, 156, 92);
	Draw(rp, 156, 172);
	Draw(rp, 212, 172);
	Draw(rp, 212, 76);
	Draw(rp, 156, 76);
	Draw(rp, 156, 92);
	Draw(rp, 212, 92);
	Draw(rp, 212, 172);
	Draw(rp, 244, 172);
	Draw(rp, 244, 76);
	Draw(rp, 212, 76);
	Draw(rp, 212, 92);
	Draw(rp, 244, 92);
	Draw(rp, 244, 172);
	Draw(rp, 276, 172);
	Draw(rp, 276, 76);
	Draw(rp, 244, 76);
	Draw(rp, 244, 92);
	Draw(rp, 276, 92);
	Draw(rp, 276, 172);
	Draw(rp, 316, 172);
	Draw(rp, 316, 76);
	Draw(rp, 276, 76);
	Draw(rp, 276, 92);
	Draw(rp, 316, 92);
	SetAPen(rp, 1);
/************************************************************************/
/*********** end assembler setup of screen characteristics **************/
/************************************************************************/

}

static void upd_com_line(int com_no, int units, int stock_no, int price, int autobuy, int autosell, char *com_array,
			 int *com_char_count) {
	int divisor, variation;
	int counter, i, pr;
	int found_first;	/* flag for finding non-zero int */


	i = 1;
	while((commands[com_no][i - 1] != '\0') && (commands[com_no][i - 1] != ' ')) {
		com_array[i] = commands[com_no][i - 1];
		++i;
	}
	if((com_no == QUIT) || (com_no == SAVE)) {
		--i;
		goto finish;
	} else {
		com_array[i] = ' ';
	}

	/*  put in the number of units, in characters  */
	if(com_no != GRAPH) {
		divisor = 1000;
		found_first = FALSE;
		if(units == 0)
			com_array[++i] = '0';
		else {
			while(divisor >= 1) {
				if((!found_first) && ((units / divisor) >= 1))
					found_first = TRUE;
				if(found_first) {
					com_array[++i] = (int) (units / divisor) + DIFFERENCE;
					units = units - (((int) units / divisor) * divisor);
				}
				divisor = divisor / 10;
			}
		}
	}
	if((com_no == DELETE) || (com_no == BONDS) || (com_no == CASH) || (com_no == JUMP)) {
		if(com_no == JUMP) {
			com_array[++i] = ' ';
			com_array[++i] = 'W';
			com_array[++i] = 'E';
			com_array[++i] = 'E';
			com_array[++i] = 'K';
			com_array[++i] = 'S';
		}
		*com_char_count = i;
		goto finish;
	} else {
		com_array[++i] = ' ';
	}

	/* put in the stock name */

	if(com_no != EXERCISE) {
		for(counter = 0; stock_array[stock_no].name[counter] != '\0'; ++counter)
			com_array[++i] = stock_array[stock_no].name[counter];
	}

	if((com_no == GRAPH) || (com_no == LIMIT))
		goto finish;

	/* put in the word AT */

	com_array[++i] = ' ';
	com_array[++i] = 'A';
	com_array[++i] = 'T';
	com_array[++i] = ' ';

	/* put in the price  */

	variation = 1;

nextcase:

	pr = NOT_FOUND;
	switch (variation) {
		case 1:
			if(price != NOT_FOUND) {
				pr = price;
			}
			break;
		case 2:
			if(autobuy != NONE) {
				com_array[++i] = '-';
				pr = autobuy;
			}
			break;
		case 3:
			if(autosell != NONE) {
				com_array[++i] = '+';
				pr = autosell;
			}
			break;
	}

	if(pr != NOT_FOUND) {
		divisor = 1000;
		found_first = FALSE;
		while(divisor >= 1) {
			if((!found_first) && ((pr / divisor) >= 1))
				found_first = TRUE;
			if(found_first) {
				com_array[++i] = (int) (pr / divisor) + DIFFERENCE;
				pr = pr - (((int) pr / divisor) * divisor);
			}
			divisor = divisor / 10;

		}
		com_array[++i] = ' ';
	}

	++variation;

	if(variation <= 3)
		goto nextcase;

finish:

	*com_char_count = i;
	cursor(24, 0);
	for(counter = 1; counter <= i; counter++) {
		TxWritef(rp, "%c", com_array[counter]);
	}
}				/* end procedure up_com_line() */


static void add_auto_exec(int auto_minus, int auto_plus, int option, int opt_price, int units, int stockno, int player,
			  int pur_price) {
	int count;

	count = players[player - 1].auto_count;

	players[player - 1].auto_ptr[count]->minus_price = auto_minus;
	players[player - 1].auto_ptr[count]->plus_price = auto_plus;
	players[player - 1].auto_ptr[count]->option_type = option;
	players[player - 1].auto_ptr[count]->option_price = opt_price;
	players[player - 1].auto_ptr[count]->units = units;
	players[player - 1].auto_ptr[count]->stock_no = stockno;
	players[player - 1].auto_ptr[count]->cur_price = stock_array[stockno].price;
	players[player - 1].auto_ptr[count]->pur_price = pur_price;

	++players[player - 1].auto_count;

	if((cur_player == &players[0] + player - 1) && (scr_status == AUTOSUP))
		write_auto(count);
	cursor(24, 0);
}

/************************************************************/
/* Rewrites the entire set of auto execs for the cur_player */
/************************************************************/

static void auto_display() {
	int counter;

	for(counter = 0; counter < cur_player->auto_count; ++counter)
		write_auto(counter);
	cursor(24, 0);
}


/************************************************************/
/*    Writes out a single auto exec of the current player   */
/************************************************************/

static void write_auto(int position) {
	int stockno, cposition;

	stockno = cur_player->auto_ptr[position]->stock_no;
	cposition = position + 12;
	cursor(cposition, 1);
	TxWritef(rp, "%3s", stock_array[stockno].name);
	cursor(cposition, 5);
	TxWritef(rp, "%4d", cur_player->auto_ptr[position]->units);

	switch ((int) cur_player->auto_ptr[position]->option_type) {
		case PUT:
			cursor(cposition, 20);
			TxWrite(rp, "PU");
			cursor(cposition, 23);
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->option_price);
			cursor(cposition, 10);
			TxWritef(rp, "%9.0f", opt_value(player, position));
			break;
		case CALL:
			cursor(cposition, 20);
			TxWrite(rp, "CA");
			cursor(cposition, 23);
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->option_price);
			cursor(cposition, 10);
			TxWritef(rp, "%9.0f", opt_value(player, position));
			break;
		case AUTOSELL:
		case AUTOBUY:
			cursor(cposition, 10);
			TxWrite(rp, "LIMIT ORD");
			break;
	}

	cursor(cposition, 27);
	TxWritef(rp, "%3d", cur_player->auto_ptr[position]->cur_price);

	cursor(cposition, 31);
	if(cur_player->auto_ptr[position]->option_type == AUTOBUY) {
		if(cur_player->auto_ptr[position]->minus_price != NONE) {
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->minus_price);
		} else if(cur_player->auto_ptr[position]->plus_price != NONE) {
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->plus_price);
		}
	}

	else {
		TxWritef(rp, "%3s", " - ");
	}

	cursor(cposition, 36);
	if(cur_player->auto_ptr[position]->option_type == AUTOSELL) {
		if(cur_player->auto_ptr[position]->minus_price != NONE) {
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->minus_price);
		} else if(cur_player->auto_ptr[position]->plus_price != NONE) {
			TxWritef(rp, "%3d", cur_player->auto_ptr[position]->plus_price);
		}
	} else {
		TxWritef(rp, "%3s", " - ");
	}
}

static void clear_auto(int position) {
	position += 12;
	cursor(position, 1);
	TxWrite(rp, "   ");
	cursor(position, 5);
	TxWrite(rp, "    ");
	cursor(position, 10);
	TxWrite(rp, "         ");
	cursor(position, 20);
	TxWrite(rp, "      ");
	cursor(position, 27);
	TxWrite(rp, "   ");
	cursor(position, 31);
	TxWrite(rp, "   ");
	cursor(position, 35);
	TxWrite(rp, "    ");
	cursor(24, com_char_count);
}


/************************************************************/
/* Updates the current price of a stock in the cur_players  */
/* auto exec list and on the screen.                        */
/************************************************************/

static void upd_autos(int stockno) {
	int counter, flag;

	flag = FALSE;
	if(scr_status != RANKINGSUP) {
		for(counter = 0; counter < cur_player->auto_count; ++counter) {
			if((cur_player->auto_ptr[counter]->stock_no == stockno) &&
			   (cur_player->auto_ptr[counter]->cur_price != stock_array[stockno].price)) {
				flag = TRUE;
				cur_player->auto_ptr[counter]->cur_price = stock_array[stockno].price;
				if(scr_status == AUTOSUP) {
					cursor(12 + counter, 27);
					TxWritef(rp, "%3d", stock_array[stockno].price);
					if((cur_player->auto_ptr[counter]->option_type != AUTOBUY) &&
					   (cur_player->auto_ptr[counter]->option_type != AUTOSELL)) {
						cursor(12 + counter, 10);
						TxWritef(rp, "%9.0f", opt_value(player, counter));
					}
				}
			}
		}
		if(flag) {
			calc_netw(player);
			upd_netw();
		}
	}
}

/** Calculates the value of the player's (1 - 4) option in auto ptr ****/
/** POSITION.                                                       ****/

static float opt_value(int player, int position) {
	PLAYER *ptr;
	float difference;
	int stock_price;

	ptr = &players[0] + player - 1;
	if(ptr == cur_player)
		stock_price = ptr->auto_ptr[position]->cur_price;
	else
		stock_price = stock_array[ptr->auto_ptr[position]->stock_no].price;

	DPRINTF("stock price is %d\n",stock_price);

	if(ptr->auto_ptr[position]->option_type == CALL)
		difference = (float) (((int) stock_price) - ((int) ptr->auto_ptr[position]->option_price));
	else
		difference = (float) (((int) ptr->auto_ptr[position]->option_price) - ((int) stock_price));

	DPRINTF("difference is %f\n",difference);

	difference *= (100.0 * ((float) ptr->auto_ptr[position]->units));
	DPRINTF("difference is now %f\n",difference);

	if(difference > 0)
		return (difference);
	else
		return (0);
}

/****************************************************************************/
/*    This deletes the specified auto exec from the specified player. It    */
/*    updates the screen if appropriate.                                    */
/****************************************************************************/

static void del_auto(int position, int player) {
	int counter;
	AUTOS *tmp_ptr;

	tmp_ptr = players[player - 1].auto_ptr[position];
	for(counter = position; counter < AUTOLIMIT - 1; ++counter) {
		players[player - 1].auto_ptr[counter] = players[player - 1].auto_ptr[counter + 1];
	}
	players[player - 1].auto_ptr[AUTOLIMIT - 1] = tmp_ptr;
	--players[player - 1].auto_count;
	if((cur_player == &players[0] + player - 1) && (scr_status == AUTOSUP)) {
		clear_auto(cur_player->auto_count);
		auto_display();
	}
}


static void chk_autos(int stockno) {
	int count, player, tunits;
	PLAYER *ptr;

	for(player = 1; player <= no_of_players; ++player) {
		ptr = &players[0] + player - 1;
		for(count = 0; count < ptr->auto_count; ++count) {
			tunits = (int) ptr->auto_ptr[count]->units;
			if((((int) ptr->auto_ptr[count]->stock_no) == stockno) &&
			   (((int) ptr->auto_ptr[count]->option_type) == AUTOBUY)) {
				if((((int) ptr->auto_ptr[count]->minus_price) >= stock_array[stockno].price) &&
				   (((int) ptr->auto_ptr[count]->minus_price) > 0) &&
				   (ptr->cash >=
				    round(((float) ptr->auto_ptr[count]->units) * ((float) stock_array[stockno].price) *
					  101.5)) && (((int) ptr->stock_count) < STOCKLIMIT)) {
					del_auto(count, player);
					--count;
					execute(BUY, tunits, stockno, stock_array[stockno].price, NONE, NONE, player);
				} else if((((int) ptr->auto_ptr[count]->plus_price) <= stock_array[stockno].price) &&
					  (((int) ptr->auto_ptr[count]->plus_price) > 0) &&
					  (ptr->cash >= round(((float) ptr->auto_ptr[count]->units) *
							      ((float) stock_array[stockno].price) * 101.5)) &&
					  (((int) ptr->stock_count) < STOCKLIMIT))
				{
					del_auto(count, player);
					--count;
					execute(BUY, tunits, stockno, stock_array[stockno].price, NONE, NONE, player);
				}
			}
			/* end if doing an auto_minus */
			else if((((int) ptr->auto_ptr[count]->stock_no) == stockno) &&
				(((int) ptr->auto_ptr[count]->option_type) == AUTOSELL)) {
				if((((int) ptr->auto_ptr[count]->minus_price) > 0) &&
				   (((int) ptr->auto_ptr[count]->minus_price) >= stock_array[stockno].price)) {
					del_auto(count, player);
					--count;
					if(tunits > ((int) ptr->portfolio[stockno].shares))
						tunits = ((int) ptr->portfolio[stockno].shares);
					if(tunits > 0)
						execute(SELL, tunits, stockno, stock_array[stockno].price, NONE, NONE,
							player);
				} else if((((int) ptr->auto_ptr[count]->plus_price) > 0)
					  && (((int) ptr->auto_ptr[count]->plus_price) <= stock_array[stockno].price)) {
					del_auto(count, player);
					--count;
					if(tunits > ((int) ptr->portfolio[stockno].shares))
						tunits = ((int) ptr->portfolio[stockno].shares);
					if(tunits > 0)
						execute(SELL, tunits, stockno, stock_array[stockno].price, NONE, NONE,
							player);
				}

			}
		     /** end if doing an AUTOSELL  **/
		}
		for(count = 0; count < NO_OF_STOCKS; ++count) {
			if((((int) ptr->portfolio[count].limit) >= stock_array[count].price) &&
			   (((int) ptr->portfolio[count].shares) > 0))
				execute(SELL, ((int) ptr->portfolio[count].shares), count, stock_array[stockno].price,
					NONE, NONE, player);
		}

	}
}

static void upd_coh() {
	if((scr_status != RANKINGSUP) && (scr_status != GRAPHUP)) {
		cursor(22, 5);
		TxWritef(rp, "%-12.0f", cur_player->cash);
		cursor(24, 0);
	}
}

static void upd_netw() {
	if((scr_status != RANKINGSUP) && (scr_status != GRAPHUP)) {
		cursor(22, 23);
		TxWritef(rp, "%-12.0f", cur_player->net_worth);
		cursor(24, 0);
	}
}

static void upd_bonds() {
	if(scr_status != RANKINGSUP) {
		cursor(22, 38);
		TxWritef(rp, "%2d", cur_player->bonds);
		cursor(24, 0);
	}
}

static const char* getquartername(int q) {
	assert(q < 5 && q > 0);
	static const char quarters[4][4] = {"1ST", "2ND", "3RD", "4TH"};
	return quarters[q-1];
}

static void upd_quarter() {
	int q = quarter % 4;
	if(!q) q = 4;
	cursor(0, 0);
	SetAPen(rp, 0);
	RectFill(rp, 0,0,FONT_W*3,FONT_H);
	SetAPen(rp, 1);
	TxWrite(rp,getquartername(q));
	TxWrite(rp, " QUARTER");
	cursor(24, com_char_count);
}

static void upd_year() {
	if(year < 1000) {
		cursor(0, 17);
		TxWritef(rp, "YEAR %d", year);
	} else {
		cursor(0, 18);
		TxWritef(rp, "%4d", year);
	}
	cursor(24, com_char_count);
}

static void upd_prime() {
	float x;
	SetAPen(rp, 0);
	RectFill(rp,SCREENW-5*FONT_W,0,SCREENW,FONT_H);
	SetAPen(rp, 1);
	x = prime_rate * 100;
	cursor(0, 34);
	TxWritef(rp, "%5.2f%%", x);
	cursor(24, com_char_count);
}

static void write_stock(int line_inc, int element) {
	int stockno;

	stockno = scr_ptr[element]->stock_no;
	cursor(12 + line_inc, 1);
	TxWritef(rp, "%3s", stock_array[stockno].name);
	cursor(12 + line_inc, 5);
	TxWritef(rp, "%4d", scr_ptr[element]->units);
	cursor(12 + line_inc, 10);
	TxWritef(rp, "%9.0f", ((float) cur_player->portfolio[stockno].shares) * (stock_array[stockno].price * 100.0));
	cursor(12 + line_inc, 20);
	TxWritef(rp, "%3d", scr_ptr[element]->price);
	cursor(12 + line_inc, 24);
	TxWritef(rp, "%9.0f", round((double) cur_player->portfolio[stockno].margin_debt));
	cursor(12 + line_inc, 36);
	if(cur_player->portfolio[stockno].limit > 0) {
		TxWritef(rp, "%3d", cur_player->portfolio[stockno].limit);
	} else {
		TxWritef(rp, "%3s", " ");
	}
}

static void clear_stock(int line_inc) {
	line_inc += 12;
	cursor(line_inc, 1);
	TxWritef(rp, "%3s", " ");
	cursor(line_inc, 5);
	TxWritef(rp, "%4s", " ");
	cursor(line_inc, 10);
	TxWritef(rp, "%9s", " ");
	cursor(line_inc, 20);
	TxWritef(rp, "%3s", " ");
	cursor(line_inc, 24);
	TxWritef(rp, "%9s", " ");
	cursor(line_inc, 34);
	TxWritef(rp, "%5s", " ");
}


static void load_screen() {
	int counter;

	screen_count = 0;
	for(counter = 0; counter < NO_OF_STOCKS; ++counter) {
		if((cur_player->portfolio[counter].shares > 0) || (cur_player->portfolio[counter].margin_debt > 0)) {
			scr_ptr[screen_count]->stock_no = ((int) counter);
			scr_ptr[screen_count]->price = ((int) stock_array[counter].price);
			scr_ptr[screen_count]->margin_debt = cur_player->portfolio[counter].margin_debt;
			scr_ptr[screen_count]->units = cur_player->portfolio[counter].shares;
			++screen_count;
		}
	}
	for(counter = 0; counter < cur_player->auto_count; ++counter) {
		cur_player->auto_ptr[counter]->cur_price =
		    ((int) stock_array[cur_player->auto_ptr[counter]->stock_no].price);
	}
}


/* This procedure loops through all the current players holdings and auto */
/* execs and updates the entire screen of data.                           */

static void screen_upd(void) {
	int counter;
	for(counter = 0; counter < screen_count; ++counter) {
		write_stock(counter, counter);
	}
}

static double calc_netw(int player) {
	int counter, is_up;
	double total, difference;

	total = 0;
	if((cur_player == &players[0] + player - 1) && (scr_status == STOCKSUP))
		is_up = TRUE;
	else
		is_up = FALSE;
	if(is_up) {
		for(counter = 0; counter < cur_player->stock_count; ++counter) {
			total += (100.0 * ((float) scr_ptr[counter]->units) * ((float) scr_ptr[counter]->price))
			    - round((double) scr_ptr[counter]->margin_debt);
		}
	} else {
		for(counter = 0; counter < NO_OF_STOCKS; ++counter) {
			total += (100.0 * ((float) players[player - 1].portfolio[counter].shares) *
				  ((float) stock_array[counter].price)) -
			    players[player - 1].portfolio[counter].margin_debt;
		}
	}
	for(counter = 0; counter < players[player - 1].auto_count; ++counter) {
		if((players[player - 1].auto_ptr[counter]->option_type == PUT) ||
		   (players[player - 1].auto_ptr[counter]->option_type == CALL))

			difference = opt_value(player, counter);

		else
			/* if it is an auto transaction instead of an option */
			difference = 0;

		total += difference;
	}

	total += players[player - 1].cash;
	total += ((float) players[player - 1].bonds) * 1000.0;

	players[player - 1].net_worth = total;
	return total;
}

static void upd_stock(int stockno) {
	int tally, counter;
	float garbage;

	tally = 0;
	counter = 0;

	if(scr_status == STOCKSUP) {
		while((scr_ptr[tally]->stock_no != stockno) && (tally < screen_count)) ++tally;

		/* if the stock is a new acquisition */

		if((tally == screen_count) && (cur_player->portfolio[stockno].shares != 0)) {
			scr_ptr[screen_count]->stock_no = stockno;
			scr_ptr[screen_count]->units = cur_player->portfolio[stockno].shares;
			scr_ptr[screen_count]->price = stock_array[stockno].price;
			scr_ptr[screen_count]->margin_debt = cur_player->portfolio[stockno].margin_debt;

			write_stock(tally, screen_count);
			++screen_count;
			goto continue4;
		}

		/* if player has sold all shares of the stock */

		if((cur_player->portfolio[stockno].shares == 0) &&
		   (cur_player->portfolio[stockno].margin_debt == 0) && (tally < screen_count)) {
			SCREEN *tmp_ptr;

			tmp_ptr = scr_ptr[tally];
			for(counter = tally; counter < STOCKLIMIT - 1; ++counter) {
				scr_ptr[counter] = scr_ptr[counter + 1];
			}
			scr_ptr[STOCKLIMIT - 1] = tmp_ptr;
			--screen_count;
			for(counter = tally; counter < screen_count; ++counter) {
				write_stock(counter, counter);
			}
			clear_stock(screen_count);
			cursor(24, 0);
			goto continue4;
		}

		/* if a change in the amount or value of a stock already held occured */

		if((cur_player->portfolio[stockno].shares > 0) || (cur_player->portfolio[stockno].margin_debt > 0)) {
			/* update the players netw according to the current prices */

			if(scr_ptr[tally]->price != stock_array[stockno].price) {
				cur_player->net_worth +=
				    (float) (((int) stock_array[stockno].price -
					      (int) scr_ptr[tally]->price) * 100.0 * ((int) scr_ptr[tally]->units));
				upd_netw();
				scr_ptr[tally]->price = stock_array[stockno].price;
				cursor(12 + tally, 10);
				garbage = (float) (scr_ptr[tally]->price) * (float) (scr_ptr[tally]->units) * 100.0;
				TxWritef(rp, "%9.0f", garbage);
				cursor(12 + tally, 20);
				TxWritef(rp, "%3d", scr_ptr[tally]->price);
				cursor(24, com_char_count);
				goto continue3;
			}
			if(scr_ptr[tally]->units != cur_player->portfolio[stockno].shares) {
				scr_ptr[tally]->units = cur_player->portfolio[stockno].shares;
				scr_ptr[tally]->margin_debt = cur_player->portfolio[stockno].margin_debt;
				write_stock(tally, tally);
				cursor(24, com_char_count);
				goto continue3;
			}
			if(scr_ptr[tally]->margin_debt != cur_player->portfolio[stockno].margin_debt) {
				scr_ptr[tally]->margin_debt = cur_player->portfolio[stockno].margin_debt;
				cursor(12 + tally, 24);
				TxWritef(rp, "%9.0f", round((float) scr_ptr[tally]->margin_debt));
				cursor(24, com_char_count);
			}
	continue3:
			if((cur_player->portfolio[stockno].shares == 0) && (!stop))
				make_call = TRUE;
		}
		/* end: update a current holding */
	continue4:;
	}			/* end: if rankings aren't up */
}

int main() {
	int stock, ic, i;
	char c;
	int junk_counter;	/* a throwaway counter variable */
	int hit;		/* which PLAYER function key was hit */

	cur_player = &players[0];           /* set current player as player 1 */

	ezsdl_init(SU(SCREENW), SU(SCREENH));
	init_gfx();
	struct RastPort foo = {0};
	rp = &foo;

	SetRGB4(rp,0,0,0,0);
	SetRGB4(rp,1,15,15,15);
	SetRGB4(rp,2,15,0,0);
	SetRGB4(rp,3,15,15,0);
	SetRGB4(rp,4,0,15,0);
	SetRGB4(rp,5,0,0,15);

	SetAPen(rp, 1);

/* puts us in 40x25 color text mode */



/* build the ticker ring and set up the player data, initialize timers */

	init();

	for(junk_counter = 0; junk_counter < NO_OF_STOCKS; ++junk_counter) {
		players[0].portfolio[junk_counter].purchases = NULL;
		players[1].portfolio[junk_counter].purchases = NULL;
		players[2].portfolio[junk_counter].purchases = NULL;
		players[3].portfolio[junk_counter].purchases = NULL;
	}

/* header messages sent then screen cleared */
	i = headers1();
	if(i == TRUE) {
		getsave();
		if(year > 10) {
			getstd("std.dat");
			goto cont4;
		}
		if(emphasis == '1') {
			getstd("gnp.dat");
			goto cont4;
		}
		if(emphasis == '2') {
			getstd("pol.dat");
			goto cont4;
		}

		getstd("int.dat");
		goto cont4;
	}
	headers2();

	cont4:
	getyear(year); /** no overlay as of 11/22/85  **/

	prime_rate = (high_prime + low_prime) / 2.0;

/* puts in the news and stock ticker lines, quarter, prime rate, etc. */

	set_top_screen();

/* sets up the player data section, player name, coh, netw, etc. */

	set1_bottom_screen();


	while((year != 1985) && (year != 6)) {
		// reduce CPU usage
		ezsdl_sleep(20);

		if(((ic = kbhit()) != 0) || ((q_break == TRUE) && (jump_weeks > 0) && (make_call == FALSE))) {

			if(message) {
				clear_line();
				message = FALSE;
				com_char_count = 0;
			}

			if((q_break == TRUE) && (jump_weeks > 0) && (make_call == FALSE)) {
				c = F10;
			} else {
				c = (char) ic;
			}

			if(((scr_status == RANKINGSUP) || (scr_status == GRAPHUP)) && ((c < -80) || (c > -71))) {

				if(scr_status == GRAPHUP) {
					clear_top();
					set_top_screen();
					tick_position = 0;
				}
				scr_status = STOCKSUP;
				clear_bottom();
				set1_bottom_screen();
				load_screen();
				screen_upd();
				upd_coh();
				calc_netw(player);
				upd_netw();
				upd_bonds();
			}

			if((c >= -80) && (c <= -71)) {

				if(scr_status == GRAPHUP) {
					scr_status = RANKINGSUP;
					clear_top();
					set_top_screen();
					tick_position = 0;
				}

				switch (c) {
					case -79:
						if(timer2.status == OFF)
							tmp_player = 1;	/* F1 */
						hit = 1;
						break;
					case -78:
						if(timer2.status == OFF)
							tmp_player = 2;	/* F1 */
						hit = 2;
						break;
					case -77:
						if(timer2.status == OFF)
							tmp_player = 3;	/* F3 */
						hit = 3;
						break;
					case -76:
						if(timer2.status == OFF)
							tmp_player = 4;	/* F4 */
						hit = 4;
						break;
					case -73:
						display_rankings();
						scr_status = RANKINGSUP;	/* F7 */
						com_char_count = 0;
						hit = NOT_FOUND;
						break;
					case -72:
						earnings_display();
						scr_status = RANKINGSUP;	/* F8 */
						com_char_count = 0;
						hit = NOT_FOUND;
						break;

					case -71:
						clear_bottom();
						cursor(10, 0);
						for(stock = 0; stock < (NO_OF_STOCKS - 1); stock += 3) {
							cursor(10 + (int) (stock / 3), 0);
							TxWritef(rp, "  %3s - %3d    %3s - %3d    %3s - %3d",
								stock_array[alpha[stock]].name,
								stock_array[alpha[stock]].price,
								stock_array[alpha[stock + 1]].name,
								stock_array[alpha[stock + 1]].price,
								stock_array[alpha[stock + 2]].name,
								stock_array[alpha[stock + 2]].price);
						}
						cursor(21, 15);
#if 0
						tempstr[0] = stock_array[MMMS].name[0];
						tempstr[1] = stock_array[MMMS].name[1];
						tempstr[2] = stock_array[MMMS].name[2];
						tempstr[3] = stock_array[MMMS].name[3];
#endif
						TxWritef(rp, "%3s - %3d", stock_array[MMMS].name, stock_array[MMMS].price);
						clear_line();
						scr_status = RANKINGSUP;	/* F9 */
						com_char_count = 0;
						hit = NOT_FOUND;
						break;

					case -80:
						hit = NOT_FOUND;

						/* toggle: stop or restart game */

						if(timer1.status == OFF) {
							stop = FALSE;
							margin();
					     /** no overlay as of 11/22/85  **/
						} else {
							stop = TRUE;
						}
						break;
					default:
						DisplayBeep();
						hit = NOT_FOUND;
						break;
				}

				if(((hit != NOT_FOUND) && (timer2.status == OFF)) || (hit == player)) {
					if(((com_char_count == 0) ||
					    (player == hit) || (timer1.status == OFF)) && (hit == player)) {
						change_player(hit);
						goto continue1;
					}
					if(((com_char_count == 0) ||
					    (player == hit) || (timer1.status == OFF)) && (hit != player)) {
						timer2.status = OFF;
						change_player(tmp_player);
						goto continue1;
					}

					timer2.status = ON;
					timer2.count = time_count + CHANGEOVERTIME;

/* if the timer is about to run over the hour we set a flag and then set */
/* the timer beyond the hour change */

					if(timer2.count >= 3599) {
						timer2.count -= 3599;
						timer2.hour_changed = TRUE;
					}
				}

				goto continue1;
			}

			if(c == BACKSP) {
				if(com_char_count != 0) {
					--com_char_count;
					cursor(24, com_char_count);
					Text(rp, " ", 1);
					cursor(24, com_char_count);
				}
				goto continue1;
			}
			if(c == ESC) {
				if(validity_check
				   (com_array, &com_char_count, &com_no, &units, &stock_no, &trans_price, &autobuy,
				    &autosell) == TRUE) {
					clear_line();
					upd_com_line(com_no, units, stock_no, trans_price, autobuy, autosell, com_array,
						     &com_char_count);
				}

				goto continue1;
			}
			if(c == CR) {
				com_array[1+com_char_count] = 0;
				if(com_char_count == 0) {
					timer2.count = 0;
					timer2.hour_changed = FALSE;
				}
				if(validity_check
				   (com_array, &com_char_count, &com_no, &units, &stock_no, &trans_price, &autobuy,
				    &autosell) == TRUE) {
					com_char_count = 0;
					timer2.count = 0;	/* timer over */
					timer2.hour_changed = FALSE;
					clear_line();
					execute(com_no, units, stock_no, trans_price, autobuy, autosell, player);
					cursor(24, 0);
				}
				goto continue1;
			}
			if(c == BREAK) {
				int x;
				cursor(24, 0);
				for(x = 1; x <= com_char_count; ++x) {
					TxWrite(rp, " ");
				}
				cursor(24, 0);
				com_char_count = 0;
				goto continue1;
			}
			if(c == CLICK_CHANGE) {
				if(click_on == TRUE)
					click_on = FALSE;
				else
					click_on = TRUE;

				goto continue1;
			}
			if(com_char_count == 29) {	/* buffer is full */
				DisplayBeep();
				goto continue1;
			}
			if(isdigit(c)) {
				++com_char_count;
				com_array[com_char_count] = c;
				TxWritef(rp, "%c", c);
				goto continue1;
			}
			if((isupper(c)) || (c == ' ') || (c == '+') || (c == '-') || (c == '*')) {
				++com_char_count;
				com_array[com_char_count] = c;
				TxWritef(rp, "%c", c);
				goto continue1;
			}
			if(islower(c)) {
				c = toupper(c);
				++com_char_count;
				com_array[com_char_count] = c;
				TxWritef(rp, "%c", c);
				goto continue1;
			}
			DisplayBeep();

		}


		/* end if kbhit() */
 /********** BEGIN SCREEN SCROLLING STUFF ****************/
	continue1:
		if(timer1.status == ON) {
			if(newsline[cur_news_line] == '\n')
			{
				if((half_way == -2) && (in_progress == TRUE)) {
					half_way = load_stock + 6;
					if(half_way > (NO_OF_STOCKS - 1))
						half_way -= NO_OF_STOCKS;

				}

				newsch = ' ';
				delay = !delay;


			} else {
				if(delay == TRUE) {

					delay = FALSE;
					if(cur_news_line == -1)
						newsch = ' ';
					else
						newsch = newsline[cur_news_line++];
				} else {
					delay = TRUE;
				}
			}


			if(cur_tick->item[tick_position] == '\0') {
				tick_position = 0;
				++time_count;
				if(time_count == 3600)
					time_count = 0;
				cur_tick = cur_tick->next;
				if(in_progress == TRUE) {
					event(load_stock, var);
					if(half_way == load_stock) {
						half_way = -1;
						cur_news_line = 0;

					}
					if(jump_weeks < 1)
						stall(SHORT);
				} else {
					if(jump_weeks < 1)
						stall(LONG);
				}
				load_tick_element(cur_tick->previous, load_stock);
				chk_autos(load_stock);
				upd_stock(load_stock);
				upd_autos(load_stock);
				cursor(24, com_char_count);
				if(load_stock != NO_OF_STOCKS - 1)
					++load_stock;
				else
					load_stock = 0;
			} else {
				if(jump_weeks < 1)
					stall(LONG);
			}
			if(!delay)
				scrollne(newsch);
			scrollit(cur_tick->item[tick_position]);
			++tick_position;
			if(click_on == TRUE)
				click();
		}

/************ END SCREEN SCROLLING STUFF ****************/



		time = time_count;
		if(time == 0) {
			timer1.hour_changed = FALSE;
			timer2.hour_changed = FALSE;
			timer3.hour_changed = FALSE;
		}

		if((timer3.status == OFF) &&
		   (!stop) && (!in_progress) && ((cur_news_line == -1) || (newsline[cur_news_line] == '\n'))) {
			timer3.count = time + 8 + ((int) round((double) frand() * 3.0));
			if(timer3.count >= 3599) {
				timer3.count -= 3599;
				timer3.hour_changed = TRUE;
			}
			timer3.status = ON;

		} else if((timer3.status == ON) && (time > timer3.count) && (!in_progress) && (!timer3.hour_changed)) {
			float j, k, l, m;

			cur_news_line = 0;
			half_way = -2;
			j = frand();
			k = ((float) y_mssg_count - 0.00001);
			l = j * k;
			messno = (int) l;
			DPRINTF("%f %f %f %f %d  %d\n",j,k,l,m,messno,y_mssg_count);

			if(yrly_mssgs.item[messno].used == FALSE) {

				for(g = 0; g < 82; ++g)
					newsline[g] = yrly_mssgs.item[messno].string[g];

				yrly_mssgs.item[messno].used = TRUE;
				mess_type = YRLY;

			} else {


				messno = (int) (frand() * 29.9999);


				for(g = 0; g < 82; ++g)
					newsline[g] = std_mssgs.item[messno].string[g];
				mess_type = STD;

			}
			in_progress = 2;

			if(mess_type == STD) {
				gnp = std_mssgs.item[messno].gnp;
				pol = std_mssgs.item[messno].pol;
				intr = std_mssgs.item[messno].intr;
				var = std_mssgs.item[messno].var;

			} else {
				gnp = yrly_mssgs.item[messno].gnp;
				pol = yrly_mssgs.item[messno].pol;
				intr = yrly_mssgs.item[messno].intr;
				var = yrly_mssgs.item[messno].var;
			}

			instd1 = .2 * q_int_rate;
			instd2 = .02 * q_int_rate;
			instd3 = .04 * q_int_rate;
			instd4 = .3 * q_int_rate;

			x1 = myrandom(instd1) + gnp;
			x2 = myrandom(instd2) + pol;
			x3 = myrandom(instd4) + intr;

			prime_rate -= intr / 2.0;
			if(prime_rate > high_prime)
				prime_rate = high_prime;
			else if(prime_rate < low_prime)
				prime_rate = low_prime;
			q_int_rate = pow(prime_rate + 1.0, 0.25) - 1.0;
			upd_prime();

			the_interest = ((float) weeks) * q_int_rate / 13.0;

			weeks = ((int) week) - ((int) event_week);
			if(weeks < 0)
				weeks = 52 - ((int) event_week) + ((int) week);
			event_week = (int) week;

			loop_to = load_stock + 2;
			if(loop_to > (NO_OF_STOCKS - 1))
				loop_to -= NO_OF_STOCKS;

		} else if((in_progress) && (load_stock == loop_to)) {
			--in_progress;
			if(in_progress == TRUE) {
				event(loop_to - 1, var);
				if(loop_to == 0)
					loop_to = NO_OF_STOCKS - 1;
				else
					--loop_to;

			} else
				event(MMMS, var);
			timer3.status = OFF;
		}

		if(timer2.status == ON) {
/* if time for changing players has run out then change 'em */

			if((time > timer2.count) && (!timer2.hour_changed)) {
				com_char_count = 0;	/* delete stuff in progress */
				clear_line();
				change_player(tmp_player);
				timer2.status = OFF;
			}
		}

		/* see if it's time to start a new week */

		if(timer1.status == ON) {
			int x, y;

			if((time > timer1.count) && (!timer1.hour_changed)) {
				++week;

				if(jump_weeks > 0)
					--jump_weeks;
				for(x = 0; x < no_of_players; ++x) {
					for(y = 0; y < NO_OF_STOCKS; ++y) {
						players[x].portfolio[y].margin_debt *= (1.0 + wk_int_rate);
						upd_stock(y);
					}
				}
				if(make_call == TRUE)
					margin();
				       /** no overlay as of 11/22/85  **/
				wk_int_rate = (q_int_rate / 13.0) * 1.15;
				if(((int) week) % 13 == 1) {
					end_quarter();
				       /** no overlay as of 11/22/85 **/
					margin();
					/** no overlay as of 11/22/85 **/
					if(((int) week) == 53)
						end_year();
				       /** no overlay as of 11/22/85 **/
					if(((((int) quarter) == 3) || (((int) quarter) == 1))
					   && ((year != 1985) && (year != 6)))
						getyear(year);
					/** no overlay as of 11/22/85 **/
				} else if(stop == TRUE) {
					timer1.status = OFF;
					timer2.status = OFF;
					timer3.status = OFF;
				} else {
					cursor(24, com_char_count);
					timer1.count = time_count + WEEKLENGTH;
					if(timer1.count >= 3599) {
						timer1.count -= 3599;
						timer1.hour_changed = TRUE;
					}
				}
			}
		} else if((timer1.status == OFF) && (!q_break) && (!stop)) {
			timer1.status = ON;
			if((timer1.count = (time_count + WEEKLENGTH)) > 3599) {
				timer1.count -= 3599;
				timer1.hour_changed = TRUE;
			}
		}
	}			/* while year != 1985 nor 6 */

	quit();
	return 0;
}
