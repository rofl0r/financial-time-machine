#include "amiga.h"
#include "extern.h"
#include <stdlib.h>

end_year()
  {
    int player, counter, difference, counter2;
    PLAYER *play_ptr;
    float earnings;
    extern float opt_value();
    extern float calc_taxes();

    clear_bottom();
    scr_status = RANKINGSUP;
    cursor(8,13);
    sprintf(tempstr,"Taxes Assessed");
    TxWrite(rp,tempstr);
    cursor(9,13);
    sprintf(tempstr,"______________");
    TxWrite(rp,tempstr);

    for (player = 0; player < no_of_players; ++player)
      {
        play_ptr = &players[0]+player;
        for (counter = 0; counter < play_ptr->auto_count; ++counter)
          {
            if ((play_ptr->auto_ptr[counter]->option_type == CALL) ||
                (play_ptr->auto_ptr[counter]->option_type == PUT))
              {
                players[player].cash += (opt_value(player + 1,counter) * .985);
                players[player].other_earnings += ((opt_value(player + 1,counter) * .985) -
                 players[player].auto_ptr[counter]->pur_price);
                del_auto(counter,player + 1);
                --counter;
              }

          }  /* end for counter */


        play_ptr->taxes += calc_taxes(player);

        cursor(11 + player,0);
        sprintf(tempstr,"%20s  $%-10.0f",play_ptr->name,play_ptr->taxes);
        TxWrite(rp,tempstr);
        if (play_ptr->taxes < 0)
          {
            cursor(11 + player,37);
            sprintf(tempstr,"CR");
            TxWrite(rp,tempstr);
          }
        else if (play_ptr->taxes <= play_ptr->cash)
          {
            play_ptr->cash -= play_ptr->taxes;
            play_ptr->taxes = 0;
          }
        else
          {
            play_ptr->taxes -= play_ptr->cash;
            play_ptr-> cash = 0;
          }
        play_ptr->short_term = play_ptr->long_term = play_ptr->other_earnings = 0;


      }   /* end for players */

    ++year;
    week = 1;
    upd_year();
    year_over = TRUE;
    clear_line();
    sprintf(tempstr,"END OF YEAR");
    TxWrite(rp,tempstr);
  }


quit()
  {
    int tplayer;
    int counter;
    PLAYER *play_ptr;
    extern float opt_value();

    for (tplayer = 0; tplayer < no_of_players; ++tplayer)
      {
        play_ptr = &players[0]+tplayer;
        for (counter = 0; counter < NO_OF_STOCKS; ++counter)
          {
            if (play_ptr->portfolio[counter].shares > 0)
            execute(SELL,play_ptr->portfolio[counter].shares,counter,stock_array[counter].price,NONE,NONE,tplayer + 1);
          }
        for (counter = 0; counter < play_ptr->auto_count; ++counter)
          {
            if ((play_ptr->auto_ptr[counter]->option_type == CALL) ||
                (play_ptr->auto_ptr[counter]->option_type == PUT))
              {
                play_ptr->cash += opt_value(tplayer + 1,counter);
                del_auto(counter,tplayer + 1);
              }
          }
      }
    display_rankings();
    clear_line();
    sprintf(tempstr,"All Holdings Liquidated");
    TxWrite(rp,tempstr);
    sprintf(tempstr," That's It!");
    TxWrite(rp,tempstr);
    shutdown();

  }

shutdown()
  {
  Delay(300);
  clear_line();
  sprintf(tempstr,"Hit any key to continue");
  TxWrite(rp,tempstr);
  while (kbhit() == NULL)
    ;
  FreeMem(ioa->ioa_Data,sizeof(si));
  CloseDevice(ioa);
  DeletePort(ioa->ioa_Request.io_Message.mn_ReplyPort);
  FreeMem(ioa,sizeof(*ioa));
//  CloseWindow(W);
//  CloseScreen(S);
  exit(TRUE);
  }



set_graph()
  {int i;
  /* Set up graph initialization */
  SetAPen(rp,5);
  Move(rp,280,176);
  Draw(rp,116,176);
  Draw(rp,116,16);
  SetAPen(rp,1);
  for (i=0;i<5;++i)
    {
    Move(rp,124+(i*32),176);
    Draw(rp,124+(i*32),183);
    }
  SetAPen(rp,0);
  SetBPen(rp,5);
  cursor(5,4);
  Text(rp,"X",1);
  cursor(6,4);
  Text(rp,"X",1);
  cursor(7,4);
  Text(rp,"X",1);
  SetAPen(rp,1);
  SetBPen(rp,0);
  cursor(2,10);
  sprintf(tempstr,"200-");
  TxWrite(rp,tempstr);
  cursor(7,10);
  sprintf(tempstr,"150-");
  TxWrite(rp,tempstr);
  cursor(12,10);
  sprintf(tempstr,"100-");
  TxWrite(rp,tempstr);
  cursor(17,11);
  sprintf(tempstr,"50-");
  TxWrite(rp,tempstr);
  cursor(21,11);
  sprintf(tempstr,"10-");
  TxWrite(rp,tempstr);
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


line_draw(position,number,color)
  int position, number, color;
  {
  if ((number>0) && (number <= 200))
   {
  SetAPen(rp,color);
  RectFill(rp,112+(position*8),175-number,119+(position*8),175);
  SetAPen(rp,1);
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

graph(stockno,year)
int stockno, year;
  {
    int x,y;
    int color,tcolor;

    clear_top();
    clear_bottom();
    set_graph();
    year -= 4;
    color = RED;
    tcolor = NOT_FOUND;

    for (x=0; x<20; ++x)
      {

        if (history[stockno].splits[x])
          {
            tcolor = color;
            color = BLUE;
          }

        line_draw(x + 1, (int) round((((float) history[stockno].price[x]) * 0.8) + .49),color);
        SetDrMd(rp,JAM1);
        if (history[MMMS].price[x] != 0)
          {
            cursor((int) (round (( ((double) 21.51) - ( ((double) history[MMMS].price[x])) / ((double) 10.0) ))),
             15 + x);

            sprintf(tempstr,"*");
            TxWrite(rp,tempstr);
          }
        SetDrMd(rp,JAM2);
        if (tcolor != NOT_FOUND)
          {
            if (color == RED)
              color = GREEN;
            else
              color = RED;
            tcolor = NOT_FOUND;
          }
        if (color == RED)
          color = GREEN;
        else
          color = RED;
        if ((x % 4) == 0)
          {
            cursor(23,15 + x);
            if (year > 1900)
              {
              sprintf(tempstr,"%2d",year - 1900);
              TxWrite(rp,tempstr);
              }
            else
              {
              sprintf(tempstr,"%2d",year);
              TxWrite(rp,tempstr);
              }
            ++year;
          }
      }
    cursor(2,32);
    sprintf(tempstr,"%s",stock_array[stockno].name);
    TxWrite(rp,tempstr);
    cursor(1,1);
    sprintf(tempstr,"Stock");
    TxWrite(rp,tempstr);
    cursor(3,1);
    sprintf(tempstr,"Splits");
    TxWrite(rp,tempstr);
    scr_status = GRAPHUP;
  }
