#include "amiga.h"
#include "extern.h"
#include <stdlib.h>
getsave()
  {
    FILE *saved;
    AUTOS *aptr;
    int stock,shares,price;
    int num,junk,tyear,player,counter,tweek;
    char *fgets();
    float get_float();

    saved = fopen("save","r");
    if (saved == NULL)
      {
        cursor(24,0);
        sprintf(tempstr,"NO SAVED GAME FOUND");
        TxWrite(rp,tempstr);
        exit(1);
      }
    else
      {
        clear_top();
        clear_bottom();
        no_of_players = get_int(saved);
        year = get_int(saved);
        quarter = (int) get_int(saved);
        week = (int) get_int(saved);
        if (year < 10)
          emphasis = (char) get_int(saved);
        prime_rate = get_float(saved);
        q_int_rate = get_float(saved);
        wk_int_rate = get_float(saved);
        bond_rate = get_float(saved);

        vx1 = get_float(saved);
        vx2 = get_float(saved);
        vx3 = get_float(saved);
        vx4 = get_float(saved);

        for (player = 0; player < no_of_players; ++player)
          {
            players[player].name_length = (int) get_int(saved);
            fgets(players[player].name,21,saved);
            players[player].name[players[player].name_length] = '\0';
            players[player].cash        = get_float(saved);
            players[player].net_worth   = get_float(saved);
            players[player].taxes       = get_float(saved);
            players[player].short_term  = get_float(saved);
            players[player].long_term   = get_float(saved);
            players[player].other_earnings = get_float(saved);
            players[player].bonds       = (int) get_int(saved);
            players[player].stock_count = (int) get_int(saved);
            players[player].auto_count = (int) get_int(saved);

            for (counter = 0; counter < players[player].stock_count; ++counter)
              {
                stock = (int) get_int(saved);

                players[player].portfolio[stock].shares = (int) get_int(saved);
                players[player].portfolio[stock].margin_debt = get_float(saved);
                num = get_int(saved);
                tyear = year;
                tweek = week;
                for (junk = 0;junk < num; ++junk)
                  {
                    year   = get_int(saved);
                    week   = (int) get_int(saved);
                    shares = (int) get_int(saved);
                    price  = (int) get_int(saved);
                    add_purchase(player + 1,stock,shares,price);
                  }
                year = tyear;
                week = tweek;
              }
            for (counter = 0; counter < players[player].auto_count; ++counter)
              {
                aptr = players[player].auto_ptr[counter];
                aptr->minus_price  = (int) get_int(saved);
                aptr->plus_price   = (int) get_int(saved);
                aptr->option_type  = (int) get_int(saved);
                aptr->option_price = (int) get_int(saved);
                aptr->units        = (int)  get_int(saved);
                aptr->stock_no     = (int) get_int(saved);
                aptr->cur_price    = (int) get_int(saved);
                aptr->pur_price    = get_int(saved);
              }
          }
        for (counter = 0; counter < NO_OF_STOCKS; ++counter)
          {
            stock_array[counter].price = (int) get_int(saved);
            for (junk = 0; junk < 20; ++junk)
              history[counter].price[junk]  = (int) get_int(saved);
            for (junk = 0; junk < 20; ++junk)
              history[counter].splits[junk] = (int) get_int(saved);
          }
        fclose(saved);
        unlink("save");
        load_screen();
        screen_upd();
        load_tick_element(&ticker[0],0);
        load_tick_element(&ticker[0]+1,1);
        load_tick_element(&ticker[0]+2,2);
      }

  }






