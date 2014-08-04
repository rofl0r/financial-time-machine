#include "amiga.h"
#include "extern.h"

#define OPTIONS 1
#define TAXES 2

margin()
  {
    int player;
    int counter;
    static  char owes[15];
    int found,passby;
    float dif,y;
    PLAYER *ptr;
    float opt_value();



    make_call = FALSE;
    owes[0] = 0;
    found = FALSE;
    passby = FALSE;


    for (player = 0; player < no_of_players; ++player)
      {
        ptr = &players[0]+player;


        for (counter = 0; counter < NO_OF_STOCKS; ++counter)
          {
            if ((round((double) ptr->portfolio[counter].margin_debt) >
                 (dif = (round( (((float) ptr->portfolio[counter].shares) * 100.0 *
                                ((float) stock_array[counter].price)) / 2.0)))))
              {
                dif = round((double) ptr->portfolio[counter].margin_debt) - dif;
                if (ptr->cash >= dif)
                  {
                    ptr->cash -= dif;
                    ptr->portfolio[counter].margin_debt -= dif;
                    passby = TRUE;
                  }
                else
                  {
                    ptr->portfolio[counter].margin_debt -= ptr->cash;
                    ptr->cash = 0;
                    timer1.status = OFF;
                    stop = TRUE;
                    make_call = TRUE;
                    owes[player] = 1;
                    found = OPTIONS;
                  }
                if (round((double) ptr->portfolio[counter].margin_debt) < 1.0)
                  {
                    ptr->portfolio[counter].margin_debt = 0;
                    if (ptr->portfolio[counter].shares == 0)
                      --ptr->stock_count;
                  }
                if (cur_player == ptr)
                  {
                    upd_stock(counter);
                    upd_coh();
                  }

              }
          }

        if (ptr->taxes > 0)
          if (ptr->cash < ptr->taxes)
            {

              ptr->taxes -= ptr->cash;
              ptr->cash = 0;
              found = TAXES;
              stop = TRUE;
              make_call = TRUE;
              owes[player + 4] = 1;
            }
          else
            {
              ptr->cash -= ptr->taxes;
              ptr->taxes = 0;
            }


        if (cur_player == ptr)
          {
            calc_netw(player + 1);
            upd_netw();
            upd_coh();
          }


      }

    if (found || passby)
      {
        clear_line();
        message = TRUE;
        if (passby && !found)
          {
          sprintf(tempstr,"MARGIN CALL ISSUED");
          TxWrite(rp,tempstr);
          }
        else if (stop)
          {
            int x;

            x = 0;
            if (found == OPTIONS)
              {
                clear_bottom();
                scr_status = RANKINGSUP;
                for (player = 0; player < no_of_players; ++player)
                  {
                    if (owes[player])
                      {
                        cursor(11 + x,4);
                        ++x;
                        sprintf(tempstr,"Player %d Must Settle Margin Debts",player + 1);
                        TxWrite(rp,tempstr);
                      }
                  }
              }
            else
              {
                clear_bottom();
                scr_status = RANKINGSUP;
                for (player = 0; player < no_of_players; ++player)
                  {
                    if (owes[4 + player])
                      {
                        cursor(11 + x,5);
                        ++x;
                        sprintf(tempstr,"Player %d Must Settle Tax Debts",player + 1);
                        TxWrite(rp,tempstr);
                      }
                  }
              }

            for (player = 0; player < no_of_players; ++player)
              {
                ptr = &players[0]+player;
                y = calc_netw(player + 1);
                if (!q_break)
                  y -= (((float) ptr->bonds) * 1000.0);
                for (x = 0; x < players[player].auto_count; ++x)
                  {
                    if ((players[player].auto_ptr[x]->option_type == CALL) ||
                        (players[player].auto_ptr[x]->option_type == PUT))
                        y += 1;
                  }

                if (y <= 0)
                  {
                    ptr->cash = 0;
                    ptr->bonds = 0;
                    ptr->net_worth = 0;
                    ptr->auto_count = 0;
                    ptr->stock_count = 0;
                    ptr->taxes = 0;
                    for (x = 0; x < NO_OF_STOCKS; ++x)
                      {
                        ptr->portfolio[x].shares = 0;
                        ptr->portfolio[x].margin_debt = 0;
                      }
                    display_rankings();
                    clear_line();
                    sprintf(tempstr,"%s IS BANKRUPT",ptr->name);
                    TxWrite(rp,tempstr);
                  }



      }  /* end going through players */

          }

      }



    if (stop == FALSE)
      {
        q_break = FALSE;
        year_over = FALSE;
      }
  }



end_quarter()
  {
    int player,x,y;
    float earnings,myrandom();
    double sqr();

    timer1.status = OFF;
    timer2.status = OFF;
    timer3.status = OFF;
    in_progress = FALSE;

    for (player = 0; player < no_of_players; ++player)
      {
        earnings = round((double) (( ((float) players[player].bonds) * BONDVALUE) * bond_rate));
        players[player].cash += earnings;
        players[player].other_earnings += earnings;
        players[player].net_worth += round((double) (( ((float) players[player].bonds) * BONDVALUE) * bond_rate));
      }
    for (x = 0; x < NO_OF_STOCKS; ++x)
      history[x].price[15 + quarter] = ((int) stock_array[x].price);
    if (quarter == 4)
      for (x = 0; x < NO_OF_STOCKS; ++x)
        {
          for (y = 0; y < 16; ++y)
            history[x].price[y] = history[x].price[y + 4];
          for (y = 16; y < 20; ++y)
            history[x].price[y] = 0;
          shift_it(history[x].splits);
        }
    bond_rate = (q_int_rate * .85);
    upd_coh();
    upd_netw();
    q_break = TRUE;
    stop = TRUE;
    ++quarter;
    if (quarter == 5)
      quarter = 1;
    upd_quarter();
    message = TRUE;
    clear_line();
    sprintf(tempstr,"END OF QUARTER");
    TxWrite(rp,tempstr);
    factor1 = sqr(0.2 * q_int_rate);
    factor2 = sqr(0.02 * q_int_rate);
    factor3 = sqr(0.04 * q_int_rate);
    factor4 = sqr(0.3 * q_int_rate);

  }


double sqr(x)
double x;
  {
    return(x * x);
  }

shift_it(str)
  char str[];
  {
  int i;
  for (i=0; i<16; ++i)
    str[i] = str[i+4];
  }
