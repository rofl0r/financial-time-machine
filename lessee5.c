#include "amiga.h"
#include "extern.h"


compare (start,finish,com_array,method)
  int start;
  int finish;
  char *com_array;
  int method;

    {
      int counter;
      int case_count;
      int cases;
      char *cur_case;     /* pointer to the stock or command name array */
      int possible;       /* the number of possible matches found */
      int holder;         /* holds the last match found */

      possible = 0;
      if (method == COMMAND)
        cases = NO_OF_COMMANDS - 1;
      else
        cases = NO_OF_STOCKS - 1;
      for (case_count = 0;case_count <= cases;++case_count)
        {
          counter = start;
          if (method == COMMAND)
            cur_case = commands[case_count];
          else                                  /* method is STOCK */
            cur_case = stock_array[case_count].name;


          while ((counter <= finish) && (com_array[counter] == cur_case[counter-start]) &&
                 (cur_case[counter-start] != '\0'))
            {
              ++counter;
            }

      /* if we found an exact match */

          if ((cur_case[counter - start] == '\0') && (counter > finish))
            return(case_count);

     /* if we found a possible match */

          if (counter > finish)
            {
              ++possible;
              holder  = case_count;
            }

        }
     if (possible == 1)
        return(holder);
      else
        return(NOT_FOUND);
    }

int set_top_screen()
{
  int junk;

  upd_quarter();

  cursor(0,27);
  sprintf(tempstr,"PRIME");
  TxWrite(rp,tempstr);
  upd_prime();

  upd_year();

/************************************************************************/
/********** begin assembler setup of screen characteristics *************/
/************************************************************************/

SetAPen(rp,2);
RectFill(rp,0,8,320,31);

SetAPen(rp,3);
RectFill(rp,0,32,320,55);

SetAPen(rp,1);

/************************************************************************/
/*********** end assembler setup of screen characteristics **************/
/************************************************************************/



}


set1_bottom_screen()
  {  int x;

    cursor(8,0);
    sprintf(tempstr,"HOLDINGS");
    TxWrite(rp,tempstr);
    cursor(8,34);
    sprintf(tempstr,"PAGE 1");
    TxWrite(rp,tempstr);
    x = (int) ((39 - (int)cur_player->name_length) / 2);
    cursor(8,x);
    sprintf(tempstr,"%s", cur_player->name);
    TxWrite(rp,tempstr);
    cursor(10,1);
    sprintf(tempstr," S  LOTS   VALUE   CP    MARGIN  LIMIT");
    TxWrite(rp,tempstr);
    cursor(22,0);
    sprintf(tempstr,"COH $");
    TxWrite(rp,tempstr);
    cursor(22,17);
    sprintf(tempstr,"NETW $");
    TxWrite(rp,tempstr);
    cursor(22,36);
    sprintf(tempstr,"T-");
    TxWrite(rp,tempstr);
    upd_bonds();
    upd_netw();
    upd_coh();


/************************************************************************/
/********** begin assembler setup of screen characteristics *************/
/************************************************************************/

SetAPen(rp,5);
/* Draw appropriate rectangles */
Move(rp,4,92);
Draw(rp,4,172);
Draw(rp,36,172);
Draw(rp,36,76);
Draw(rp,4,76);
Draw(rp,4,92);
Draw(rp,36,92);
Draw(rp,36,172);
Draw(rp,76,172);
Draw(rp,76,76);
Draw(rp,36,76);
Draw(rp,36,92);
Draw(rp,76,92);
Draw(rp,76,172);
Draw(rp,156,172);
Draw(rp,156,76);
Draw(rp,76,76);
Draw(rp,76,92);
Draw(rp,156,92);
Draw(rp,156,172);
Draw(rp,188,172);
Draw(rp,188,76);
Draw(rp,156,76);
Draw(rp,156,92);
Draw(rp,188,92);
Draw(rp,188,172);
Draw(rp,268,172);
Draw(rp,268,76);
Draw(rp,188,76);
Draw(rp,188,92);
Draw(rp,268,92);
Draw(rp,268,172);
Draw(rp,316,172);
Draw(rp,316,76);
Draw(rp,268,76);
Draw(rp,268,92);
Draw(rp,316,92);
SetAPen(rp,1);

/************************************************************************/
/*********** end assembler setup of screen characteristics **************/
/************************************************************************/

  }



