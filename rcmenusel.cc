// reccurse menu selector

int startx, starty, width, height;
  
// menu selector main
int Menu_Selector()
{
  int i, i1, t, selection=0, cmenu=currentmenu, fathermenuid, ccommand=0, replaychar=0; // cmenu is pos in vector, turn ccommand to array
  Show_Menu_Bar(1);
  mousemenucommands.clear();
  curs_set(0);
  Draw_Mouse_Box(mousemenus[cmenu].menuId);
  if (cmenu>3) // add more in reccurse.cc 2598
   cmenu=0;
  while (t!=ESC) {

   for (i=0;i<mousemenus[cmenu].menuEntries.size();i++) {
    Change_Color(2);
    if (selection==i)
     Change_Color(highlightcolors[0]);
    gotoxy(startx+1, starty+1+i);
    // place string in the middle of width
   printw("%s", bringstringtomiddle(mousemenus[cmenu].menuEntries[i].entryText, width)); }
   refresh();
   t=(replaychar) ? replaychar : sgetch();
   replaychar=0;
   if (t==']') // imitate right mouse button
    t=ESC;
   switch (t) {
    case KEY_MOUSE:
     if (leftmousebuttondoubleclicked()) {
      i1=locatemenuselectionbymouseclick(cmenu);
      if (i1>-1) {
       selection=i1;
       replaychar='\n';
      }
      break;
     }
     if (rightmousebuttonclicked()) {
      t=ESC;
     break; }
     i1=locatemenuselectionbymouseclick(cmenu);
     if (i1>-1)
      selection=i1;
     else
      t=ESC;
    break;
    case '\n':
     // copy commands to external vector
     if (mousemenus[cmenu].menuEntries[selection].returnCommands.size()) {
      for (int x:mousemenus[cmenu].menuEntries[selection].returnCommands)
      mousemenucommands.push_back(x);
      if (mousemenus[cmenu].menuEntries[selection].menuDirectionId==mousemenus[cmenu].menuId) { // if menuDirectionId is the same as current menu, send vector to main
      ccommand=mousemenucommands[mousemenucommands.size()-1];
     break; } }
     i1=mousemenus[cmenu].fatherMenuPosition;
     fathermenuid=mousemenus[cmenu].fatherMenuId;
     for (i=0;i<mousemenus.size();i++)
      if (mousemenus[i].menuId==mousemenus[cmenu].menuEntries[selection].menuDirectionId) {
       Draw_Mouse_Box(mousemenus[cmenu].menuId);
       cmenu=i;
     Draw_Mouse_Box(mousemenus[cmenu].menuId); }
     if (mousemenus[cmenu].menuReference>-1) // if menu has reference to program menu, change to that
      currentmenu=mousemenus[cmenu].menuReference;
     selection=0;
     // if new menu is the same as father of previous, go to it's selection position
     if (mousemenus[cmenu].fatherMenuId==fathermenuid);
    selection=i1;
    break;
    case UP:
     if (!selection) {
      selection=mousemenus[cmenu].menuEntries.size()-1;
     break; }
     if (selection)
      --selection;
    break;
    case DOWN:
     if (selection==mousemenus[cmenu].menuEntries.size()-1) {
      selection=0;
     break; }
     if (selection<mousemenus[cmenu].menuEntries.size()-1)
      ++selection;
   break; }
   if (ccommand)   
  break; }

  if (menubar)
   Show_Menu_Bar();
  curs_set(1);
  
 return ccommand;
}

// draw mouse box 
int Draw_Mouse_Box(int menuid)
{
  int i, i1, boxposition=-1;  
  
   for (i=0;i<mousemenuboxes.size();i++)
    if (mousemenuboxes[i].menuId==menuid) {
     boxposition=i;
   break; }
   if (boxposition<0 || boxposition>mousemenuboxes.size()-1)
   return 0;
   Draw_Box(mousemenuboxes[boxposition]);
   startx=mousemenuboxes[boxposition].pt.x;
   starty=mousemenuboxes[boxposition].pt.y;
   width=mousemenuboxes[boxposition].size.x;
   height=mousemenuboxes[boxposition].size.y;
    
 return 1;
}

// locate menu selection by mouse position
int locatemenuselectionbymouseclick(int cmenu)
{
  int i, i1, selection=-1;
  
     for (i=0;i<mousemenus[cmenu].menuEntries.size()+1;i++)
      for (i1=0;i1<width;i1++)
       if (mouse.x+1==startx+i1 && mouse.y+1==starty+i) {
        selection=i-1;
       break; }
       
 return selection;
}

