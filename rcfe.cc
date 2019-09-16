// reccurse field editor


// field editor and setup routine
void Field_Editor()
{
  int i, selection, fieldshown=0;
  char t, ttext[MAXSTRING];
  
  while (t!=ESC) {
   Draw_Box(BOXCHAR, 6, 17, 33, 5, 16, 3);
   Show_Menu_Bar(1);
   Change_Color(58);
   for (i=18;i<40;i++) {
    gotoxy(i, 20);
   addch(SPACE); }
   Change_Color(3);
   gotoxy(25, 6);
   printw("reccurse database");
   for (i=8;i<18;i++) {
    gotoxy(34, i);
   printw("|"); }
   gotoxy(29, 7);
   attron(A_BOLD);
   printw("field id:%d", fieldshown+1);
   attroff(A_BOLD);
   gotoxy(18, 8);
   printw("1.title:%.8s", record[fieldshown].title);
   gotoxy(18, 9);
   printw("2.title pos:%d", record[fieldshown].title_position);
   gotoxy(18, 10);
   printw("3.title attrs:*");
   gotoxy(18, 11);
   printw("4.title color:%2d", record[fieldshown].title_color);
   gotoxy(18,12);
   printw("5.pos.x:%d", record[fieldshown].pt.x);
   gotoxy(18,13);
   printw("6.pos.y:%d", record[fieldshown].pt.y);
   gotoxy(18,14);
   printw("7.size.x:%d", record[fieldshown].size.x);
   gotoxy(18,15);
   printw("8.size.y:%d", record[fieldshown].size.y);
   gotoxy(18,16);
   printw("9.attributes:*");
   gotoxy(18,17);
   printw("10.color:%2d", record[fieldshown].color);
   gotoxy(35,8);
   printw("11.box:%s", onoff[record[fieldshown].box]);
   gotoxy(35,9);
   printw("12.box color:%2d", record[fieldshown].box_color);
   gotoxy(35,10);
   printw("13.field type:%d", record[fieldshown].type);
   gotoxy(35,11);
   printw("14.decimals:%d", record[fieldshown].decimals);
   gotoxy(35,12);
   printw("15.suffix:*");
   gotoxy(35,13);
   printw("16.formula:%s", onoff[record[fieldshown].formula]);
   gotoxy(35,14);
   printw("17.list:%d", record[fieldshown].fieldlist);
   gotoxy(35,15);
   printw("18.editable:%s", onoff[record[fieldshown].editable]);
   gotoxy(35,16);
   printw("19.active:%s", onoff[record[fieldshown].active]);
   gotoxy(35,17);
   printw("20.autovalue:*");
   Change_Color(5);
   gotoxy(19, 18);
   printw("<arrows> <enter> <space> <esc>");
   gotoxy(22, 19);
   printw("<j>ump <insert> <delete>");
   Change_Color(3);
   gotoxy(18,20);
   t=tolower(sgetch(18,20));
   cleanstdin();
   if (t!=SPACE && t!=LEFT && t!=RIGHT && t!=ESC && t!=INSERT && t!=DELETE && t!='j')
    t='\n';
   switch (t) {
    case SPACE:
     Clear_Screen();
     Show_Field(&records[fieldshown]);
     getch();
    break;
    case LEFT:
     if (fieldshown)
      --fieldshown;
     break;
     case RIGHT:
      if (fieldshown<fieldsperrecord-1)
       ++fieldshown;
      break;
     case INSERT:
      Add_Field();
      strcpy(ttext, "added field ");
      strcat(ttext, itoa(fieldsperrecord));
      Show_Message(18, 20, 1, ttext, 1500);
     break;
     case DELETE:
      if (fieldsperrecord<2)
       break;
      printw("delete (y/n):");
      t=sgetch();
      if (tolower(t)=='y') {
       Delete_Field(fieldshown);
       Show_Message(18, 20, 1, "field deleted      ", 1500);
      --fieldshown; }
     break;
     case 'j':
      printw("to field:");
      fieldshown=Scan_Input(1, 1, fieldsperrecord-1, 4);
     break;
     case '\n':
      printw("selection:");
      selection=Scan_Input(1, 1, 20, 2);
      printw("->");
      switch (selection) {
       case 1:
        strcpy(ttext, record[fieldshown].title);
        Scan_Input(ttext, 1, 24, record[fieldshown].title_color);
        if (!strlen(ttext) || strlen(ttext)>MAXTITLE)
         break;
        strcpy(record[fieldshown].title, ttext);
       break;
       case 2:
        record[fieldshown].title_position=Scan_Input(1, 0, 3, 1);
       break;
       case 3:
        strcpy(ttext, record[fieldshown].title_attributes);
        Scan_Input(ttext, 1, 24, record[fieldshown].title_color);
        if (!strlen(ttext) || strlen(ttext)>9)
         break;
        strcpy(record[fieldshown].title_attributes, ttext);
       break;
       case 4:
        record[fieldshown].title_color=Scan_Input(1, 1, 58, 2);
       break;
       case 5:
        record[fieldshown].pt.x=Scan_Input(1, 1, 80, 2);
       break;
       case 6:
        record[fieldshown].pt.y=Scan_Input(1, 1, 24, 2);
       break;
       case 7:
        record[fieldshown].size.x=Scan_Input(1, 1, 75, 2);
       break;
       case 8:
        record[fieldshown].size.y=Scan_Input(1, 1, 20, 2);
       break;
       case 9:
        strcpy(ttext, record[fieldshown].attributes);
        Scan_Input(ttext, 1, 24, record[fieldshown].color);
        if (!strlen(ttext) || strlen(ttext)>9)
         break;   
        strcpy(record[fieldshown].attributes, ttext);
       break;
       case 10:
        record[fieldshown].color=Scan_Input(1, 1, 58, 2);
       break;
       case 11:
        record[fieldshown].box=Scan_Input(1, 0, 1, 1);
       break;
       case 12:
        record[fieldshown].box_color=Scan_Input(1, 1, 58, 2);
       break;
       case 13:
        record[fieldshown].type=Scan_Input(1, 0, 2, 1);
       break;
       case 14:
        record[fieldshown].decimals=Scan_Input(1, 1, 5, 1);
       break;
       case 15:
        strcpy(ttext, record[fieldshown].suffix);
        Scan_Input(ttext, 1, 24, record[fieldshown].color);
        if (!strlen(ttext) || strlen(ttext)>3)
         break;
        strcpy(record[fieldshown].suffix, ttext); 
       break;
       case 16:
        record[fieldshown].formula=Scan_Input(1, 0, 1, 1);
       break;
       case 17:
        record[fieldshown].fieldlist=Scan_Input(1, 1, fieldsperrecord, 4);
       break;
       case 18:
        record[fieldshown].editable=Scan_Input(1, 0, 1, 1);
       break;
       case 19:
        record[fieldshown].active=Scan_Input(1, 0, 1, 1);
       break;
       case 20:
        Scan_Input(record[fieldshown].automatic_value, 1, 24, record[fieldshown].color);
      break; }
  break; } }
  currentrecord=0;
  Change_Color(58);
  for (i=18;i<40;i++) {
   gotoxy(i, 20);
  addch(SPACE); }
  Change_Color(1);
  gotoxy(18,20);
  cleanstdin();
  printw("save changes (y/n):");
  t=sgetch();
  if (tolower(t)=='y') {
   Read_Write_db_File(3);
  Read_Write_db_File(1); }
}