set2_bottom_screen()
  { int x;

    cursor(8,0);
    sprintf(tempstr,"HOLDINGS");
    TxWrite(rp,tempstr);
    cursor(8,34);
    sprintf(tempstr,"PAGE 2");
    TxWrite(rp,tempstr);
    x = (39 - (int)cur_player->name_length) / 2;
    cursor(8,x);
    sprintf(tempstr,"%s", cur_player->name);
    TxWrite(rp,tempstr);
    cursor(10,1);
    sprintf(tempstr," S  LOTS   VALUE   OPTION CP  BUY SELL");
    TxWrite(rp,tempstr);
    cursor(22,0);
    sprintf(tempstr,"COH $");
    TxWrite(rp,tempstr);
    cursor(22,17);
    sprintf(tempstr,"NETW $");
    TxWrite(rp,tempstr);
    cursor(22,36);
    sprintf(tempstr,"T-");
    TxWrite(rp,tempstr);
    upd_bonds();
    upd_netw();
    upd_coh();


/************************************************************************/
/********** begin assembler setup of screen characteristics *************/
/************************************************************************/

SetAPen(rp,5);
/* Draw appropriate rectangles */

Move(rp,4,92);
Draw(rp,4,172);
Draw(rp,36,172);
Draw(rp,36,76);
Draw(rp,4,76);
Draw(rp,4,92);
Draw(rp,36,92);
Draw(rp,36,172);
Draw(rp,76,172);
Draw(rp,76,76);
Draw(rp,36,76);
Draw(rp,36,92);
Draw(rp,76,92);
Draw(rp,76,172);
Draw(rp,156,172);
Draw(rp,156,76);
Draw(rp,76,76);
Draw(rp,76,92);
Draw(rp,156,92);
Draw(rp,156,172);
Draw(rp,212,172);
Draw(rp,212,76);
Draw(rp,156,76);
Draw(rp,156,92);
Draw(rp,212,92);
Draw(rp,212,172);
Draw(rp,244,172);
Draw(rp,244,76);
Draw(rp,212,76);
Draw(rp,212,92);
Draw(rp,244,92);
Draw(rp,244,172);
Draw(rp,276,172);
Draw(rp,276,76);
Draw(rp,244,76);
Draw(rp,244,92);
Draw(rp,276,92);
Draw(rp,276,172);
Draw(rp,316,172);
Draw(rp,316,76);
Draw(rp,276,76);
Draw(rp,276,92);
Draw(rp,316,92);
SetAPen(rp,1);
/************************************************************************/
/*********** end assembler setup of screen characteristics **************/
/************************************************************************/

  }

upd_com_line(com_no,units,stock_no,price,autobuy,autosell,com_array,com_char_count)
int com_no,units,stock_no,price,autobuy,autosell;
char *com_array;
int *com_char_count;
  {
    int divisor, variation;
    int counter, i, pr;
    int found_first;                    /* flag for finding non-zero int */


    i = 1;
    while ((commands [com_no] [i-1] != '\0') &&
           (commands [com_no] [i-1] != ' '))
      {
      com_array [i] = commands [com_no] [i-1];
      ++i;
      }
    if ((com_no == QUIT) || (com_no == SAVE))
      {
        --i;
        goto finish;
      }
    else
      {
        com_array [i] = ' ';
      }

  /*  put in the number of units, in characters  */
    if (com_no != GRAPH)
      {
        divisor = 1000;
        found_first = FALSE;
        if (units == 0)
          com_array[++i] = '0';
        else
          {
            while (divisor  >= 1)
              {
                if ((!found_first) && ((units / divisor) >= 1))
                  found_first = TRUE;
                if (found_first)
                  {
                     com_array [++i] = (int) (units /divisor) + DIFFERENCE;
                     units = units - (((int) units /divisor) * divisor);
                  }
                divisor = divisor /10;
              }
          }
      }
    if ((com_no == DELETE) || (com_no == BONDS) || (com_no == CASH) || (com_no == JUMP))
      {
        if (com_no == JUMP)
     {
       com_array[++i] = ' ';
       com_array[++i] = 'W';
       com_array[++i] = 'E';
       com_array[++i] = 'E';
       com_array[++i] = 'K';
       com_array[++i] = 'S';
     }
        *com_char_count = i;
        goto finish;
      }
    else
      {
        com_array [++i] = ' ';
      }

  /* put in the stock name */

    if (com_no != EXERCISE)
      {
        for (counter = 0;stock_array[stock_no].name[counter] != '\0'; ++counter)
          com_array[++i] = stock_array[stock_no].name[counter];
      }

     if ((com_no == GRAPH) || (com_no == LIMIT))
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
    switch(variation)
      {
        case 1:  if (price != NOT_FOUND)
                   {
                     pr = price;
                   }
                 break;
        case 2:  if (autobuy != NONE)
                   {
                     com_array[++i] = '-';
                     pr = autobuy;
                   }
                 break;
        case 3:  if (autosell != NONE)
                   {
                     com_array[++i] = '+';
                     pr = autosell;
                   }
                 break;
      }

    if (pr != NOT_FOUND)
      {
        divisor = 1000;
        found_first = FALSE;
        while (divisor  >= 1)
          {
            if ((!found_first) && ((pr / divisor) >= 1))
              found_first = TRUE;
            if (found_first)
              {
                 com_array [++i] = (int) (pr /divisor) + DIFFERENCE ;
                 pr = pr - (((int) pr /divisor) * divisor);
              }
            divisor = divisor /10;

          }
        com_array[++i] = ' ';
      }

        ++variation;

        if (variation <= 3)
          goto nextcase;

finish:

    *com_char_count = i;
    cursor(24,0);
    for (counter = 1; counter <= i; counter++)
      {
      sprintf(tempstr,"%c",com_array[counter]);
      TxWrite(rp,tempstr);
      }
    }  /* end procedure up_com_line() */