headers2()
  {
    char inmoney[5];
    int money;
    int junk1_counter, junk2_counter, junk3_counter;
    char c;
    char *fgets();


    clear_top();
    clear_bottom();
    cursor(10,6);
    sprintf(tempstr,"How Many Will Be Playing?");
    TxWrite(rp,tempstr);
    re: c = kbhit();
    no_of_players = c;
    if (isdigit(no_of_players))
      no_of_players -= DIFFERENCE;
    else
      no_of_players = -1;

    if ((no_of_players <= 0) || (no_of_players > 4))
      goto re;
    for (junk1_counter = 0; junk1_counter < no_of_players; ++junk1_counter)
      {
        clear_top();
        clear_bottom();
        cursor(10,9);
        sprintf(tempstr,"Enter Player %d's Name",junk1_counter + 1);
        TxWrite(rp,tempstr);
        cursor(12,6);
        sprintf(tempstr,"(Maximum Of 20 Characters)");
        TxWrite(rp,tempstr);
        cursor(14,6);
        sprintf(tempstr,"> ");
        TxWrite(rp,tempstr);
        junk3_counter = -1;
        while (junk3_counter < 20)
          {
            char c;

            c = (char) kbhit();
            if (c == NULL) goto re2;
            if (isupper(c) || islower(c) || (c == ' ') || (c == '.') || (c == ','))
              {
                ++junk3_counter;
                if (junk3_counter == 20)
                  {
                    DisplayBeep();
                    --junk3_counter;
                  }
                else
                  {
                    players[junk1_counter].name[junk3_counter] = c;
                    sprintf(tempstr,"%c",c);
                    TxWrite(rp,tempstr);
                  }
              }
            else if (c == (char) CR)
              {
                players[junk1_counter].name[++junk3_counter] = '\0';
                junk3_counter = 20;
              }
            else if (c == BACKSP)
              {
                if (junk3_counter >= 0)
                  {
                    --junk3_counter;
                    cursor(14,9+junk3_counter);
                    Text(rp," ",1);
                    cursor(14,9+junk3_counter);
                  }
              }
            else if (c != NULL)
              {
                DisplayBeep();
              }
re2:       c = c;
          }
        junk2_counter = 0;
        while (players[junk1_counter].name[junk2_counter] != '\0')
          ++junk2_counter;
        if (junk2_counter > 20)
          {
          junk2_counter = 20;
          players[junk1_counter].name[19] = '\0';
          }
        players[junk1_counter].name_length = junk2_counter;

      }
    money = 0;
    while ((money != 25) && (money != 100) && (money != 50))
      {
        junk2_counter = 0;
        clear_top();
        clear_bottom();
        cursor(10,5);
        sprintf(tempstr,"How Much Money Would You Like?");
        TxWrite(rp,tempstr);
        cursor(11,13);
        sprintf(tempstr,"(In Thousands)");
        TxWrite(rp,tempstr);
        cursor(13,10);
        sprintf(tempstr,"Enter 25, 50, or 100");
        TxWrite(rp,tempstr);
        cursor(15,18);
        sprintf(tempstr,"> ");
        TxWrite(rp,tempstr);
        c = ' ';
        while (c != CR)
          {
            c = kbhit();
            if ((isdigit(c)) && (junk2_counter < 3))
              {
                inmoney[junk2_counter++] = c;
                inmoney[junk2_counter] = '\0';
                sprintf(tempstr,"%c",c);
                TxWrite(rp,tempstr);
              }
            else if ((c == BACKSP) && (junk2_counter != 0))
              {
                inmoney[junk2_counter--] = '\0';
                cursor(15,20+junk2_counter);
                Text(rp," ",1);
                cursor(15,20+junk2_counter);
              }
            else if (c == CR)
              money = atoi(inmoney);
          }
      }

    for (junk1_counter = 0;junk1_counter < no_of_players;++junk1_counter)
      {
        players[junk1_counter].cash = money * 1000.0;
        players[junk1_counter].net_worth = players[junk1_counter].cash;
        players[junk1_counter].stock_count = 0;
      }
    clear_top();
    clear_bottom();
  }
