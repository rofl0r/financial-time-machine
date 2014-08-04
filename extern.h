#define cursor cursorit
#define vmode vmodeitt
#define getkb getkbitt

#define NONE 0
#define NOT_FOUND -1
#define COMMAND 1
#define STOCK 2
#define ALL -2

#define BONDVALUE 1000
#define STOCKLIMIT 9            /* max number of stocks one can hold */
#define AUTOLIMIT  9            /* max number of autos and options */

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
#define NO_OF_COMMANDS 14
#define NO_OF_STOCKS 31
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
#define AUTOBUY 20
#define AUTOSELL 21
#define NON_AUTO 1
#define ON 1
#define OFF 2
#define CHANGEOVERTIME 10               /* 10 seconds to complete command */
#define CLICK_CHANGE '\021'
#define NO_YRLY_MESSAGES 15
#define NO_STD_MESSAGES 30
#define YRLY 0
extern int i;
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


#define F10 17408          /** for JUMPing.  Added 11/22/85 **/

typedef struct stock {
                        char name[4];
                        float b1;
                        float b2;
                        float b3;
                        float  e;
                        int price;
                     }ASTOCK;

typedef struct {
                  int price[20];
                  char splits[20];
               } HISTORY;

extern HISTORY history[NO_OF_STOCKS];



extern ASTOCK stock_array[NO_OF_STOCKS];

extern char alpha[];

typedef struct
    {
      int stock_no;
      int units;
      int price;
      float  margin_debt;
    } SCREEN;

extern SCREEN screen[STOCKLIMIT];

extern SCREEN *scr_ptr[STOCKLIMIT];

extern int screen_count;



typedef struct
  {
    char string[82];
    float intr;
    float gnp;
    float pol;
    float var;
    char used;

  } YMESS;

typedef struct
  {
    YMESS item[NO_YRLY_MESSAGES];
  } YMESSS;

extern YMESSS yrly_mssgs;

extern int y_mssg_count;

typedef struct
  {
    char string[82];
    float intr;
    float gnp;
    float pol;
    float var;

  } SMESS;

typedef struct
  {
    SMESS item[NO_STD_MESSAGES];
  } SMESSS;

extern SMESSS std_mssgs;


extern FILE *fp,*fopen();

extern char commands[NO_OF_COMMANDS] [9];


typedef struct tick_item
    {
      char item[11];
      struct tick_item *next;
      struct tick_item *previous;
    } TICK_ITEM;

extern TICK_ITEM *cur_tick;
extern TICK_ITEM ticker[TICK_SIZE];


extern int load_stock;     /* the number of the next stock to be loaded into
                           the ticker. */

extern char newsline[82];         /* holds current news message */

extern int cur_news_line;   /* the number of the last position in the newline
                           array to be put on screen */
typedef struct autos
    {
      int minus_price;     /* price to exercise auto buy */
      int plus_price;    /* price to exercise auto sell */
      int option_type;   /* CALL or PUT option?  */
      int option_price;  /* price option is exerciseable at */
      int  units;         /* number of shares */
      int stock_no;      /* stock id number */
      int cur_price;     /* the last price posted */
      int  pur_price;     /* how much the option cost per lot */
    } AUTOS;


typedef struct node
    {
      int year;
      int week;
      int shares;
      struct node *next;
      struct node *previous;
      int price;
    } NODE;

typedef struct
    {
      int shares;
      int limit;
      float  margin_debt;
      NODE *purchases;
    } STOCKS;



typedef struct player
    {
      double cash;
      double net_worth;
      STOCKS portfolio[NO_OF_STOCKS];
      int stock_count;
      int auto_count;
      AUTOS *auto_ptr[AUTOLIMIT];       /* pointers to the auto exec structures */
      AUTOS auto_array[AUTOLIMIT];      /* the actual auto exec structures */
      char name[21];
      int name_length;          /* characters in the person's name */
      double taxes;
      float short_term;
      float long_term;
      float other_earnings;
      int bonds;

    } PLAYER;

extern PLAYER players[MAXPLAYERS];
extern PLAYER *cur_player;

typedef struct
    {
      int status;               /* 1 if ON 2 if OFF */
      int count;                /* seconds since hour when initialized */
      int hour_changed;         /* flag set if timer approaching hour change */
    } TIMER;

extern TIMER timer1;                   /* for game time timing */
extern TIMER timer2;                   /* for changing player timing */
extern TIMER timer3;                   /* for event gap timing */


extern char message;                    /* true if program message displayed */

extern char scr_status;      /* either stocks,autos,or rankings are up */

extern char in_progress;        /* in the middle of a news event ? */
extern int iter;                       /* the number of times an event has an effect */
extern char mess_type;                  /* yearly or standard message in use */

extern char q_break;           /* true at end of quarter */
extern int no_of_players;              /* how many players we have, set at beginning */
extern int player;                 /* the current player */
extern int tmp_player;                 /* holds the new player while we wait */
extern float prime_rate;         /* actually will be read in initially */
extern float high_prime;               /* highest prime rate allowed */
extern float low_prime;                /* lowest prime rate allowed */
extern float q_int_rate;        /* the quarterly interest rate */
extern float wk_int_rate;       /* quarterly divided by 13 */
extern float bond_rate;                /* quarterly rate at beginning of quarter */
extern int quarter;      /* what the current quarter is */
extern int week;         /* the current week of the year */
extern int year;                /* actually will be read in initially */
extern char stop;              /* flag to indicate end of quarter break */
extern int  com_char_count;            /* how many chars in the buffer */
extern char delay;             /* scroll another news char or not */
extern char year_over;         /* paused at end of year ?  */
extern int event_week;   /* the week of the last event */
extern char com_array[30];             /* holds user input */
extern int time_count;                 /* 0 to 3600 replaces real time */
extern int make_call;          /* do a margin call at end of week ? */


extern double round(),calc_netw();

extern float factor1,factor2,factor3,factor4;
extern float x1,x2,x3,instd1,instd2,instd3,instd4,the_interest,gnp,pol,intr,var,myrandom();
extern int weeks;

extern int jump_weeks;                /* number of weeks left in JUMP */
                                   /* added: 11/22/85              */



extern char newsch;                        /* the next newsline char to scroll */
extern int units;                          /* how many to buy,sell,etc. */
extern int com_no;                         /* the digit corres. to the command */
extern int stock_no;                       /* the number corres. to the stock */
extern int trans_price;                    /* the price at which the transaction
                                       is to be executed */
extern int autobuy;                        /* the price to execute an auto buy */
extern int autosell;                       /* the price to execute an auto sell */
extern int tick_position;                  /* the position in the current ticker
                                           element */
extern int click_on;                       /* flag to turn on clicking */
extern int time;                           /* current time seconds + minutes */
extern int messno;                         /* message selected */
extern int loop_to;                        /* when to end the event */
extern int half_way;                  /* half thru reset message */
extern int g;                              /* junk */
extern char emphasis;

extern char id[6];
extern char serial[8];
extern int tries;

extern float vx1, vx2, vx3, vx4;           /* variances for option use */

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

extern struct TextAttr MyFont;

extern struct NewScreen NS;

extern struct IntuiMessage *messg;

extern struct Screen *S;
extern struct NewWindow NW;

extern struct Window *W;
extern struct MsgPort *CreatePort();
extern UBYTE allocationMap[];
extern BYTE si[];
extern struct IOAudio *ioa;
extern struct RastPort *rp;
extern struct ViewPort *vp;
extern char tempstr[250];