execute(com_no,units,stock_no,price,auto_minus,auto_plus,player)
int com_no,units,stock_no,price,auto_minus,auto_plus,player;
  {
    AUTOS *ptr;
    PLAYER *play_ptr;
    int counter,tplayer;
    float opt_value();


    switch (com_no)
    {
      case  BUY:  if (price == NOT_FOUND)
                    {
                      if (players[player - 1].auto_count < AUTOLIMIT )
                        {
                          add_auto_exec(auto_minus,auto_plus,AUTOBUY,NONE,units,stock_no,player,NOT_FOUND);
                        }

                      else
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE %d AUTO TRANSACTIONS",AUTOLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }

                    }
                  else if (round(((float)units) * ((float) price) * 101.5) <= players[player - 1].cash)
                    {
                      if ((players[player - 1].portfolio[stock_no].shares != 0) ||
                          (players[player - 1].stock_count < STOCKLIMIT))
                        {
                  /* add a record node for tax purposes */

                          add_purchase(player,stock_no,units,price);

                  /* if its a new stock, note that his portfolio has grown */

                          if (players[player - 1].portfolio[stock_no].shares == 0)
                            ++players[player - 1].stock_count;

                  /* charge the player */

                          players[player - 1].cash -=  round(((float) units) * ((float) price) * 101.5);

                  /* give him the stock */

                          players[player - 1].portfolio[stock_no].shares += units;

                          upd_coh();
                          upd_stock(stock_no);

                        }
                      else   /* reached limit of holdings */
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE %d STOCKS",STOCKLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }

                    }  /* end if enough cash */

                  else /* too little cash */
                    {
                      sprintf(tempstr,"YOU HAVEN'T GOT ENOUGH CASH");
                      TxWrite(rp,tempstr);
                      message = TRUE;
                    }

                  break;

      case SELL:  if (price == NOT_FOUND)
                    {
                      if (players[player - 1].auto_count < AUTOLIMIT)
                        {
                          add_auto_exec(auto_minus,auto_plus,AUTOSELL,NONE,units,stock_no,player,NOT_FOUND);
                        }

                      else
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE %d AUTO TRANSACTIONS",AUTOLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }

                    }

                  else if ((players[player - 1].portfolio[stock_no].shares >= units) &&
                           (units > 0))
                    {

                  /* pay the player for the stock he sold, first deducting */
                  /* any margin debt owed on those shares.                 */

                      if (round((double) players[player - 1].portfolio[stock_no].margin_debt) >= round(((float) units) *
                         ((float) stock_array[stock_no].price * 98.5))) 
                        players[player - 1].portfolio[stock_no].margin_debt -= round(98.5 * ((float) units) *
                         ((float) stock_array[stock_no].price));
                      else
                        {
                          players[player - 1].cash += round(((float) units) * ((float) stock_array[stock_no].price) *
                           98.5) - round((double) players[player - 1].portfolio[stock_no].margin_debt);
                          players[player - 1].portfolio[stock_no].margin_debt = 0;
                        }

                      if (round((double) players[player - 1].portfolio[stock_no].margin_debt) < 1)
                        players[player - 1].portfolio[stock_no].margin_debt = 0;

                  /* remove or update the tax record node */

                      del_purchase(player,stock_no,units);

                  /* remove the shares from his portfolio */

                      players[player - 1].portfolio[stock_no].shares -= units;

                  /* if he sold all his shares note his fewer holdings */

                      if ((players[player - 1].portfolio[stock_no].shares == 0) &&
                          (players[player - 1].portfolio[stock_no].margin_debt == 0))
                        {
                          players[player - 1].portfolio[stock_no].limit = 0;
                          --players[player - 1].stock_count;
                        }

                      upd_coh();
                      upd_stock(stock_no);
                    }

                  else    /* if he doesn't have that many shares */
                    {
                      if (players[player - 1].portfolio[stock_no].shares == 0)
                        {
                        sprintf(tempstr,"YOU DON'T OWN ANY!");
                        TxWrite(rp,tempstr);
                        }
                      else
                        {
                        sprintf(tempstr,"YOU DON'T HAVE THAT MANY SHARES");
                        TxWrite(rp,tempstr);
                        }
                      message = TRUE;
                    }

                  break;

      case CALL:  if (round(((float) price) * ((float) units) * 1.015) <= players[player - 1].cash)
                    {
                      if (players[player - 1].auto_count < AUTOLIMIT)
                        {
                          add_auto_exec(NONE,NONE,CALL,stock_array[stock_no].price,units,stock_no,player,price);
                          players[player - 1].cash -= round(((float) price) * ((float) units) * 1.015);
                          upd_coh();
                          upd_netw();

                        }
                      else
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE  %d AUTO TRANSACTIONS",AUTOLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }
                    }
                  else     /* if he hasn't got the money */
                    {
                      sprintf(tempstr,"YOU NEED SOME MORE CASH");
                      TxWrite(rp,tempstr);
                      message = TRUE;
                    }
                  break;

      case  PUT:  if (round(((float) price) * ((float) units) * 1.015) <= players[player - 1].cash)
                    {
                      if (players[player - 1].auto_count < AUTOLIMIT)
                        {
                          add_auto_exec(NONE,NONE,PUT,stock_array[stock_no].price,units,stock_no,player,price);
                          players[player - 1].cash -= round(((float) price) * ((float) units) * 1.015);
                          upd_coh();
                          upd_netw();

                        }
                      else
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE  %d AUTO TRANSACTIONS",AUTOLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }
                    }
                  else     /* if he hasn't got the money */
                    {
                      sprintf(tempstr,"YOU NEED SOME MORE CASH");
                      TxWrite(rp,tempstr);
                      message = TRUE;
                    }
                  break;

   case  MARGIN:  if (round((((float) units) * ((float) price) * 103.0) / 2.0) <= players[player - 1].cash)
                    {
                      if ((players[player - 1].portfolio[stock_no].shares != 0) ||
                          (players[player - 1].stock_count < STOCKLIMIT))
                        {

                  /* update or add a tax record node */

                          add_purchase(player,stock_no,units,price);

                  /* if its a new stock, note that his portfolio has grown */

                          if (players[player - 1].portfolio[stock_no].shares == 0)
                            ++players[player - 1].stock_count;

                  /* charge the player half the cost */

                          players[player - 1].cash -= round((((float) units) * ((float) price) * 103.0) / 2.0);

                  /* put the other half on margin */

                          players[player - 1].portfolio[stock_no].margin_debt += round((100.0 * ((float) units) *
                           ((float) price)) / 2.0);

                  /* give him the stock */

                          players[player - 1].portfolio[stock_no].shares += ((int) units);

                          upd_coh();
                          upd_stock(stock_no);

                        }
                      else   /* reached limit of holdings */
                        {
                          sprintf(tempstr,"YOU ALREADY HAVE %d STOCKS",STOCKLIMIT);
                          TxWrite(rp,tempstr);
                          message = TRUE;
                        }

                    }  /* end if enough cash */

                  else /* too little cash */
                    {
                      sprintf(tempstr,"YOU HAVEN'T GOT ENOUGH CASH");
                      TxWrite(rp,tempstr);
                      message = TRUE;
                    }

                  break;

    case DELETE:  del_auto(units - 1,player);
                  break;

      case QUIT:  quit();
                  break;

     case BONDS:  bonds(units);
                  break;

     case SAVE:   save();
                  break;

      case CASH:  cur_player->cash += ((float) units) * 1000.0;
                  cur_player->bonds -= units;
                  upd_coh();
                  upd_bonds();
                  break;
     case GRAPH:  graph(stock_no,year);
                  if (newsline[cur_news_line] != '\n')
                    cur_news_line = 0;
                  break;

  case EXERCISE:  ptr = cur_player->auto_ptr[units - 1];
                  if ((ptr->option_type == CALL) || (ptr->option_type == PUT))
                    {
                      price = (int) round(.985 * ((float) price));
                      cur_player->cash += (((float) price) * ((float) ptr->units));
                      cur_player->other_earnings += ((((float) price) - ((float) ptr->pur_price)) *
                       ((float) ptr->units));
                      del_auto(units - 1,player);
                      upd_coh();
                    }
                  else
                    {
                      message = TRUE;
                      sprintf(tempstr,"You Can't Exercise a Limit Order");
                      TxWrite(rp,tempstr);
                    }
                  break;

     case LIMIT:  cur_player->portfolio[stock_no].limit = units;
                  for (counter = 0; counter < screen_count ; ++counter)
                    if ((scr_status == STOCKSUP) && (scr_ptr[counter]->stock_no == stock_no))
                      {
                        cursor(12 + counter, 36);
                        if (units != 0)
                          {
                          sprintf(tempstr,"%3d",units);
                          TxWrite(rp,tempstr);
                          }
                        else
                          {
                          sprintf(tempstr,"   ");
                          TxWrite(rp,tempstr);
                          }
                      }
                  break;

     case JUMP:   jump_weeks = units;
        break;




      }
    calc_netw(player);
    if ((scr_status == STOCKSUP) || (scr_status == AUTOSUP))
      upd_netw();
  }