getyear(year)
int year;
  {
    float f1,f2,f3,f4,get_float();
    FILE *fopen(), *in;
    int i,x,counter,found;
    char *fgets(),c;
    char datfile[11];
    double sqr();

    x = 1000;
    counter = 0;
    found = FALSE;
    while (x > 0)
      {
        i = year / x;
        if (i > 0)
          found = TRUE;
        year -= (i * x);
        x /= 10;
        if (found)
          datfile[counter++] = i + DIFFERENCE;
      }
    datfile[counter++] = '.';
    datfile[counter++] = 'd';
    datfile[counter++] = 'a';
    datfile[counter++] = 't';
    datfile[counter++] = '\0';


    in = fopen(datfile,"r");
    y_mssg_count = 0;
    i = 0;

    high_prime = get_float(in);
    low_prime = get_float(in);
    x = get_int(in);

    if (quarter > 2)
      {
        for (i = 0; i < (2 * x); ++i)
          {
            while (getc(in) !=  '\n')
              ;
          }
        x = get_int(in);
      }
    y_mssg_count = x;

    for  (i = 0; i < x; ++i)
      {
        fgets(yrly_mssgs.item[i].string,82,in);
        yrly_mssgs.item[i].intr = get_float(in);
        yrly_mssgs.item[i].gnp  = get_float(in);
        yrly_mssgs.item[i].pol  = get_float(in);
        yrly_mssgs.item[i].var  = get_float(in);

        yrly_mssgs.item[i].used = FALSE;

/*
fp = fopen("prn","w");
fprintf(fp,"i is %d\n",i);
fprintf(fp,"message %s \n",yrly_mssgs.item[i].string);
fclose(fp);
*/

      }

    if (quarter == 1)
      {
        vx1 = vx2 = vx3 = vx4 = 0;
        for (i = 0; i < y_mssg_count; ++i)
          {
            vx1 += sqr(yrly_mssgs.item[i].intr);
            vx2 += sqr(yrly_mssgs.item[i].gnp);
            vx3 += sqr(yrly_mssgs.item[i].pol);
            vx4 += sqr(yrly_mssgs.item[i].var - 1.0);
          }
        x = get_int(in);
        for (i = 0; i < x; ++i)
          {

            while (getc(in) !=  '\n')
              ;
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

/*
fp = fopen("prn","w");
fprintf(fp,"final messcount is %d\n",y_mssg_count);
fclose(fp);
*/


  }





/***************************************************/
/************  begin main body of event()  *********/
/***************************************************/
static int outc = 1;
static float x4,E,R;
static float gamma1 = 1.2;
static float gamma2 = .24;
static float gamma3 = .24;

event(stockno,news)
int stockno;
float news;
{


     int i,x,y;
     float myrandom();
     int newshares;
     NODE *temp;



      if (stockno != MMMS)
        {



          x4 =  (( ((float) gethundredths()) - 50.0) / 5000.0) * news * stock_array[stockno].e;


/* calculated expected rate of return using method 2 as outlined */


          E = ((gamma1 * stock_array[stockno].b1) +
               (gamma2 * stock_array[stockno].b2) +
               (gamma3 * stock_array[stockno].b3)) * the_interest;




/* generate percent change in stock price   */


          R =  (((stock_array[stockno].b1 * x1) +
                 (stock_array[stockno].b2 * x2) +
                 (stock_array[stockno].b3 * x3) +
                 (stock_array[stockno].e  * x4) + E) * 1.15) + 1.0;



/* check value of R to prevent negative stock prices */

          if (R < .01)
            R = .01;


/* calculate new stock price  */


          stock_array[stockno].price =  (int) round((stock_array[stockno].price * R));

          if (stock_array[stockno].price >= SPLITPRICE)
            {
              history[stockno].splits[quarter + 15] = 1;
              stock_array[stockno].price /= 2;
              clear_line();
              sprintf(tempstr,"%s STOCK SPLITS 2 FOR 1", stock_array[stockno].name);
              TxWrite(rp,tempstr);
              cursor(24,0);
              message = TRUE;
              for (y = 0; y < no_of_players; ++y)
                {
                  newshares = (int) players[y].portfolio[stockno].shares;

                  if ( ((int) players[y].portfolio[stockno].shares) >= 5000)
                    {
                      players[y].portfolio[stockno].shares *= 2;
                      players[y].cash += ( ((float) players[y].portfolio[stockno].shares) - 9999.0) *
                       ((float) stock_array[stockno].price);
                      players[y].other_earnings += ( ((float) players[y].portfolio[stockno].shares) - 9999.0) *
                       ((float) stock_array[stockno].price);
                      players[y].portfolio[stockno].shares = (int) 9999;
                    }
                  else
                    players[y].portfolio[stockno].shares *= 2;

                    newshares = players[y].portfolio[stockno].shares - newshares;

                  if ( ((int) players[y].portfolio[MMMS].shares) > 0)
                    {
                      players[y].cash += ((float) players[y].portfolio[MMMS].shares) * 333.0;
                      players[y].other_earnings += ((float) players[y].portfolio[MMMS].shares) * 333.0;
                    }


                   /*** adjust purchase record for splits *****/


                  temp = players[y].portfolio[stockno].purchases;

             if (temp != NULL)
                {
                  while (temp->next !=  NULL)
                    temp = temp->next;
                }

                  while (temp != NULL)
                    {
                      if ( ((int) temp->shares) < newshares)
                        {
                          newshares -= (int) temp->shares;
                          temp->shares *= 2;
                        }
                      else
                        {
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

        }
      else
        {
          stock_array[MMMS].price = 0;
          x = 0;

          for (i=0; i < NO_OF_STOCKS; ++i)
            {
              x += stock_array[i].price;
            }

          x -= stock_array[GOLD].price;
          x -= stock_array[REALESTATE].price;
          stock_array[MMMS].price = x / (NO_OF_STOCKS - 3);
        }

  }  /* end procedure event() */





static float pi2 = 6.28;

float myrandom(stdev)
float stdev;
  {

    float holder;
    float frand();
    double cos(),sqrt(),log();


    holder = (sqrt(-2.0 * log(frand())) * (cos (pi2 * frand()))) * stdev;
    return(holder);
  }


stall(x)
int x;
  {
    int staller;
    int flag,y,z;

    flag = FALSE;
    staller = y = gethundredths();
    if (x == LONG)
      staller += 10;
    else
      staller += 8;

    if (staller > 100)
      {
         staller -= 99;
         flag = TRUE;
      }
    while (((z = gethundredths()) < staller) || (flag))
      if (z < y)
        {
          z = y;
          flag = FALSE;
          if (staller > 90)
            staller = 0;
        }


  }
save()
  {
    int player,stock,temp,count;
    FILE *saveto;
    AUTOS *aptr;
    NODE *ptr;

    saveto = fopen("save","w");
    if (saveto == NULL)
      {
        message = TRUE;
        clear_line();
          sprintf(tempstr,"FAILED ATTEMPT TO SAVE");
          TxWrite(rp,tempstr);
      }
    else
      {
        fprintf(saveto," %d %d %d %d ",no_of_players,year,quarter,week);
        if (year < 10)
          fprintf(saveto," %d ",(int) emphasis);
        fprintf(saveto,"%6.4f %6.4f %6.4f %6.4f\n",prime_rate,q_int_rate,wk_int_rate,bond_rate);
        fprintf(saveto,"%8.6f %8.6f %8.6f %8.6f\n", vx1, vx2, vx3, vx4);
        for (player = 0; player < no_of_players; ++player)
          {
            fprintf(saveto," %d %-20s\n",players[player].name_length,players[player].name);
            fprintf(saveto,"%f %f %f\n",players[player].cash,players[player].net_worth,players[player].taxes);
            fprintf(saveto,"%f %f %f\n",players[player].short_term,players[player].long_term,
             players[player].other_earnings);
            temp = fprintf(saveto,"%d %d %d\n",players[player].bonds,players[player].stock_count,
             players[player].auto_count);

            for (stock = 0; stock < NO_OF_STOCKS; ++stock)
              {
                if (players[player].portfolio[stock].shares > 0)
                  {
                    temp = fprintf(saveto,"%d %d %f\n",stock,players[player].portfolio[stock].shares,
                           players[player].portfolio[stock].margin_debt);
                    count = 1;
                    ptr = players[player].portfolio[stock].purchases;
                    while (ptr->next != NULL)
                      {
                        ++count;
                        ptr = ptr->next;
                      }
                    fprintf(saveto,"%d\n",count);
                    ptr = players[player].portfolio[stock].purchases;
                    while (ptr != NULL)
                      {
                        fprintf(saveto,"%d %d %d %d\n",ptr->year,ptr->week,ptr->shares,ptr->price);
                        ptr = ptr->next;
                      }
                  }
              }
            if (temp == -1)
              goto jump1;
            for (count = 0; count < players[player].auto_count; ++count)
              {
                aptr = players[player].auto_ptr[count];
                fprintf(saveto," %d %d %d ",aptr->minus_price,aptr->plus_price,aptr->option_type);
                fprintf(saveto,"%d %d %d %d %d\n",aptr->option_price,aptr->units,aptr->stock_no,aptr->cur_price,
                 aptr->pur_price);
              }
          }

        for (stock = 0; stock < NO_OF_STOCKS; ++stock)
          {
            fprintf(saveto,"%d\n",stock_array[stock].price);
            for (count = 0; count < 20; ++count)
              fprintf(saveto,"%d\n",history[stock].price[count]);
            for (count = 0; count < 20; ++count)
              fprintf(saveto,"%d\n",history[stock].splits[count]);
          }
        temp = fclose(saveto);
        jump1:
        if (temp == -1)
          {
            message = TRUE;
            clear_line();
              sprintf(tempstr,"FAILED ATTEMPT TO SAVE");
              TxWrite(rp,tempstr);
          }
        else
          {
            clear_line();
            sprintf(tempstr,"GAME OVER");
            TxWrite(rp,tempstr);
            shutdown();
          }
      }
  }
