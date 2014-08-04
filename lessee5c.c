#include "amiga.h"
#include "extern.h"

write_stock(line_inc,element)
int line_inc, element;
  {
    int stockno;

    stockno = scr_ptr[element]->stock_no;
    cursor(12 + line_inc, 1);
    sprintf(tempstr,"%3s",stock_array[stockno].name);
    TxWrite(rp,tempstr);
    cursor(12 + line_inc,5);
    sprintf(tempstr,"%4d", scr_ptr[element]->units);
    TxWrite(rp,tempstr);
    cursor(12 + line_inc,10);
    sprintf(tempstr,"%9.0f", ((float) cur_player->portfolio[stockno].shares) * (stock_array[stockno].price * 100.0));
    TxWrite(rp,tempstr);
    cursor(12 + line_inc,20);
    sprintf(tempstr,"%3d", scr_ptr[element]->price);
    TxWrite(rp,tempstr);
    cursor(12 + line_inc,24);
    sprintf(tempstr,"%9.0f", round((double) cur_player->portfolio[stockno].margin_debt));
    TxWrite(rp,tempstr);
    cursor(12 + line_inc, 36);
    if (cur_player->portfolio[stockno].limit > 0)
      {
      sprintf(tempstr,"%3d",cur_player->portfolio[stockno].limit);
      TxWrite(rp,tempstr);
      }
    else
      {
      sprintf(tempstr,"%3s"," ");
      TxWrite(rp,tempstr);
      }
  }


clear_stock(line_inc)
int line_inc;
  {
    line_inc += 12;
    cursor(line_inc, 1);
    sprintf(tempstr,"%3s"," ");
    TxWrite(rp,tempstr);
    cursor(line_inc, 5);
    sprintf(tempstr,"%4s", " ");
    TxWrite(rp,tempstr);
    cursor(line_inc, 10);
    sprintf(tempstr,"%9s", " ");
    TxWrite(rp,tempstr);
    cursor(line_inc, 20);
    sprintf(tempstr,"%3s", " ");
    TxWrite(rp,tempstr);
    cursor(line_inc, 24);
    sprintf(tempstr,"%9s"," ");
    TxWrite(rp,tempstr);
    cursor(line_inc, 34);
    sprintf(tempstr,"%5s"," ");
    TxWrite(rp,tempstr);
  }



load_screen()
  {
    int counter;

    screen_count = 0;
    for (counter = 0; counter < NO_OF_STOCKS; ++counter)
      {
        if ((cur_player->portfolio[counter].shares > 0) ||
            (cur_player->portfolio[counter].margin_debt > 0))
          {
            scr_ptr[screen_count]->stock_no = ((int) counter);
            scr_ptr[screen_count]->price    = ((int) stock_array[counter].price);
            scr_ptr[screen_count]->margin_debt = cur_player->portfolio[counter].margin_debt;
            scr_ptr[screen_count]->units = cur_player->portfolio[counter].shares;
            ++screen_count;
          }
      }
    for (counter = 0; counter < cur_player->auto_count; ++counter)
      {
        cur_player->auto_ptr[counter]->cur_price =
         ((int) stock_array[cur_player->auto_ptr[counter]->stock_no].price);
      }
  }


/* This procedure loops through all the current players holdings and auto */
/* execs and updates the entire screen of data.                           */

screen_upd()
  {
    int counter;

    for (counter = 0; counter < screen_count; ++counter)
      {
        write_stock(counter,counter);
      }

  }



double calc_netw(player)
int player;
  {
    int counter, is_up;
    double total,difference;
    float opt_value();

    total = 0;
    if ((cur_player == &players[0] + player - 1) && (scr_status == STOCKSUP))
      is_up = TRUE;
    else
      is_up = FALSE;
    if (is_up)
      {
        for (counter = 0; counter < cur_player->stock_count; ++counter)
          {
            total += (100.0 * ((float) scr_ptr[counter]->units) * ((float) scr_ptr[counter]->price))
                     - round((double) scr_ptr[counter]->margin_debt);
          }
      }
    else
      {
        for (counter = 0; counter < NO_OF_STOCKS; ++counter)
          {
            total += (100.0 * ((float) players[player - 1].portfolio[counter].shares) *
                      ((float) stock_array[counter].price))  -
                     players[player - 1].portfolio[counter].margin_debt;
          }
      }
    for (counter = 0; counter < players[player - 1].auto_count; ++counter)
      {
        if ((players[player - 1].auto_ptr[counter]->option_type == PUT) ||
            (players[player - 1].auto_ptr[counter]->option_type == CALL))

          difference = opt_value(player,counter);

        else   /* if it is an auto transaction instead of an option */

          difference = 0;

        total += difference;
      }

    total += players[player - 1].cash;
    total += ((float) players[player - 1].bonds) * 1000.0;

    players[player - 1].net_worth = total;

  }