add_auto_exec(auto_minus,auto_plus,option,opt_price,units,stockno,player,pur_price)
int auto_minus,auto_plus,option,opt_price,units,stockno,player,pur_price;
  {
    int count;

    count = players[player - 1].auto_count;

    players[player - 1].auto_ptr[count]->minus_price   = auto_minus;
    players[player - 1].auto_ptr[count]->plus_price    = auto_plus;
    players[player - 1].auto_ptr[count]->option_type   = option;
    players[player - 1].auto_ptr[count]->option_price  = opt_price;
    players[player - 1].auto_ptr[count]->units         = units;
    players[player - 1].auto_ptr[count]->stock_no      = stockno;
    players[player - 1].auto_ptr[count]->cur_price     = stock_array[stockno].price;
    players[player - 1].auto_ptr[count]->pur_price     = pur_price;

    ++players[player - 1].auto_count;

    if ((cur_player == &players[0] + player - 1) && (scr_status == AUTOSUP))
      write_auto(count);
    cursor(24,0);

  }


/************************************************************/
/* Rewrites the entire set of auto execs for the cur_player */
/************************************************************/

auto_display()
  {
    int counter;

    for (counter = 0; counter < cur_player->auto_count; ++counter)
        write_auto(counter);
    cursor(24,0);

  }


