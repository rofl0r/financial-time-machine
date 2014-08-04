#include "amiga.h"
#include "extern.h"

float ndis[24] =
  {
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


/********************************************/
/*** begin main body of procedure OPTION ****/
/********************************************/

option(com_no,stockno,ex_price)
int com_no,stockno,ex_price;
  {
    float d1, d2, vari, factor1, factor2, variance(),normal(),holder;
    double exp(),sqrt(),sqr(),log10();
    float t;

    vari = variance(stockno);

/*
fp = fopen("prn","w");
fprintf(fp,"stockno %d com_no %d\n",stockno,com_no);
fclose(fp);
*/

/*
fp = fopen("prn","w");
fprintf(fp,"variance is %f\n",vari);
fclose(fp);
*/

    t = (53.0 - ((float) week)) / 13.0;


/*
fp = fopen("prn","w");
fprintf(fp,"\n t is %d\nlog is %f\n",t,log10(1.0));
fclose(fp);
*/

    factor1 = log10((float) stock_array[stockno].price / (float) ex_price);
    factor2 = (sqrt(vari) * sqrt((double) t));

/*
fp = fopen("prn","w");
fprintf(fp,"factor 1 %f factor 2 %f\n",factor1,factor2);
fclose(fp);
*/

/*
fp = fopen("prn","w");
fprintf(fp,"sqrt vari %f sqrt t %f\n",sqrt(vari),sqrt((double) t));
fclose(fp);
*/

    d1 = (factor1 + ((q_int_rate + (vari/2.0)) * t)) / factor2;
    d2 = (factor1 + ((q_int_rate - (vari/2.0)) * t)) / factor2;

/*
fp = fopen("prn","w");
fprintf(fp,"d1 is %f d2 is %f\n",d1,d2);
fclose(fp);


fp = fopen("prn","w");
fprintf(fp,"vari / 2 * t %f\n",(vari/2.0) * t);
fclose(fp);
*/

    if (com_no == CALL)
      holder = 100.0 * ((stock_array[stockno].price * normal(d1)) - ((ex_price/exp((double) q_int_rate * t)) *
               normal(d2)));
    else
      holder = 100.0 * (((ex_price/exp((double) q_int_rate * t)) * normal(-d2)) -  (stock_array[stockno].price *
               normal(-d1)));


/*
fp = fopen("prn","w");
fprintf(fp,"price calculated %f\n",holder);
fclose(fp);

fp = fopen("prn","w");
fprintf(fp,"stock %s price %d ow %f\n",stock_array[stockno].name,stock_array[stockno].price,exp(q_int_rate * t));
fclose(fp);

*/


    if (holder < ((53.0 - ((float) week) ) * 4.0))
      holder = (53.0 - ((float) week) ) * 4.0;
    if ((holder < ((stock_array[stockno].price - ex_price) * 100)) && (com_no == CALL))
      holder = ((stock_array[stockno].price - ex_price) * 100.0);
    if ((holder < ((ex_price - stock_array[stockno].price) * 100)) && (com_no == PUT))
      holder = ((ex_price - stock_array[stockno].price) * 100.0);

/*
fp = fopen("prn","w");
fprintf(fp,"price %f\n",holder);
fclose(fp);
*/

    return(round(holder));
  }


float variance(stockno)
int stockno;
  {
    int i;
    float j,myrandom();
    double sqr();
    float tvx1,tvx2,tvx3,tvx4;


    tvx1 = vx1 + factor1;
    tvx2 = vx2 + factor2;
    tvx3 = vx3 + factor3;
    tvx4 = vx4 + factor4;


/*
fp = fopen("prn","w");
fprintf(fp,"ran %f q_int %f\n",myrandom(2.0 * q_int_rate),q_int_rate);
fprintf(fp,"4 tvx1 %f tvx2 %f tvx3 %f tvx4 %f\n",tvx1,tvx2,tvx3,tvx4);
fclose(fp);

fp = fopen("prn","w");
fprintf(fp,"st %d b1 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b1,sqr(stock_array[stockno].b1),
         (sqr(stock_array[stockno].b1) * tvx1));
fprintf(fp,"st %d b2 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b2,sqr(stock_array[stockno].b2),
         (sqr(stock_array[stockno].b2) * tvx2));
fprintf(fp,"st %d b3 %f sqr %f sq* %f\n",stockno,stock_array[stockno].b3,sqr(stock_array[stockno].b3),
         (sqr(stock_array[stockno].b3) * tvx3));
fprintf(fp,"st %d e %f sqr %f sq* %f\n",stockno,stock_array[stockno].e,sqr(stock_array[stockno].e),
         (sqr(stock_array[stockno].e) * tvx4));
fclose(fp);

*/

    return(((sqr( stock_array[stockno].b1) * tvx1) +
            (sqr( stock_array[stockno].b2) * tvx2) +
            (sqr( stock_array[stockno].b3) * tvx3) +
            (sqr( stock_array[stockno].e) * tvx4)) * 10.0);
  }




float normal(value)
float value;
  {
    int i,flag;
    float holder;

    flag = FALSE;
    value *= 10;
    if (value < 0)
      {
        value *= -1;
        flag = TRUE;
      }
    i = (int) value;
    if (i > 22)
      holder = .999;
    else
      holder = ((value - ((float) i)) * ndis[i + 1]) - ((value - ((float) (i + 1))) * ndis[i]);
    if (flag)
      holder = 1.0 - holder;

/*
fp = fopen("prn","w");
fprintf(fp,"value is %f i is %d\n",value,i);
fprintf(fp,"normal returns %f\n",holder);
fclose(fp);
*/

    return(holder);
  }

bonds(units)
int units;
  {
    if (units + ((int) players[player - 1].bonds) > 99)
      {
        message = TRUE;
        clear_line();
        sprintf(tempstr,"Maximum of 99 bonds can be held");
        TxWrite(rp,tempstr);
      }
    else if ((((float) units) * 1000.0) <= players[player - 1].cash)
      {
        players[player - 1].cash -= (((float) units) * 1000.0);
        upd_coh();
        players[player - 1].bonds += ((int) units);
        if (scr_status != RANKINGSUP)
          upd_bonds();
      }
    else
      {
        message = TRUE;
        clear_line();
        sprintf(tempstr,"INSUFFICIENT FUNDS");
        TxWrite(rp,tempstr);
      }
  }

init()
  {
    int i, x;
    int tries;


 /*   tries = 0;

    for (i = 0; i < 5; ++i)
      id[i] = '0';
    id[5] = '\0';
    chk();
    while ((tries < 3) && (strcmp(id,"00000") == 0))
      {
        chknum(id);
        ++tries;
      }
    if (strcmp(id,"00000") == 0)
      {
        sprintf(tempstr,"FATAL DISK ERROR\n");
        TxWrite(rp,tempstr);
        exit();
      }
    else if (strcmp(id,"10799") != 0)
      {
        printf("INVALID DISK IN USE\n");
        exit();
      }
    chkser(serial);
    cursor(22,9);
    printf("Serial No. #%-s",serial);

 */

/* initialize first quarter history */

    for (x=0; x < NO_OF_STOCKS; ++x)
      history[x].price[15] = 100;

/* link the ticker elements together */

    for (i=0; i < TICK_SIZE; ++i)
      {
        cur_tick = &ticker[0]+1;
        if (i != TICK_SIZE - 1)
          ticker[i].next = &ticker[0] + i + 1;
        else
          ticker[i].next = &ticker[0];
        if (i != 0)
          ticker[i].previous = &ticker[0] + i - 1;
        else
          ticker[i].previous = &ticker[0] + TICK_SIZE - 1;

        load_tick_element(&ticker[0] + i,i);

      }  /* for loop setting up ticker */

/* set up auto execute array and pointers */

    for (i=0; i < MAXPLAYERS; ++i)
        for (x = 0; x < AUTOLIMIT; ++x)
            players[i].auto_ptr[x] = &players[i].auto_array[0]+x;

/* set up screen array pointers */

    for (i=0; i < STOCKLIMIT; ++i)
      {
        scr_ptr[i] = &screen[0]+i;
      }


/* initialize timers, timer1 in use, timer2 not in use */

    timer1.status = OFF;
    timer2.status = OFF;
    timer3.status = OFF;
    timer1.hour_changed = FALSE;
    timer2.hour_changed = FALSE;
    timer3.hour_changed = FALSE;

    cursor(24,0);

  }  /* procedure initialize */




int get_int(infile)
FILE *infile;
  {
    int atoi();
    char innum[10],c;
    int i;

    while (((c = getc(infile)) == ' ') || (c == '\015') || (c== '\012'))
      ;
    innum[0] = c;
    i = 1;
    while (((c = getc(infile)) != ' ') && (c != '\015') && (c != '\012'))
      innum[i++] = c;
    innum[i] = '\0';
    return(atoi(innum));
  }

/* a procedure to write the credits etc. on the screen.  It pauses */
/* then clears the screen */

headers1()
  {
    int stall;
    char file_name[8];
    int yorn,forh;
    int temp_year;
    int junk1_counter, junk2_counter, junk3_counter;
    char c;

    cursor(7,7);
    sprintf(tempstr,"The Financial Time Machine");
    TxWrite(rp,tempstr);
    cursor(9,0);
    sprintf(tempstr,"(C) 1985 by Lehner Communications, Inc.");
    TxWrite(rp,tempstr);
    cursor(14,11);
    sprintf(tempstr,"programmed by NMc");
    TxWrite(rp,tempstr);
    cursor(16,6);
    sprintf(tempstr,"special thanks to Ed Friedman");
    TxWrite(rp,tempstr);
    Delay(300);
    yorn = (int)' ';
    while ((yorn != (int)'y') && (yorn != (int)'Y') && (yorn != (int)'n') && (yorn != (int)'N'))
      {
        clear_top();
        clear_bottom();
        cursor(10,7);
        sprintf(tempstr,"Continue A Previous Game?");
        TxWrite(rp,tempstr);
        cursor(12,13 );
        sprintf(tempstr,"(Type Y or N)");
        TxWrite(rp,tempstr);
        cursor(10,33);
re2:    yorn = kbhit();
        if ((yorn == (int)'Y') || (yorn == (int)'y'))
          {
            return(TRUE);
          }
        else if ((yorn == (int)'N') || (yorn == (int)'n'))
          {
            temp_year = 0;
            forh = ' ';
                clear_top();
                clear_bottom();
                cursor(10,0 );
                sprintf(tempstr,"Enter Historical Mode or Forecast Mode?");
                TxWrite(rp,tempstr);
                cursor(12,13 );
                sprintf(tempstr,"(Type H or F)");
                TxWrite(rp,tempstr);
            while ((forh != (int)'F') && (forh != (int)'f') && (forh != (int)'H') && (forh != (int)'h'))
              {
                cursor(24,0);
                forh = kbhit();
              }

            if ((forh == (int)'F') || (forh == (int)'f'))
              {
                temp_year = 1;
                emphasis = '\0';
                    clear_top();
                    clear_bottom();
                    cursor(6,7);
                    sprintf(tempstr,"Forecast Mode Emphasis On:");
                    TxWrite(rp,tempstr);
                    cursor(10,10);
                    sprintf(tempstr,"1) GNP Factors");
                    TxWrite(rp,tempstr);
                    cursor(12,10);
                    sprintf(tempstr,"2) Political Factors");
                    TxWrite(rp,tempstr);
                    cursor(14,10);
                    sprintf(tempstr,"3) Interest Rate Factors");
                    TxWrite(rp,tempstr);
                    cursor(18,12);
                    sprintf(tempstr,"Type 1, 2, or 3");
                    TxWrite(rp,tempstr);
                while ((emphasis < '1') || (emphasis > '3'))
                  {
                    cursor(24,0);
                    emphasis = kbhit();
                  }

                switch(emphasis)
                  {
                    case '1': getstd("gnp.dat");
                              break;
                    case '2': getstd("pol.dat");
                              break;
                    default : getstd("int.dat");
                              break;
                  }

              }
            else
              {
                getstd("std.dat");
                while ((temp_year < 1930) || (temp_year > 1980))
                  {
                    clear_top();
                    clear_bottom();
                    cursor(10,10);
                    sprintf(tempstr,"Enter The Game Year ");
                    TxWrite(rp,tempstr);
                    cursor(12,13);
                    sprintf(tempstr,"(1930 to 1980)");
                    TxWrite(rp,tempstr);
                    cursor(14,17);
                    sprintf(tempstr,"> ");
                    TxWrite(rp,tempstr);
                    stall = 0;
                    while (stall < 5)
                      {
                        c = kbhit();
                        if ((isdigit(c)) && (stall < 4))
                          {
                            file_name[stall++] = c;
                            sprintf(tempstr,"%c",c);
                            TxWrite(rp,tempstr);
                          }
                        if ((c == BACKSP) && (stall != 0))
                          {
                            --stall;
                            cursor(14,18+stall);
                            Text(rp," ",1);
                            cursor(14,18+stall);
                          }
                        if ((c == CR) && (stall == 4))
                          stall = 5;
                      }
                    temp_year = 0;
                    file_name[5] = '\0';
                    temp_year = atoi(file_name);
                  }
              }
          }
        else goto re2;
        year = temp_year;
        clear_top();
        clear_bottom();
      }

    return(FALSE);
  }




getstd(infile)
char *infile;
  {
    float f1,f2,f3,f4,get_float();
    FILE *fopen(), *in;
    int i,x;
    char *fgets();

    in = fopen(infile,"r");
    for (i=0; i <30; ++i)
      {
        fgets(std_mssgs.item[i].string,75,in);
        std_mssgs.item[i].intr = get_float(in);
        std_mssgs.item[i].gnp  = get_float(in);
        std_mssgs.item[i].pol  = get_float(in);
        std_mssgs.item[i].var  = get_float(in);

      }
    fclose(in);

  }



float get_float(infile)
FILE *infile;
  {
    extern double atof();
    char innum[20],c;
    int i;

    while (((c = getc(infile)) == ' ') || (c == '\015') || (c == '\012'))
      ;
    innum[0] = c;
    i = 1;
    while (((c = getc(infile)) != ' ') && (c != '\015') && (c != '\012'))
      innum[i++] = c;
    innum[i] = '\0';
    return((float)atof(innum));
  }
