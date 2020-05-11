// reccurse menu selector
int Menu_Selector();

// menu selector main
int Menu_Selector()
{
  int i, t, selection=currentmenu, menu=0;
  Show_Menu_Bar(1);
  curs_set(0);
  
  while (t!=ESC) {
   Draw_Box(&drawboxes[menu]);
   Change_Color(6);
   gotoxy(32, 8);
   printw("Menu Selector");
   for (i=0;i<5;i++) {
    Change_Color(2);
    if (selection==i)
     Change_Color(highlightcolors[0]);
    gotoxy(32, 10+i);
   printw("%s", menunames[i]); }
   refresh();
   t=sgetch();
   switch (t) {
    case KEY_MOUSE:
     if (rightmousebuttonclicked())
      t=ESC;  // add selection for menus
    break;
    case '\n':
     currentmenu=selection;
     t=ESC;
    break;
    case UP:
     if (selection)
      --selection;
    break;
    case DOWN:
     if (selection<4)
      ++selection;
   break; } }

  if (currentmenu==4) // quit
   End_Program();
  if (menubar)
   Show_Menu_Bar();
  curs_set(1);
  
 return t;
}
