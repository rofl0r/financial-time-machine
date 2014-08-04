int kbhit() {
#if 0
  /*
     Borrowed from The Greenleaf Functions.  Returns 0 if no key
     has been hit. Otherwise it returns the keyboard code
  */

  if (1<<W->UserPort->mp_SigBit)
    {
    messg = (struct IntuiMessage *) GetMsg(W->UserPort);
    ReplyMsg(messg);
    if (messg->Class == RAWKEY)
      return((int) messg->Code);
    else
      return(NULL);
    }
  else
    return(NULL);

#else
#warning "need to implement kbhit()!"
return 0;
#endif
  }

