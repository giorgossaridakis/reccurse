// reccurse field&references editor

// field editor and setup routine
void Field_Editor()
{
  int i, selection, fieldshown=0, t;
  char ttext[MAXSTRING];
  
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
   printw("<arrows><enter><space><*><esc>");
   gotoxy(22, 19);
   printw("<j>ump <insert> <delete>");
   Change_Color(3);
   gotoxy(18,20);
   t=sgetch(18,20);
   cleanstdin();
   if (t!=SPACE && t!=LEFT && t!=RIGHT && t!=ESC && t!=INSERT && t!=DELETE && t!='j' && t!='*')
    t='\n';
   switch (t) {
    case SPACE:
     clear();
     Show_Field(&records[fieldshown]);
     getch();
    break;
    case '*':
     clear();
     for (i=0;i<fieldsperrecord;i++) {
      Show_Field(&records[(currentrecord*fieldsperrecord)+i]);
      Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]); }
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
       Change_Color(58);
       for (i=18;i<40;i++) {
        gotoxy(i, 20);
       addch(SPACE); }
       Show_Message(18, 20, 1, "field deleted", 1500);
      --fieldshown; }
     break;
     case 'j':
      printw("to field:");
      i=Scan_Input(1, 1, fieldsperrecord, 4)-1;
      if (i>-1 && i<fieldsperrecord)
       fieldshown=i;
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
        i=Scan_Input(1, 1, 58, 2);
        if (i && i<59)
         record[fieldshown].title_color=i;
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
        i=Scan_Input(1, 1, 58, 2);
        if (i && i<59)
         record[fieldshown].color=i;
       break;
       case 11:
        i=Scan_Input(1, 0, 1, 1);
        if (!i || i==1)
         record[fieldshown].box=i;
       break;
       case 12:
        i=Scan_Input(1, 1, 58, 2);
        if (i && i<59)
         record[fieldshown].box_color=i;
       break;
       case 13:
        i=Scan_Input(1, NUMERICAL, CLOCK, 1);
        if (record[fieldshown].type==VARIABLE && i<VARIABLE && (record[fieldshown].pt.x<1 || record[fieldshown].pt.x>79 || record[fieldshown].pt.y<1 || record[fieldshown].pt.y>23))
         break;
        if (i>=NUMERICAL && i<=CLOCK)
         record[fieldshown].type=i;
       break;
       case 14:
        i=Scan_Input(1, 1, 5, 1);
        if (i>-1 && i<6)
         record[fieldshown].decimals=i;
       break;
       case 15:
        strcpy(ttext, record[fieldshown].suffix);
        Scan_Input(ttext, 1, 24, record[fieldshown].color);
        if (!strlen(ttext) || strlen(ttext)>3)
         break;
        strcpy(record[fieldshown].suffix, ttext); 
       break;
       case 16:
        i=Scan_Input(1, 0, 1, 1);
        if (!i || i==1)
         record[fieldshown].formula=i;
       break;
       case 17:
        i=Scan_Input(1, 1, fieldsperrecord, 4);
        if (i && i<fieldsperrecord+1)
         record[fieldshown].fieldlist=i;
       break;
       case 18:
        i=Scan_Input(1, 0, 1, 1);
        if (!i || i==1)
         record[fieldshown].editable=i;
       break;
       case 19:
        i=Scan_Input(1, 0, 1, 1);
        if (!i || i==1)
         record[fieldshown].active=i;
       break;
       case 20:
        Scan_Input(record[fieldshown].automatic_value, 1, 24, record[fieldshown].color);
      break; }
  break; } }
  alteredparameters=1;
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
   alteredparameters=0;
   Read_Write_db_File(3);
  Read_Write_db_File(1); }
}