/************************************************************/
/*    Writes out a single auto exec of the current player   */
/************************************************************/

write_auto(position)
int position;
  {
    int stockno,cposition;
    float opt_value();

    stockno = cur_player->auto_ptr[position]->stock_no;
    cposition = position + 12;
    cursor(cposition , 1);
    sprintf(tempstr,"%3s",stock_array[stockno].name);
    TxWrite(rp,tempstr);
    cursor(cposition ,5);
    sprintf(tempstr,"%4d", cur_player->auto_ptr[position]->units);
    TxWrite(rp,tempstr);

    switch((int) cur_player->auto_ptr[position]->option_type)
      {
        case PUT:   cursor(cposition, 20);
                    sprintf(tempstr,"PU");
                    TxWrite(rp,tempstr);
                    cursor(cposition , 23);
                    sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->option_price);
                    TxWrite(rp,tempstr);
                    cursor(cposition, 10);
                    sprintf(tempstr,"%9.0f",opt_value(player,position));
                    TxWrite(rp,tempstr);
                    break;
        case CALL:  cursor(cposition,20);
                    sprintf(tempstr,"CA");
                    TxWrite(rp,tempstr);
                    cursor(cposition , 23);
                    sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->option_price);
                    TxWrite(rp,tempstr);
                    cursor(cposition, 10);
                    sprintf(tempstr,"%9.0f",opt_value(player,position));
                    TxWrite(rp,tempstr);
                    break;
    case AUTOSELL:
     case AUTOBUY:  cursor(cposition, 10);
                    sprintf(tempstr,"LIMIT ORD");
                    TxWrite(rp,tempstr);
                    break;
      }

    cursor(cposition ,27);
    sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->cur_price);
    TxWrite(rp,tempstr);

    cursor(cposition , 31);
    if (cur_player->auto_ptr[position]->option_type == AUTOBUY)
      {
        if (cur_player->auto_ptr[position]->minus_price != NONE)
          {
          sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->minus_price);
          TxWrite(rp,tempstr);
          }
        else if (cur_player->auto_ptr[position]->plus_price != NONE)
          {
          sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->plus_price);
          TxWrite(rp,tempstr);
          }
      }

    else
      {
      sprintf(tempstr,"%3s"," - ");
      TxWrite(rp,tempstr);
      }

    cursor(cposition , 36);
    if (cur_player->auto_ptr[position]->option_type == AUTOSELL)
      {
        if (cur_player->auto_ptr[position]->minus_price != NONE)
          {
          sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->minus_price);
          TxWrite(rp,tempstr);
          }
        else if (cur_player->auto_ptr[position]->plus_price != NONE)
          {
          sprintf(tempstr,"%3d", cur_player->auto_ptr[position]->plus_price);
          TxWrite(rp,tempstr);
          }
      }
    else
      {
      sprintf(tempstr,"%3s"," - ");
      TxWrite(rp,tempstr);
      }

  }



