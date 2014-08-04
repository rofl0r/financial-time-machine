#include "amiga.h"
#include "extern.h"
#include <stdlib.h>
#include <ctype.h>

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
int i;
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


#define F10 -80
          /** for JUMPing.  Added 11/22/85 **/

HISTORY history[NO_OF_STOCKS];



ASTOCK stock_array[NO_OF_STOCKS];

char alpha[] =
  { 16, 12, 2, 28, 14, 15, 11, 1, 26, 5, 22, 29, 8, 0, 13, 18, 9, 20, 4, 24, 10, 17, 25, 23, 6, 19, 21, 7, 27, 3, 30 };

SCREEN screen[STOCKLIMIT];

SCREEN *scr_ptr[STOCKLIMIT];

int screen_count = 0;

YMESSS yrly_mssgs;

int y_mssg_count;

SMESSS std_mssgs;


FILE *fp,*fopen();

char commands[NO_OF_COMMANDS] [9] =
  {
    "BUY   ",
    "SELL  ",
    "CALL  ",
    "PUT   ",
    "DELETE",
    "QUIT  ",
    "MARGIN",
    "T-BILLS",
    "KEEP",
    "CASH",
    "GRAPH",
    "EXERCISE",
    "LIMIT",
    "JUMP"
  };


TICK_ITEM *cur_tick;
TICK_ITEM ticker[TICK_SIZE];


int load_stock = TICK_SIZE;     /* the number of the next stock to be loaded into
                           the ticker. */

char newsline[82];         /* holds current news message */

int cur_news_line = -1;   /* the number of the last position in the newline
                           array to be put on screen */

PLAYER players[MAXPLAYERS];
PLAYER *cur_player;

TIMER timer1;                   /* for game time timing */
TIMER timer2;                   /* for changing player timing */
TIMER timer3;                   /* for event gap timing */


char message;                    /* true if program message displayed */

char scr_status = STOCKSUP;      /* either stocks,autos,or rankings are up */

char in_progress = FALSE;        /* in the middle of a news event ? */
int iter;                       /* the number of times an event has an effect */
char mess_type;                  /* yearly or standard message in use */

char q_break = FALSE;           /* true at end of quarter */
int no_of_players;              /* how many players we have, set at beginning */
int player = 1;                 /* the current player */
int tmp_player;                 /* holds the new player while we wait */
float prime_rate = .05;         /* actually will be read in initially */
float high_prime;               /* highest prime rate allowed */
float low_prime;                /* lowest prime rate allowed */
float q_int_rate = .049;        /* the quarterly interest rate */
float wk_int_rate = .001;       /* quarterly divided by 13 */
float bond_rate;                /* quarterly rate at beginning of quarter */
int quarter = 1;      /* what the current quarter is */
int week = 1;         /* the current week of the year */
int year = 1938;                /* actually will be read in initially */
char stop = FALSE;              /* flag to indicate end of quarter break */
int  com_char_count;            /* how many chars in the buffer */
char delay = FALSE;             /* scroll another news char or not */
char year_over = FALSE;         /* paused at end of year ?  */
int event_week = 1;   /* the week of the last event */
char com_array[30];             /* holds user input */
int time_count;                 /* 0 to 3600 replaces real time */
int make_call = FALSE;          /* do a margin call at end of week ? */


double round(),calc_netw();

float factor1,factor2,factor3,factor4;
float x1,x2,x3,instd1,instd2,instd3,instd4,the_interest,gnp,pol,intr,var,myrandom();
int weeks;

int jump_weeks = 0;                /* number of weeks left in JUMP */
                                   /* added: 11/22/85              */



char newsch;                        /* the next newsline char to scroll */
int units;                          /* how many to buy,sell,etc. */
int com_no;                         /* the digit corres. to the command */
int stock_no;                       /* the number corres. to the stock */
int trans_price;                    /* the price at which the transaction
                                       is to be executed */
int autobuy;                        /* the price to execute an auto buy */
int autosell;                       /* the price to execute an auto sell */
int tick_position;                  /* the position in the current ticker
                                           element */
int click_on;                       /* flag to turn on clicking */
int time;                           /* current time seconds + minutes */
int messno;                         /* message selected */
int loop_to;                        /* when to end the event */
int half_way = -2;                  /* half thru reset message */
int g;                              /* junk */
char emphasis;

