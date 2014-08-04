#include "amiga.h"
#include "extern.h"

upd_stock(stockno)
int stockno;
  {
    int tally, counter;
    float garbage;

    tally = 0;
    counter = 0;

    if (scr_status == STOCKSUP)
      {
        while ((scr_ptr[tally]->stock_no != stockno)  &&
               (tally < screen_count))
          {
            ++tally;
          }

        /* if the stock is a new acquisition */

        if ((tally == screen_count) &&
            (cur_player->portfolio[stockno].shares != 0))
          {
            scr_ptr[screen_count]->stock_no = stockno;
            scr_ptr[screen_count]->units = cur_player->portfolio[stockno].shares;
            scr_ptr[screen_count]->price = stock_array[stockno].price;
            scr_ptr[screen_count]->margin_debt = cur_player->portfolio[stockno].margin_debt;

            write_stock(tally,screen_count);
            ++screen_count;
            goto continue4;
          }

        /* if player has sold all shares of the stock */

        if ((cur_player->portfolio[stockno].shares == 0) &&
                 (cur_player->portfolio[stockno].margin_debt == 0) &&
                 (tally < screen_count))
          {
            SCREEN *tmp_ptr;

            tmp_ptr = scr_ptr[tally];
            for (counter = tally; counter < STOCKLIMIT - 1; ++counter)
              {
                scr_ptr[counter] = scr_ptr[counter + 1];
              }
            scr_ptr[STOCKLIMIT - 1] = tmp_ptr;
            --screen_count;
            for (counter = tally; counter < screen_count; ++counter)
              {
                write_stock(counter,counter);
              }
            clear_stock(screen_count);
            cursor(24,0);
            goto continue4;
          }

    /* if a change in the amount or value of a stock already held occured */

        if ((cur_player->portfolio[stockno].shares > 0) ||
                 (cur_player->portfolio[stockno].margin_debt > 0))
          {
        /* update the players netw according to the current prices */

            if (scr_ptr[tally]->price != stock_array[stockno].price)
              {
                cur_player->net_worth += (float) (((int) stock_array[stockno].price - (int) scr_ptr[tally]->price) *
                                        100.0 * ((int) scr_ptr[tally]->units));
                upd_netw();
                scr_ptr[tally]->price = stock_array[stockno].price;
                cursor(12 + tally, 10);
                garbage = (float) (scr_ptr[tally]->price) * (float) (scr_ptr[tally]->units) * 100.0;
                sprintf(tempstr,"%9.0f", garbage);
                TxWrite(rp,tempstr);
                cursor(12 + tally, 20);
                sprintf(tempstr,"%3d",scr_ptr[tally]->price);
                TxWrite(rp,tempstr);
                cursor(24,com_char_count);
                goto continue3;
              }
            if (scr_ptr[tally]->units != cur_player->portfolio[stockno].shares)
              {
                scr_ptr[tally]->units = cur_player->portfolio[stockno].shares;
                scr_ptr[tally]->margin_debt = cur_player->portfolio[stockno].margin_debt;
                write_stock(tally,tally);
                cursor(24,com_char_count);
                goto continue3;
              }
            if (scr_ptr[tally]->margin_debt != cur_player->portfolio[stockno].margin_debt)
              {
                scr_ptr[tally]->margin_debt = cur_player->portfolio[stockno].margin_debt;
                cursor(12+tally,24);
                sprintf(tempstr,"%9.0f",round((float) scr_ptr[tally]->margin_debt));
                TxWrite(rp,tempstr);
                cursor(24,com_char_count);
              }
continue3:            if ((cur_player->portfolio[stockno].shares == 0) && (!stop))
              make_call = TRUE;


          }   /* end: update a current holding */

continue4:;      }  /* end: if rankings aren't up */
  }