// references editor
int References_Editor()
{
  int i, t, relationshipshown=0;
  char ttext[MAXSTRING];
  vector<Relationship> trelationships=relationships; // create a copy, copy back afterwards if wanted
  Relationship trelationship(const_cast <char *> ("dummydatabase"), 0, 1, 2, 3);
  vector<Relationship>::iterator p;
  
  if (!trelationships.size())
   trelationships.push_back(trelationship);
  
  while (t!=ESC && trelationships.size()) {
   Draw_Box(BOXCHAR, 6, 17, 33, 5, 16, 3);
   Show_Menu_Bar(1);
   for (i=18;i<40;i++) {
    gotoxy(i, 20);
   addch(SPACE); }
   Change_Color(2);
   gotoxy(20, 6);
   printw("reccurse relationship tables");
   Change_Color(5);
   gotoxy(18,8);
   printw("external database name");
   Change_Color(3);
   gotoxy(18,9);
   printw("1.filename:%.22s", trelationships[relationshipshown].extDbname);
   Change_Color(5);
   gotoxy(18,11);
   printw("equalized fields");
   Change_Color(3);
   gotoxy(18,12);
   printw("2.external:%d", trelationships[relationshipshown].extFields[0]);
   gotoxy(35,12);
   printw("3.local:%d", trelationships[relationshipshown].localFields[0]);
   gotoxy(18,14);
   Change_Color(5);
   printw("references fields");
   Change_Color(3);
   gotoxy(18,15);
   printw("4.external:%d", trelationships[relationshipshown].extFields[1]);
   gotoxy(35,15); 
   printw("5.local:%d", trelationships[relationshipshown].localFields[1]);
   Change_Color(5);
   gotoxy(18,17);
   printw("relatioship id:");
   Change_Color(3);
   printw("%d", relationshipshown+1);
   Change_Color(2);
   gotoxy(22,18);
   printw("<arrow keys|HOME|END>");
   gotoxy(25,19);
   printw("<INS|DEL|ESC>");
   Change_Color(3);
   gotoxy(18,20);
   t=tolower(sgetch(18,20));
   cleanstdin(); 
   switch (t) {
    case LEFT:
     if (relationshipshown)
      --relationshipshown;
    break;
    case RIGHT:
     if (relationshipshown<trelationships.size()-1)
      ++relationshipshown;
    break;
    case HOME:
     relationshipshown=0;
    break;
    case END:
     relationshipshown=trelationships.size()-1;
    break;
    case INSERT:
     if (trelationships.size()<MAXRELATIONSHIPS) {
      trelationships.push_back(trelationship);
     Show_Message(22,20, 1, "relationship inserted", 1500); }
     else
      Show_Message(20,20, 1, "relationships storage full", 1500);
    break;
    case DELETE:
     p=trelationships.begin();
     p+=relationshipshown;
     trelationships.erase(p);
     relationshipshown+=(!relationshipshown) ? 1 : -1;
     Show_Message(22,20, 1, "relationship deleted", 1500);
    break;
    case '1':
     Show_Menu_Bar(1);
     strcpy(ttext, trelationships[relationshipshown].extDbname);
     t=Scan_Input(ttext, 1, 24, 6);
     if (t==ESC)
      break;
     Reccurse_File_Extension(ttext, 0);
     cropstring(ttext, MAXNAME-3);
     Reccurse_File_Extension(ttext, 2);
     if (!tryfile(ttext)) {
      Show_Message(22,20, 1, "nonexistant database", 1500);
     break; }
     strcpy(trelationships[relationshipshown].extDbname, ttext);
    break;
    case '2':
     Change_Color(6);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].extFields[0]=i;
    break;
    case '3':
     Change_Color(6);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].localFields[0]=i;
    break;
    case '4':
     Change_Color(6);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].extFields[1]=i;
    break;
    case '5':
     Change_Color(6);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].localFields[1]=i;
  break; } }
  alteredparameters=1;
  Change_Color(58);
  for (i=18;i<40;i++) {
   gotoxy(i, 20);
  addch(SPACE); }
  gotoxy(18,20);
  Change_Color(1);
  cleanstdin();
  printw("save changes (y/n):");
  t=sgetch();
  if (tolower(t)=='y') {
   alteredparameters=0;
   relationships.clear();
   for (i=0;i<trelationships.size();i++)
    if (strcmp(trelationships[i].extDbname, "dummydatabase"))
     relationships.push_back(trelationships[i]);
   Read_Write_db_File(3);
  Read_Write_db_File(1); }
    
 return alteredparameters;
}