char id[6] = "00000";
char serial[8] = "0000000";
int tries;

float vx1, vx2, vx3, vx4;           /* variances for option use */

struct TextAttr MyFont =
  {
  "topaz.font",
  TOPAZ_EIGHTY,
  FS_NORMAL,
  FPF_ROMFONT,
  };

#if 0
struct NewScreen NS =
  {
  0,
  0,
  320,
  200,
  3,
  0,
  1,
  NULL,
  CUSTOMSCREEN,
  &MyFont,
  NULL,
  NULL,
  NULL,
  };
#endif
struct IntuiMessage *messg;
#if 0
struct Screen *S;
struct NewWindow NW =
  {
  0,
  0,
  320,
  200,
  0,
  1,
  VANILLAKEY,
  BORDERLESS | ACTIVATE | NOCAREREFRESH,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  0,
  0,
  0,
  CUSTOMSCREEN,
  };

struct Window *W;
#endif
extern struct MsgPort *CreatePort();
unsigned char allocationMap[] = {1, 2, 4, 8};
signed char si[] = {0,49,90,117,127,117,90,49,0,-49,-90,-117,-127,-117,-90,-49};
struct IOAudio *ioa;
struct RastPort *rp;
//struct ViewPort *vp;
char tempstr[250];
extern char headers1();

float frand();

