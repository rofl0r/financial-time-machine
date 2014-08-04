#include "amiga.h"
#include "extern.h"

/** Calculates the value of the player's (1 - 4) option in auto ptr ****/
/** POSITION.                                                       ****/


float opt_value(player,position)
int player,position;
  {
    PLAYER *ptr;
    float difference;
    int stock_price,waste;

    ptr = &players[0] + player - 1;
    if (ptr == cur_player)
      stock_price = ptr->auto_ptr[position]->cur_price;
    else
      stock_price = stock_array[ptr->auto_ptr[position]->stock_no].price;

/*
fp = fopen("prn","w");
fprintf(fp,"stock price is %d\n",stock_price);
fclose(fp);
*/

    if (ptr->auto_ptr[position]->option_type == CALL)
      difference = (float) (((int) stock_price) - ((int) ptr->auto_ptr[position]->option_price));
    else
      difference = (float) (((int) ptr->auto_ptr[position]->option_price) - ((int) stock_price));

/*
fp = fopen("prn","w");
fprintf(fp,"difference is %f\n",difference);
fclose(fp);
*/

    difference *= (100.0 * ((float) ptr->auto_ptr[position]->units) );

/*
fp = fopen("prn","w");
fprintf(fp,"difference is now %f\n",difference);
fclose(fp);
*/

    if (difference > 0)
      return(difference);
    else
      return(0);
  }




/****************************************************************************/
/*    This deletes the specified auto exec from the specified player. It    */
/*    updates the screen if appropriate.                                    */
/****************************************************************************/

del_auto(position,player)
int position, player;
  {
    int counter,waste;
    AUTOS *tmp_ptr;

    tmp_ptr = players[player - 1].auto_ptr[position];
    for (counter = position; counter < AUTOLIMIT - 1; ++counter)
      {
        players[player - 1].auto_ptr[counter] = players[player - 1].auto_ptr[counter + 1];
      }
    players[player -1].auto_ptr[AUTOLIMIT - 1] = tmp_ptr;
    --players[player - 1].auto_count;
    if ((cur_player == &players[0] + player - 1) &&
        (scr_status == AUTOSUP))
      {
        clear_auto(cur_player->auto_count);
        auto_display();
      }
  }


chk_autos(stockno)
int stockno;
  {
    int count,player,tunits;
    PLAYER *ptr;

    for (player = 1; player <= no_of_players; ++player)
      {
        ptr = &players[0] + player - 1;
        for (count = 0; count < ptr->auto_count; ++count)
          {
            tunits = (int) ptr->auto_ptr[count]->units;
            if (( ((int) ptr->auto_ptr[count]->stock_no) == stockno) &&
                ( ((int) ptr->auto_ptr[count]->option_type) == AUTOBUY))
              {
                if (( ((int) ptr->auto_ptr[count]->minus_price) >= stock_array[stockno].price) &&
                    ( ((int) ptr->auto_ptr[count]->minus_price) > 0) &&
                    (ptr->cash >= round(((float) ptr->auto_ptr[count]->units) * ((float) stock_array[stockno].price) *
                      101.5)) &&
                    ( ((int) ptr->stock_count) < STOCKLIMIT))
                  {
                    del_auto(count,player);
                    --count;
                    execute(BUY,tunits,stockno,stock_array[stockno].price,NONE,NONE,player);
                  }
                else if (( ((int) ptr->auto_ptr[count]->plus_price) <= stock_array[stockno].price) &&
                         ( ((int) ptr->auto_ptr[count]->plus_price) > 0) &&
                         (ptr->cash >= round(((float) ptr->auto_ptr[count]->units) *
                           ((float) stock_array[stockno].price) * 101.5)) && 
                         ( ((int) ptr->stock_count) < STOCKLIMIT))

                  {
                    del_auto(count,player);
                    --count;
                    execute(BUY,tunits,stockno,stock_array[stockno].price,NONE,NONE,player);
                  }
              }  /* end if doing an auto_minus */

            else if (( ((int) ptr->auto_ptr[count]->stock_no) == stockno) &&
                     ( ((int) ptr->auto_ptr[count]->option_type) == AUTOSELL))
              {
                if (( ((int) ptr->auto_ptr[count]->minus_price) > 0) &&
                    ( ((int) ptr->auto_ptr[count]->minus_price) >= stock_array[stockno].price))
                  {
                    del_auto(count,player);
                    --count;
                    if (tunits > ((int) ptr->portfolio[stockno].shares))
                      tunits = ((int) ptr->portfolio[stockno].shares);
                    if (tunits > 0)
                      execute(SELL,tunits,stockno,stock_array[stockno].price,NONE,NONE,player);
                  }
                else if (( ((int) ptr->auto_ptr[count]->plus_price) > 0) &&
                         ( ((int) ptr->auto_ptr[count]->plus_price) <= stock_array[stockno].price))
                  {
                    del_auto(count,player);
                    --count;
                    if (tunits > ((int) ptr->portfolio[stockno].shares))
                      tunits = ((int) ptr->portfolio[stockno].shares);
                    if (tunits > 0)
                      execute(SELL,tunits,stockno,stock_array[stockno].price,NONE,NONE,player);
                  }

              }  /** end if doing an AUTOSELL  **/

          }
        for (count = 0; count < NO_OF_STOCKS; ++count)
          {
            if (( ((int) ptr->portfolio[count].limit) >= stock_array[count].price) &&
                ( ((int) ptr->portfolio[count].shares) > 0))
              execute(SELL,((int) ptr->portfolio[count].shares),count,stock_array[stockno].price,NONE,NONE,player);
          }

      }
  }


/*
quit()
  {
    FILE *score;
    int tplayer,player;
    double tscore,h_score;
    char name[21];
    char *fgets();
    float get_float();

    tscore = 0;
    for (player = 0; player < no_of_players; ++player)
      {
        calc_netw(player + 1);
        if (players[player].net_worth > tscore)
          {
            tscore = players[player].net_worth;
            tplayer = player;
          }
      }
    score = fopen("score","r");
    if (score != NULL)
      {
        fgets(name,20,score);
        h_score = get_float(score);
        fclose(score);
        if (tscore > h_score)
            goto new;
        else
          {
            sprintf(tempstr,"HIGH SCORE TO DATE: \n %s %f\n",name,h_score);
            TxWrite(rp,tempstr);
            return(0);
          }
      }
    new:
    clear_line();
    sprintf(tempstr,"CONGRAGULATIONS %s,\n\n     NEW HIGH SCORE %d\n",players[tplayer].name,tscore);
    TxWrite(rp,tempstr);
    if (tscore < 5000)
      printf("\n\nNOT TOO IMPRESSIVE REALLY...\n");
    score = fopen("score","w");
    fprintf(score,"%-20s %f\n",players[tplayer].name,tscore);
    fclose(score);
  }


*/
