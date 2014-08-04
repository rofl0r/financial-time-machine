#include "amiga.h"
#include "extern.h"

upd_coh()
  {
    if ((scr_status != RANKINGSUP) && (scr_status != GRAPHUP))
      {
        cursor(22,5);
        sprintf(tempstr,"%-12.0f", cur_player->cash);
        TxWrite(rp,tempstr);
        cursor(24,0);
      }
  }




upd_netw()
  {
    if ((scr_status != RANKINGSUP) && (scr_status != GRAPHUP))
      {
        cursor(22,23);
        sprintf(tempstr,"%-12.0f", cur_player->net_worth);
        TxWrite(rp,tempstr);
        cursor(24,0);
      }
  }


upd_bonds()
  {
    if (scr_status != RANKINGSUP)
      {
        cursor(22,38);
        sprintf(tempstr,"%2d",cur_player->bonds);
        TxWrite(rp,tempstr);
        cursor(24,0);
      }
  }

upd_quarter()
  {
    int junk;

    cursor(0,0);

    junk = (quarter % 4);

    if (junk == 0)
      junk = 4;

    sprintf(tempstr,"%d",junk);
    TxWrite(rp,tempstr);

    switch (junk)
      {
        case 1: sprintf(tempstr,"ST ");
                TxWrite(rp,tempstr);
                break;
        case 2: sprintf(tempstr,"ND ");
                TxWrite(rp,tempstr);
                break;
        case 3: sprintf(tempstr,"RD ");
                TxWrite(rp,tempstr);
                break;
        case 4: sprintf(tempstr,"TH ");
                TxWrite(rp,tempstr);
                break;
       default: sprintf(tempstr,"ERROR QUARTER IS %d",junk);
                TxWrite(rp,tempstr);
                break;
      }

    sprintf(tempstr,"QUARTER");
    TxWrite(rp,tempstr);

    cursor(24,com_char_count);

  }


upd_year()
  {
    if (year < 1000)
      {
        cursor(0,17);
        sprintf(tempstr,"YEAR %d",year);
        TxWrite(rp,tempstr);
      }
    else
      {
        cursor(0,18);
        sprintf(tempstr,"%4d",year);
        TxWrite(rp,tempstr);
      }
    cursor(24,com_char_count);
  }

upd_prime()
  {
    float x;

    x = prime_rate * 100;
    cursor(0,34);
    sprintf(tempstr,"%5.2f%%", x);
    TxWrite(rp,tempstr);
    cursor(24,com_char_count);
  }