clear_auto(position)
int position;
  {
    position += 12;
    cursor(position,1);
    sprintf(tempstr,"   ");
    TxWrite(rp,tempstr);
    cursor(position,5);
    sprintf(tempstr,"    ");
    TxWrite(rp,tempstr);
    cursor(position,10);
    sprintf(tempstr,"         ");
    TxWrite(rp,tempstr);
    cursor(position,20);
    sprintf(tempstr,"      ");
    TxWrite(rp,tempstr);
    cursor(position,27);
    sprintf(tempstr,"   ");
    TxWrite(rp,tempstr);
    cursor(position,31);
    sprintf(tempstr,"   ");
    TxWrite(rp,tempstr);
    cursor(position,35);
    sprintf(tempstr,"    ");
    TxWrite(rp,tempstr);
    cursor(24,com_char_count);
  }




/************************************************************/
/* Updates the current price of a stock in the cur_players  */
/* auto exec list and on the screen.                        */
/************************************************************/

upd_autos(stockno)
int stockno;
  {
    int counter, flag;
    float opt_value();

    flag = FALSE;
    if (scr_status != RANKINGSUP)
      {
        for (counter = 0; counter < cur_player->auto_count; ++counter)
          {
            if ((cur_player->auto_ptr[counter]->stock_no == stockno) &&
                (cur_player->auto_ptr[counter]->cur_price != stock_array[stockno].price))
              {
                flag = TRUE;
                cur_player->auto_ptr[counter]->cur_price = stock_array[stockno].price;
                if (scr_status == AUTOSUP)
                  {
                    cursor(12  + counter, 27);
                    sprintf(tempstr,"%3d",stock_array[stockno].price);
                    TxWrite(rp,tempstr);
                    if ((cur_player->auto_ptr[counter]->option_type != AUTOBUY) &&
                        (cur_player->auto_ptr[counter]->option_type != AUTOSELL))
                      {
                        cursor(12 + counter, 10);
                        sprintf(tempstr,"%9.0f",opt_value(player,counter));
                        TxWrite(rp,tempstr);
                      }
                  }
              }
          }
        if (flag)
          {
            calc_netw(player);
            upd_netw();
          }
      }
  }