main()
  {
    int stock;
    int ic;
    char c;
    int junk_counter;          /* a throwaway counter variable */
    int hit;                            /* which PLAYER function key was hit */
    int getkb();
    int kbhit();
    float myrandom();
    double pow(), exp(), log(), round();

  for (i=0; i<31; i++)
    stock_array[i].price = 100;
  stock_array[0].name[0] = 'K';
  stock_array[0].name[1] = 'M';
  stock_array[0].name[2] = '\0';
  stock_array[0].b1 = 1.4;
  stock_array[0].b2 = .8;
  stock_array[0].b3 = 0.;
  stock_array[0].e = .2;
  stock_array[1].name[0] = 'G';
  stock_array[1].name[1] = 'E';
  stock_array[1].name[2] = '\0';
  stock_array[1].b1 = 1.3;
  stock_array[1].b2 = .4;
  stock_array[1].b3 = -.2;
  stock_array[1].e = .3;
  stock_array[2].name[0] = 'A';
  stock_array[2].name[1] = 'M';
  stock_array[2].name[2] = 'R';
  stock_array[2].name[3] = '\0';
  stock_array[2].b1 = 2.1;
  stock_array[2].b2 = .9;
  stock_array[2].b3 = .2;
  stock_array[2].e = .5;
  stock_array[3].name[0] = 'Z';
  stock_array[3].name[1] = 'E';
  stock_array[3].name[2] = '\0';
  stock_array[3].b1 = 1.7;
  stock_array[3].b2 = 0.;
  stock_array[3].b3 = 0.;
  stock_array[3].e = .5;
  stock_array[4].name[0] = 'M';
  stock_array[4].name[1] = 'Y';
  stock_array[4].name[2] = 'G';
  stock_array[4].name[3] = '\0';
  stock_array[4].b1 = 1.4;
  stock_array[4].b2 = .5;
  stock_array[4].b3 = 0.;
  stock_array[4].e = .3;
  stock_array[5].name[0] = 'G';
  stock_array[5].name[1] = 'M';
  stock_array[5].name[2] = '\0';
  stock_array[5].b1 = 1.3;
  stock_array[5].b2 = .8;
  stock_array[5].b3 = .3;
  stock_array[5].e = .2;
  stock_array[6].name[0] = 'S';
  stock_array[6].name[1] = 'O';
  stock_array[6].name[2] = 'I';
  stock_array[6].name[3] = '\0';
  stock_array[6].b1 = 1.;
  stock_array[6].b2 = .4;
  stock_array[6].b3 = -.3;
  stock_array[6].e = .3;
  stock_array[7].name[0] = 'U';
  stock_array[7].name[1] = 'S';
  stock_array[7].name[2] = 'S';
  stock_array[7].name[3] = '\0';
  stock_array[7].b1 = 1.1;
  stock_array[7].b2 = .6;
  stock_array[7].b3 = 0.;
  stock_array[7].e = .3;
  stock_array[8].name[0] = 'I';
  stock_array[8].name[1] = 'T';
  stock_array[8].name[2] = 'T';
  stock_array[8].name[3] = '\0';
  stock_array[8].b1 = 1.6;
  stock_array[8].b2 = 0.;
  stock_array[8].b3 = 0.;
  stock_array[8].e = 1.2;
  stock_array[9].name[0] = 'M';
  stock_array[9].name[1] = 'O';
  stock_array[9].name[2] = 'B';
  stock_array[9].name[3] = '\0';
  stock_array[9].b1 = .9;
  stock_array[9].b2 = .2;
  stock_array[9].b3 = .8;
  stock_array[9].e = .2;
  stock_array[10].name[0] = 'P';
  stock_array[10].name[1] = 'G';
  stock_array[10].name[2] = '\0';
  stock_array[10].b1 = .85;
  stock_array[10].b2 = .3;
  stock_array[10].b3 = 0.;
  stock_array[10].e = 1.2;
  stock_array[11].name[0] = 'E';
  stock_array[11].name[1] = 'K';
  stock_array[11].name[2] = '\0';
  stock_array[11].b1 = 1.2;
  stock_array[11].b2 = 0.;
  stock_array[11].b3 = 0.;
  stock_array[11].e = .4;
  stock_array[12].name[0] = 'A';
  stock_array[12].name[1] = 'H';
  stock_array[12].name[2] = '\0';
  stock_array[12].b1 = 1.;
  stock_array[12].b2 = 1.;
  stock_array[12].b3 = .2;
  stock_array[12].e = .3;
  stock_array[13].name[0] = 'K';
  stock_array[13].name[1] = 'M';
  stock_array[13].name[2] = 'B';
  stock_array[13].name[3] = '\0';
  stock_array[13].b1 = .7;
  stock_array[13].b2 = .1;
  stock_array[13].b3 = 0.;
  stock_array[13].e = .1;
  stock_array[14].name[0] = 'D';
  stock_array[14].name[1] = 'E';
  stock_array[14].name[2] = '\0';
  stock_array[14].b1 = 1.3;
  stock_array[14].b2 = 1.3;
  stock_array[14].b3 = 0.;
  stock_array[14].e = .4;
  stock_array[15].name[0] = 'E';
  stock_array[15].name[1] = 'D';
  stock_array[15].name[2] = '\0';
  stock_array[15].b1 = .65;
  stock_array[15].b2 = 1.1;
  stock_array[15].b3 = 0.;
  stock_array[15].e = .1;
  stock_array[16].name[0] = 'A';
  stock_array[16].name[1] = 'A';
  stock_array[16].name[2] = '\0';
  stock_array[16].b1 = 1.7;
  stock_array[16].b2 = 1.2;
  stock_array[16].b3 = .1;
  stock_array[16].e = .3;
  stock_array[17].name[0] = 'P';
  stock_array[17].name[1] = 'G';
  stock_array[17].name[2] = 'L';
  stock_array[17].name[3] = '\0';
  stock_array[17].b1 = .2;
  stock_array[17].b2 = 0.;
  stock_array[17].b3 = -.4;
  stock_array[17].e = 1.5;
  stock_array[18].name[0] = 'M';
  stock_array[18].name[1] = 'D';
  stock_array[18].name[2] = '\0';
  stock_array[18].b1 = 2.0;
  stock_array[18].b2 = .2;
  stock_array[18].b3 = -.5;
  stock_array[18].e = .8;
  stock_array[19].name[0] = 'T';
  stock_array[19].name[1] = '\0';
  stock_array[19].b1 = 1.2;
  stock_array[19].b2 = 0.;
  stock_array[19].b3 = 0.;
  stock_array[19].e = .7;
  stock_array[20].name[0] = 'M';
  stock_array[20].name[1] = 'T';
  stock_array[20].name[2] = 'C';
  stock_array[20].name[3] = '\0';
  stock_array[20].b1 = 1.;
  stock_array[20].b2 = .2;
  stock_array[20].b3 = 0.;
  stock_array[20].e = .4;
  stock_array[21].name[0] = 'U';
  stock_array[21].name[1] = 'N';
  stock_array[21].name[2] = 'P';
  stock_array[21].name[3] = '\0';
  stock_array[21].b1 = 1.7;
  stock_array[21].b2 = 1.4;
  stock_array[21].b3 = 0.;
  stock_array[21].e = .9;
  stock_array[22].name[0] = 'H';
  stock_array[22].name[1] = 'M';
  stock_array[22].name[2] = '\0';
  stock_array[22].b1 = -.2;
  stock_array[22].b2 = .3;
  stock_array[22].b3 = -.5;
  stock_array[22].e = .8;
  stock_array[23].name[0] = 'S';
  stock_array[23].name[1] = 'M';
  stock_array[23].name[2] = 'C';
  stock_array[23].name[3] = '\0';
  stock_array[23].b1 = 2.2;
  stock_array[23].b2 = 1.4;
  stock_array[23].b3 = 0.;
  stock_array[23].e = 1.1;
  stock_array[24].name[0] = 'N';
  stock_array[24].name[1] = 'O';
  stock_array[24].name[2] = 'M';
  stock_array[24].name[3] = '\0';
  stock_array[24].b1 = 1.3;
  stock_array[24].b2 = 1.;
  stock_array[24].b3 = -.3;
  stock_array[24].e = 1.6;
  stock_array[25].name[0] = 'R';
  stock_array[25].name[1] = 'E';
  stock_array[25].name[2] = 'S';
  stock_array[25].name[3] = '\0';
  stock_array[25].b1 = .05;
  stock_array[25].b2 = .9;
  stock_array[25].b3 = 0.;
  stock_array[25].e = .25;
  stock_array[26].name[0] = 'G';
  stock_array[26].name[1] = 'L';
  stock_array[26].name[2] = 'D';
  stock_array[26].name[3] = '\0';
  stock_array[26].b1 = -.1;
  stock_array[26].b2 = 1.1;
  stock_array[26].b3 = -.3;
  stock_array[26].e = 1.2;
  stock_array[27].name[0] = 'W';
  stock_array[27].name[1] = 'U';
  stock_array[27].name[2] = '\0';
  stock_array[27].b1 = -.2;
  stock_array[27].b2 = -.4;
  stock_array[27].b3 = 0;
  stock_array[27].e = .6;
  stock_array[28].name[0] = 'B';
  stock_array[28].name[1] = 'N';
  stock_array[28].name[2] = 'D';
  stock_array[28].name[3] = '\0';
  stock_array[28].b1 = .05;
  stock_array[28].b2 = .7;
  stock_array[28].b3 = 0.;
  stock_array[28].e = 0.;
  stock_array[29].name[0] = 'I';
  stock_array[29].name[1] = 'B';
  stock_array[29].name[2] = 'M';
  stock_array[29].name[3] = '\0';
  stock_array[29].b1 = .9;
  stock_array[29].b2 = .3;
  stock_array[29].b3 = 1.2;
  stock_array[29].e = .6;
  stock_array[30].name[0] = 'I';
  stock_array[30].name[1] = 'M';
  stock_array[30].name[2] = 'F';
  stock_array[30].name[3] = '\0';
  stock_array[30].b1 = 0.;
  stock_array[30].b2 = 0.;
  stock_array[30].b3 = 0.;
  stock_array[30].e = 0.;

#if 0
    if ((S = (struct Screen *) OpenScreen(&NS)) == NULL)
       {printf("screen failed"); exit(FALSE);}

    NW.Screen = S;
    if ((W = (struct Window *) OpenWindow(&NW)) == NULL)
       {printf("window failed"); exit(FALSE);}

    rp = W->RPort;
    vp = (struct ViewPort *)ViewPortAddress(W);

// this seems to override the color of the mouse pointer
    SetRGB4(vp,0,0,0,0);
    SetRGB4(vp,1,15,15,15);
    SetRGB4(vp,2,15,0,0);
    SetRGB4(vp,3,15,15,0);
    SetRGB4(vp,4,0,15,0);
    SetRGB4(vp,5,0,0,15);
#endif

    ioa = (struct IOAudio *) AllocMem(sizeof(*ioa),
          MEMF_PUBLIC | MEMF_CLEAR);
    if (ioa == NULL) exit(FALSE);
    ioa->ioa_Request.io_Message.mn_Node.ln_Pri = 10;
    if ((ioa->ioa_Request.io_Message.mn_ReplyPort =
       CreatePort("sound",0)) == NULL) {printf("port failed"); exit(FALSE);}
    ioa->ioa_Data = allocationMap;
    ioa->ioa_Length = sizeof(allocationMap);
    if (OpenDevice(AUDIONAME,0,ioa,0)) {printf("device failed"); exit(FALSE);}
    ioa->ioa_Request.io_Command = CMD_WRITE;
    ioa->ioa_Request.io_Flags = ADIOF_PERVOL;
    ioa->ioa_Data = (BYTE *) AllocMem(sizeof(si), MEMF_CHIP);
    if (ioa->ioa_Data == NULL) {printf("allocmem failed"); exit(FALSE);}
    else
      {
      for (i=0; i < sizeof(si); ++i)
        ioa->ioa_Data[i] = si[i];
      }
    ioa->ioa_Length = sizeof(si);
    ioa->ioa_Period = 224;
    ioa->ioa_Volume = 64;
    ioa->ioa_Cycles = 20;


    com_char_count = 0;                 /* no user input */
    message = FALSE;                    /* no message being display ? */
    cur_player = &players[0];           /* set current player as player 1 */
    click_on = TRUE;
    tick_position = 0;



/* puts us in 40x25 color text mode */



/* build the ticker ring and set up the player data, initialize timers */

    init();

      for (junk_counter=0; junk_counter < NO_OF_STOCKS; ++junk_counter)
      {
        players[0].portfolio[junk_counter].purchases = NULL;
        players[1].portfolio[junk_counter].purchases = NULL;
        players[2].portfolio[junk_counter].purchases = NULL;
        players[3].portfolio[junk_counter].purchases = NULL;
      }

/* header messages sent then screen cleared */
   i = headers1();
    if (i == TRUE)
      {
        getsave();
        if (year > 10)
          {
          getstd("std.dat");
          goto cont4;
          }
        if (emphasis == '1')
                {
                getstd("gnp.dat");
                goto cont4;
                }
        if (emphasis == '2')
                {
                getstd("pol.dat");
                goto cont4;
                }
        
        getstd("int.dat");
      goto cont4;
      }
      headers2();

cont4:    getyear(year);      /** no overlay as of 11/22/85  **/

    prime_rate = (high_prime + low_prime) / 2.0;

/* puts in the news and stock ticker lines, quarter, prime rate, etc. */

    set_top_screen();

/* sets up the player data section, player name, coh, netw, etc. */

    set1_bottom_screen();


    while ((year != 1985) && (year != 6))
      {

        if (((ic = kbhit()) != NULL) || ((q_break == TRUE) && (jump_weeks > 0) && (make_call == FALSE)))
          {

            if (message)
              {
                clear_line();
                message = FALSE;
                com_char_count = 0;
              }

            if ((q_break == TRUE) && (jump_weeks > 0) && (make_call == FALSE))
              {
                c = F10;
              }
            else
              {
                c = (char) ic;
              }

            if (((scr_status == RANKINGSUP) || (scr_status == GRAPHUP)) &&
                ((c < -80) || (c > -71)))
              {

                if (scr_status == GRAPHUP)
                  {
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

            if ((c >= -80) && (c <= -71))
              {

                if (scr_status == GRAPHUP)
                  {
                    scr_status = RANKINGSUP;
                    clear_top();
                    set_top_screen();
                    tick_position = 0;
                  }

                switch (c)
                  {
                    case -79 :  if (timer2.status == OFF)
                                tmp_player = 1;  /* F1 */
                              hit = 1;
                              break;
                    case -78 :  if (timer2.status == OFF)
                                tmp_player = 2;  /* F1 */
                              hit = 2;
                              break;
                    case -77 :  if (timer2.status == OFF)
                                tmp_player = 3;  /* F3 */
                              hit = 3;
                              break;
                    case -76 :  if (timer2.status == OFF)
                                tmp_player = 4;  /* F4 */
                              hit = 4;
                              break;
                    case -73 :  display_rankings();
                              scr_status = RANKINGSUP; /* F7 */
                              com_char_count = 0;
                              hit = NOT_FOUND;
                              break;
                    case -72 :  earnings_display();
                              scr_status = RANKINGSUP; /* F8 */
                              com_char_count = 0;
                              hit = NOT_FOUND;
                              break;

                    case -71 :
    clear_bottom();
    cursor(10,0);
    for (stock = 0; stock < (NO_OF_STOCKS - 1); stock += 3)
      {
      cursor(10 + (int)(stock/3),0);
      sprintf(tempstr,"  %3s - %3d    %3s - %3d    %3s - %3d",stock_array[alpha[stock]].name,stock_array[alpha[stock]].price,
                                                       stock_array[alpha[stock + 1]].name,stock_array[alpha[stock + 1]].price,
                                                       stock_array[alpha[stock + 2]].name,stock_array[alpha[stock + 2]].price);
      TxWrite(rp,tempstr);
      }
    cursor(21,15);
    tempstr[0] = stock_array[MMMS].name[0];
    tempstr[1] = stock_array[MMMS].name[1];
    tempstr[2] = stock_array[MMMS].name[2];
    tempstr[3] = stock_array[MMMS].name[3];
    sprintf(tempstr,"%3s - %3d",tempstr,
            stock_array[MMMS].price);
    TxWrite(rp,tempstr);
    clear_line();
                              scr_status = RANKINGSUP; /* F9 */
                              com_char_count = 0;
                              hit = NOT_FOUND;
                              break;

                    case -80 :  hit = NOT_FOUND;

                              /* toggle: stop or restart game */

                              if (timer1.status == OFF)
                                {
                                  stop = FALSE;
                                  margin();  /** no overlay as of 11/22/85  **/
                                }
                              else
                                {
                                  stop = TRUE;
                                }
                              break;
                   default:   DisplayBeep();
                              hit = NOT_FOUND;
                              break;
                  }

                if (((hit != NOT_FOUND) && (timer2.status == OFF)) ||
                    (hit == player))
                  {
                    if (((com_char_count == 0) ||
                        (player == hit) ||
                        (timer1.status == OFF)) &&
                        (hit == player))
                      {
                          change_player(hit);
                          goto continue1;
                      }
                    if (((com_char_count == 0) ||
                        (player == hit) ||
                        (timer1.status == OFF)) &&
                        (hit != player))
                          {
                            timer2.status = OFF;
                            change_player(tmp_player);
                            goto continue1;
                          }

                        timer2.status = ON;
                        timer2.count  = time_count + CHANGEOVERTIME;

/* if the timer is about to run over the hour we set a flag and then set */
/* the timer beyond the hour change */

                        if (timer2.count >= 3599 )
                          {
                            timer2.count -= 3599;
                            timer2.hour_changed = TRUE;
                          }
                  }

goto continue1; }

             if (c == BACKSP)
              {
                if (com_char_count != 0)
                  {
                    --com_char_count;
                    cursor(24,com_char_count);
                    Text(rp," ",1);
                    cursor(24,com_char_count);
                  }
goto continue1; }
             if (c==ESC)
              {
                if (validity_check(com_array,&com_char_count,&com_no,&units,&stock_no,&trans_price,&autobuy,&autosell)==
                   TRUE)
                  {
                    clear_line();
                    upd_com_line(com_no,units,stock_no,trans_price,autobuy,autosell,&com_array,&com_char_count);
                  }

goto continue1; }
            if (c==CR)
              {
                if (com_char_count == 0)
                  {
                    timer2.count = 0;
                    timer2.hour_changed = FALSE;
                  }
                if (validity_check(com_array,&com_char_count,&com_no,&units,&stock_no,&trans_price,&autobuy,&autosell)==
                   TRUE)
                  {
                    com_char_count = 0;
                    timer2.count   = 0;   /* timer over */
                    timer2.hour_changed = FALSE;
                    clear_line();
                    execute(com_no,units,stock_no,trans_price,autobuy,autosell,player);
                    cursor(24,0);
                  }
goto continue1; }
            if (c==BREAK)
              {
                int x;
                cursor(24,0);
                for (x=1;x<=com_char_count;++x)
                  {
                  sprintf(tempstr," ");
                  TxWrite(rp,tempstr);
                  }
                cursor(24,0);
                com_char_count = 0;
goto continue1; }
            if (c == CLICK_CHANGE)
              {
                if (click_on == TRUE)
                  click_on = FALSE;
                else
                  click_on = TRUE;

goto continue1; }
            if (com_char_count == 29)      /* buffer is full */
              {
                 DisplayBeep();
goto continue1; }
            if (isdigit(c))
              {
                ++com_char_count;
                com_array[com_char_count]=c;
                sprintf(tempstr,"%c",c);
                TxWrite(rp,tempstr);
goto continue1; }
            if ((isupper(c)) ||
                     (c == ' ') ||
                     (c == '+') ||
                     (c == '-') ||
                     (c == '*'))
              {
                ++com_char_count;
                com_array[com_char_count]=c;
                sprintf(tempstr,"%c",c);
                TxWrite(rp,tempstr);
goto continue1; }
            if (islower(c))
              {
                c=toupper(c);
                ++com_char_count;
                com_array[com_char_count]=c;
                sprintf(tempstr,"%c",c);
                TxWrite(rp,tempstr);
goto continue1; }
                DisplayBeep();

          }  /* end if kbhit() */

/********** BEGIN SCREEN SCROLLING STUFF ****************/

continue1:        if (timer1.status == ON)
          {
            if (newsline[cur_news_line] == '\n')

              {
                if ((half_way == -2) && (in_progress == TRUE))
                  {
                    half_way = load_stock + 6;
                    if (half_way > (NO_OF_STOCKS - 1))
                      half_way -= NO_OF_STOCKS;

                  }

                newsch = ' ';
                delay = !delay;


              }
            else
              {
                if (delay == TRUE)
                  {

                    delay = FALSE;
                    if (cur_news_line == -1)
                      newsch = ' ';
                    else
                      newsch = newsline[cur_news_line++];
                  }
                else
                  {
                    delay = TRUE;
                  }
              }


            if (cur_tick->item[tick_position] == '\0')
              {
                tick_position = 0;
                ++time_count;
                if (time_count == 3600)
                  time_count = 0;
                cur_tick = cur_tick->next;
                if (in_progress == TRUE)
                  {
                    event(load_stock,var);
                    if (half_way == load_stock)
                      {
                        half_way = -1;
                        cur_news_line = 0;

                      }
          if (jump_weeks < 1)
                      stall(SHORT);
                  }
                else
        {
                    if (jump_weeks < 1)
            stall(LONG);
        }
                load_tick_element(cur_tick->previous,load_stock);
                chk_autos(load_stock);
                upd_stock(load_stock);
                upd_autos(load_stock);
                cursor(24, com_char_count);
                if (load_stock != NO_OF_STOCKS - 1)
                  ++load_stock;
                else
                  load_stock = 0;
              }
            else
              {
      if (jump_weeks < 1)
                  stall(LONG);
              }
            if (!delay)
              scrollne(newsch);
            scrollit(cur_tick->item[tick_position]);
            ++tick_position;
            if (click_on == TRUE)
              click();
          }

/************ END SCREEN SCROLLING STUFF ****************/



        time = time_count;
        if (time == 0)
          {
            timer1.hour_changed = FALSE;
            timer2.hour_changed = FALSE;
            timer3.hour_changed = FALSE;
          }

        if ((timer3.status == OFF) &&
            (!stop) && (!in_progress) &&
            ((newsline[cur_news_line] == '\n') || (cur_news_line == -1)))
          {
            timer3.count = time + 8 + ((int) round((double) frand() * 3.0));
            if (timer3.count >= 3599)
              {
                timer3.count -= 3599;
                timer3.hour_changed = TRUE;
              }
            timer3.status = ON;

          }
        else if ((timer3.status == ON) &&
                 (time > timer3.count) &&
                 (!in_progress) &&
                 (!timer3.hour_changed))
          {
            float j,k,l,m;

            cur_news_line = 0;
            half_way = -2;
            j = frand();
            k = ((float) y_mssg_count - 0.00001 );
            l = j * k;
            messno = (int) l;
/*
fp = fopen("prn","w");
fprintf(fp,"%f %f %f %f %d  %d\n",j,k,l,m,messno,y_mssg_count);
fclose(fp);
*/

            if (yrly_mssgs.item[messno].used == FALSE)
              {

                for (g = 0; g < 82;++g)
                  newsline[g] = yrly_mssgs.item[messno].string[g];

                yrly_mssgs.item[messno].used = TRUE;
                mess_type = YRLY;

              }
            else
              {


                messno = (int)(frand() * 29.9999);


                for (g = 0; g < 82; ++g)
                  newsline[g] = std_mssgs.item[messno].string[g];
                mess_type = STD;

              }
            in_progress = 2;

            if (mess_type == STD)
              {
                gnp  = std_mssgs.item[messno].gnp;
                pol  = std_mssgs.item[messno].pol;
                intr = std_mssgs.item[messno].intr;
                var  = std_mssgs.item[messno].var;

              }
            else
              {
                gnp  = yrly_mssgs.item[messno].gnp;
                pol  = yrly_mssgs.item[messno].pol;
                intr = yrly_mssgs.item[messno].intr;
                var  = yrly_mssgs.item[messno].var;
              }

            instd1 = .2  * q_int_rate;
            instd2 = .02 * q_int_rate;
            instd3 = .04 * q_int_rate;
            instd4 = .3  * q_int_rate;

            x1 = myrandom(instd1) + gnp;
            x2 = myrandom(instd2) + pol;
            x3 = myrandom(instd4) + intr;

            prime_rate -= intr / 2.0;
            if (prime_rate > high_prime)
              prime_rate = high_prime;
            else if (prime_rate < low_prime)
              prime_rate = low_prime;
            q_int_rate = pow(prime_rate + 1.0, 0.25) -1.0;
            upd_prime();

            the_interest = ((float) weeks) * q_int_rate / 13.0;

            weeks = ((int) week) - ((int) event_week);
            if (weeks < 0)
              weeks = 52 - ((int) event_week) + ((int) week);
            event_week = (int) week;

            loop_to = load_stock + 2;
            if (loop_to > (NO_OF_STOCKS - 1))
              loop_to -= NO_OF_STOCKS;

          }
        else if ((in_progress) &&
                 (load_stock == loop_to))
          {
            --in_progress;
            if (in_progress == TRUE)
              {
                event(loop_to - 1,var);
                if (loop_to == 0)
                  loop_to = NO_OF_STOCKS - 1;
                else
                  --loop_to;

              }
            else
              event(MMMS,var);
            timer3.status = OFF;
          }

        if (timer2.status == ON)
          {
            int x;

/* if time for changing players has run out then change 'em */

            if (( time > timer2.count) &&
                (!timer2.hour_changed))
              {
                com_char_count = 0;     /* delete stuff in progress */
                clear_line();
                change_player(tmp_player);
                timer2.status = OFF;
              }
          }

  /* see if it's time to start a new week */

        if (timer1.status == ON)
          {
            int x,y;

            if (( time > timer1.count) &&
                (!timer1.hour_changed))
              {
                ++week;

                if (jump_weeks > 0)
                  --jump_weeks;
                for (x = 0; x < no_of_players; ++x)
                  {
                    for (y = 0; y < NO_OF_STOCKS; ++y)
                      {
                        players[x].portfolio[y].margin_debt *= (1.0 + wk_int_rate);
                        upd_stock(y);
                      }
                  }
                if (make_call == TRUE)
                  margin();            /** no overlay as of 11/22/85  **/
                wk_int_rate = (q_int_rate / 13.0) * 1.15;
                if ( ((int) week) % 13 == 1)
                  {
                    end_quarter();     /** no overlay as of 11/22/85 **/
                    margin();           /** no overlay as of 11/22/85 **/
                    if ( ((int) week) == 53)
                      end_year();      /** no overlay as of 11/22/85 **/
                    if ( (( ((int) quarter) == 3) || ( ((int) quarter) == 1)) && ((year != 1985) && (year != 6)))
                      getyear(year);    /** no overlay as of 11/22/85 **/
                  }
                else if (stop == TRUE)
                  {
                    timer1.status = OFF;
                    timer2.status = OFF;
                    timer3.status = OFF;
                  }
                else
                  {
                    cursor(24, com_char_count);
                    timer1.count = time_count + WEEKLENGTH;
                    if (timer1.count >= 3599)
                      {
                        timer1.count -= 3599;
                        timer1.hour_changed = TRUE;
                      }
                  }
              }
          }
        else if ((timer1.status == OFF) && (!q_break) && (!stop))
          {
            timer1.status = ON;
            if ((timer1.count = (time_count + WEEKLENGTH)) > 3599)
              {
                timer1.count -= 3599;
                timer1.hour_changed = TRUE;
              }
          }

      }  /* while year != 1985 nor 6 */

    quit();


  }  /* end main */

TxWrite(rp,ch)
struct RastPort *rp;
char *ch;
  {
  int j;
  char *c;

  c = ch;
  j = 0;
  while (*c++ != '\0')
    ++j;
  Text(rp,ch,j);
  }

