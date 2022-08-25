// definitions
const int MAXTOKENS=10;
const int QUOTE=34;
const int COMMA=44;

// variables
extern char input_string[MAXSTRING];
const char *daysofweek[] = {
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
    "Monday",
    "Tuesday"
};
const char *months[]= {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

//functions

// simple code and decode char
char charcoder(char d, int mode) // 0 encrypt, 1 decrypt
{
  char t;
  int code_key=106; // reccurse
  
  if (!mode)
   t=d+code_key;
  else
   t=d-code_key;

 return t;
}

// use charcoder to encode-decode file
int filecodedecode(char *source, char *destination, int mode) // 0 code, 1 decode
{
  FILE *s, *d;
  s=fopen(source, "rb");
  d=fopen(destination, "wb");
  if (!s || !d)
   return -1;
  
  while (s) 
   fputc(charcoder(fgetc(s), mode), d);

 return 0;
}

// separate words in string and store in vector inside structures
int separatewords(char formula[], vector<Word> &separatedwords, char *separator)
{
  vector<Word>::iterator p=separatedwords.begin();
  while (p<separatedwords.end()) // because words.clear() only moves pointer, words are kept
   separatedwords.erase(p++);
  separatedwords.clear();
    
   Word *tstringpart=strtok2(formula, separator);
   while (tstringpart!=NULL) {
    separatedwords.push_back(*tstringpart);
    tstringpart=strtok2(NULL, separator);
   }

  return (int) separatedwords.size();
}

// strtok2, strtok alike, returns struct with char and word position, start and end in string
Word* strtok2(char *formula, char *separator)
{
  int n=0;
  char tformula[MAXSTRING];
  Word *sP=NULL;
  static int position;
  static char lformula[MAXSTRING];
  
  // initilizations
  if (formula!=NULL) {
   position=0;
   strcpy(lformula, formula);
  }

    while ( (lformula[position]==separator[0] || lformula[position]==separator[1] || lformula[position]==separator[2] || lformula[position]==separator[3]) && position<(int) strlen(lformula))
     ++position;
    while (lformula[position]!=separator[0] && lformula[position]!=separator[1] && lformula[position]!=separator[2] && lformula[position]!=separator[3] && position<(int) strlen(lformula))
     tformula[n++]=lformula[position++];
    tformula[n]='\0';
    if (n) {
     sP=new Word;
     strcpy(sP->formulaWord, tformula);
     sP->formulaPosition[STARTOFWORD]=position-strlen(tformula);
     sP->formulaPosition[ENDOFWORD]=position;
    }

  return sP;
}

// show error for file access
void Show_File_Error(char *filename)
{
  Change_Color(RED);
  gotoxy(1,24);
  if (!(int) strlen(filename))
   strcpy(filename, "<none>");
  printw("error: could not access file %s", filename);
  refresh();
  getchar();
}

// use charcoder to encode-decode string
void stringcodedecode(char *source, char *destination, int mode) // 0 code, 1 decode
{
  while (*source)
   *destination++=charcoder(*source++, mode);
}

// checkpoint
void checkpoint(int id, int color)
{
  char text[MAXSTRING];
  
  strcpy(text, "checkpoint ");
  strcat(text, itoa(id));
  strcat(text, "\n");
  printw("%s", text);
  refresh();
}

// scan integer and string
int Scan_Input(int flag, int lim_a, int lim_b, int length) // 0 string, 1 integer
{
  int x, y;
  int res=NUMERICALLIMIT;
  getyx(win1, y, x);
  
   while (res<lim_a || res>lim_b) {
    memset(input_string, 0, sizeof(input_string));
    Scan_Input(input_string, x+1, y+1, 0, length);
    res=atoi(input_string);
    if (!flag || !res) // res==0 is a non-numerical input_string, if in limits ok to return
     return 0;
   refresh(); } // had to do that as routine stuck in illogical loop 

 return res;
}

// scan input overloaded
char Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color, int length)
{
  int i, t=0, column, fieldreferenceflag=0, fieldreferencelist, dummyfieldreferencelist, fieldreferencerecord=currentrecord;
  char tstring[MAXSTRING], iistring[MAXSTRING];
  
  if ( record.size() && records.size() && record[currentfield].fieldlist && editoroption == -1 ) {
   fieldreferenceflag=1;
   if (fieldhasdependancy==2) {
     fieldreferencerecord=0;
  dummyfieldreferencelist=relationships[externalreferencedatabase].extFields[1]-1; }
  else 
 fieldreferencelist=record[records[(fieldreferencerecord*fieldsperrecord)+currentfield].id].fieldlist-1; }
  
   strcpy(tstring, istring);
   for (i=(int) strlen(istring);i<length+(int) strlen(istring);i++)
    tstring[i]=SPACE;
   tstring[i]='\0';
   if ( length )
    length+=x_pos-1;
   if ( length > 79 )
    length=79;
   if ( color > 64 )
    color=WHITE;
   if ( color > 0 )
    Change_Color(color);
   column=x_pos;
   while (t!=ESC && t!='\n') {
    gotoxy(x_pos, y_pos);
    if (fieldreferenceflag==2) {
     fieldreferenceflag=1;
     if (fieldhasdependancy!=2)
      strcpy(tstring, records[(fieldreferencerecord*fieldsperrecord)+fieldreferencelist].text); 
     else
    strcpy(tstring, externalrecords[externalreferencedatabase][(fieldreferencerecord*dummyfieldsperrecord)+dummyfieldreferencelist].text); }
    printw("%s", tstring);
    if ( length == 79 )
     clrtoeol();
    gotoxy(column, y_pos);
    if (tstring[column-x_pos-1]==SPACE)
     addch(UNDERSCORE);
    gotoxy(column, y_pos);
    refresh();
    cleanstdin();
    t=bgetch(SCANUNBLOCK); // give time for input char, otherwise leave 
    if (t==-1)
     t='\n';
    if (t==PASTE && (int) strlen(clipboard))
     strcpy(tstring, clipboard);
    if (isprintablecharacter(t) && column<length+1) {
     tstring[column-x_pos]=t;
     if (column<length)
    ++column; }
     switch (t) {
      case LEFT:
       if (column>x_pos) {
       --column; }
      break;
      case RIGHT:
       if (column<length) {
       ++column; }
      break;
      case UP:
       if (fieldreferenceflag && fieldreferencerecord>0) {
        --fieldreferencerecord;
       fieldreferenceflag=2; }
      break;
      case DOWN:
       if (fieldhasdependancy==2) {
        if (fieldreferenceflag && fieldreferencerecord<dummyrecordsnumber-1) {
         ++fieldreferencerecord;
       fieldreferenceflag=2; } } 
       else
       if (fieldreferenceflag && fieldreferencerecord<recordsnumber-1) {
        ++fieldreferencerecord;
       fieldreferenceflag=2; }
      break;
      case INSERT:
       if (fieldreferenceflag)
        fieldreferenceflag=2;
       else {
        t=0;
        for (i=0;i<(int) strlen(tstring)-1;i++) {
         if ( i == column - 1 && t < length - 2 )
          iistring[t++]=SPACE;
         if ( t < length - 2 )
          iistring[t++]=tstring[i]; 
        }
        iistring[MAXSTRING-1]='\0';
        strcpy(tstring, iistring); }
      break;
      case BACKSPACE:
       if (column>x_pos) {
        tstring[column-x_pos-1]=SPACE;
        for (i=column-2;i<length;i++)
         tstring[i]=tstring[i+1];
        tstring[i]='\0';
       --column; }
      break;
      case HOME:
       column=1;
      break;
      case END:
       column=(int) strlen(tstring)-1;
       while (isspace(tstring[column]) && column>0)
        --column;
       ++column;
      break;
      case DELETE:
       if ( column < length ) {
        for (i=column-1;i<length;i++)
          tstring[i]=tstring[i+1];
        tstring[i++]=SPACE;
       tstring[i]='\0'; }
    break; } }
    
    terminatestringatcharactersend(tstring);
    if (t=='\n')
     strcpy(istring, tstring);
    
 return t;
}

// scan date
void Scan_Date(int x_pos, int y_pos, char tdate[], int flag) // 0 entire date, 1 month and year
{
  int c=0, y, m, d;
  int highlight=0; // 0 y, 1 m, 2 d
  time_t now = time(0);
  char date[MAXSTRING];
  tm *ltm = localtime(&now);

   y=1900 + ltm->tm_year;
   m=1 + ltm->tm_mon;
   d=ltm->tm_mday;
   if (flag)
    d=1;
   while (c!=ESC && c!='\n') {
    Show_Menu_Bar(1);
    Change_Color(YELLOW);
    strcpy(date, addmissingzeros(itoa(d), 2));
    strcat(date, " ");
    strcat(date, addmissingzeros(itoa(m), 2));
    strcat(date, " ");
    strcat(date, itoa(y));
    strcat(date, " ");
    strcat(date, daysofweek[CalcDayNumFromDate(y, m, d)]);
    gotoxy(x_pos, y_pos);
    printw("date->%s", date);
    Change_Color(YELLOWONCYAN);
    switch (highlight) {
     case 0:
      gotoxy(x_pos+12, y_pos);
      printw("%s", itoa(y));
     break;
     case 1:
      gotoxy(x_pos+9, y_pos);
      printw("%s", addmissingzeros(itoa(m), 2));
     break;
     case 2:
      gotoxy(x_pos+6, y_pos);
      printw("%s", addmissingzeros(itoa(d), 2));
    break; }
    refresh();
    c=getch();
    if (c==0 || c==254)
     c=getch();
    switch (c) {
     case LEFT:
      if (highlight<2-flag)
       ++highlight;
     break;
     case RIGHT:
      if (highlight)
       --highlight;
     break;
     case UP:
      switch (highlight) {
       case 0:
        if (y<32765)
         ++y;
       break;
       case 1:
        ++m;
        if (m>12)
         m=1;
       break;
       case 2:
        ++d;
        if (d>daysinmonth(y, m))
         d=1;
      break; }
     break;
     case DOWN:
      switch (highlight) {
       case 0:
        if (y>1)
         --y;
       break;
       case 1:
        --m;
        if (m<1)
         m=12;
       break;
       case 2:
        --d;
        if (d<1)
         d=daysinmonth(y, m);
      break; }
   break; }
   if (d>daysinmonth(y, m))
  d=daysinmonth(y, m); }
  
  if (c==ESC) 
   return;

  // scan_date for calendar
  if (flag) {
   sprintf(tdate, "%d/%d", m, y);
   return;
  }
  // pass new values to time structure and rearrange
  ltm->tm_year=y-1900;
  ltm->tm_mon=m-1;
  ltm->tm_mday=d;
  strcpy(calendarformat, "%x %X");
  if (isautomaticvalueformatinstruction(currentfield))
   strcpy(calendarformat, record[currentfield].automatic_value);
  strftime(tdate, MAXSTRING, const_cast<char *> (calendarformat), ltm);
}

// add zeros before first number in string
char *addmissingzeros(char tstring[], int length)
{    
  int i;
  char ttstring[(int) strlen(tstring)+(length-(int) strlen(tstring))];
  if ((int) strlen(tstring)==length)
   return &tstring[0];
  strcpy(ttstring, tstring);
  
   for (i=0;i<length-(int) strlen(tstring);i++)
    ttstring[i]='0';
   ttstring[i]='\0';
   strcat(ttstring, tstring);
   strcpy(tstring, ttstring);
   
 return &tstring[0];
}

// terminate string when characters end to cut off extra spaces
void terminatestringatcharactersend(char *ttext)
{
  ttext+=(int) strlen(ttext)-1;
    
   while (isspace(*ttext) && *ttext--);
   if (!(int) strlen(ttext))
    *++ttext=' ';
  *++ttext='\0';
}

// add leading zeros to cover field string length
void addleadingzeros(char ttext[], Annotated_Field *tfield, int flag) // 0 spaces, 1 zeros
{
  int i, n;
  char ttext2[MAXSTRING];
    
  n=record[tfield->id].size.x-(int) strlen(ttext);
  for (i=0;i<n;i++)
   ttext2[i]=(!flag) ? ' ' : '0';
  ttext2[i]='\0';
  strcat(ttext2, ttext);
  strcpy(ttext, ttext2);
}

// add leading spaces to cover desired string length
void addleadingspaces(char ttext[], int overallsize)
{
  int i;
  char ttext2[MAXSTRING];
    
  for (i=0;i<overallsize-(int) strlen(ttext);i++)
   ttext2[i]=' ';
  ttext2[i]='\0';
  strcat(ttext2, ttext);
  strcpy(ttext, ttext2);
}

// count text length without spaces
int fieldlength(char *fieldtext)
{
  int size=((int) strlen(fieldtext)==1 && isspace(fieldtext[0])) ? 0 : (int) strlen(fieldtext);
 
 return size;
}

// enclose string in quotes, add comma if wanted
void stringquotesencloser(char *tstring, int flag) // 1 do not add comma at the end of string
{
  int i;
  char ttstring[MAXSTRING];
  ttstring[0]=QUOTE;
 
   for (i=0;i<(int) strlen(tstring);i++)
    ttstring[i+1]=tstring[i];
   ++i;
   ttstring[i++]=QUOTE;
   if (!flag)
    ttstring[i++]=COMMA;
   ttstring[i]='\0';
   strcpy(tstring, ttstring);
}

// remove quotes from string
void stringquotesopener(char *tstring)
{
  int i, n=0;
  char ttstring[MAXSTRING];
 
   for (i=1;i<(int) strlen(tstring)-1;i++)
    ttstring[n++]=tstring[i];
   ttstring[n]='\0';
   strcpy(tstring, ttstring);
}

// is string in quotes ?
int isinquotes(char tstring[])
{
  if (tstring[0]==QUOTE && tstring[(int) strlen(tstring)-1]==QUOTE)
   return 1;
         
 return 0;
}

// show key pressed
int sgetch(int x_pos, int y_pos, int sleeptime, int showflag)
{
  int t;
  
  // are keys for next loops stored in vector ?
  if (keyonnextloop.size()) {
   t=keyonnextloop[0];
   vector<int>::iterator p=keyonnextloop.begin();
   keyonnextloop.erase(p);
  return t; }
   
    t=getch();
    if ( t == ESC ) { // possible alt held down
     t=bgetch(10);
     if ( t == -1 ) // escape key pressed
      return ESC;
     altpressed=1;
    }
    // mouse activity
    if (t==KEY_MOUSE)
     if(getmouse(&mouse) == OK)
      return KEY_MOUSE;
    // character
    if (isalpha(t)) {
     if (menubar && showflag) {
      printw(".%c", t);
      gotoxy(x_pos, y_pos);
      refresh();
     Sleep(sleeptime); }
    return tolower(t); 
    }
  
  // movement or otherwise
 return t;
}

// getch with block/unblock
int bgetch(int delay)
{
  int t;  
    
   if (delay!=BLOCK)
    blockunblockgetch(delay);
  
   t=getch();
   
   blockunblockgetch();
   
 return t;
}

/* remove garbage from stdin */
void cleanstdin()
{
 int stdin_copy = dup(STDIN_FILENO);

  tcdrain(stdin_copy);
  tcflush(stdin_copy, TCIFLUSH);
 close(stdin_copy);
}

// show a message
void Show_Message(int x_pos, int y_pos, int color, char *message, int sleeptime)
{
  int i, n=0;
  
  Change_Color(color);
  for (i=x_pos;i<x_pos+(int) strlen(message);i++) {
   gotoxy(i, y_pos);
  addch(message[n++]); }
  refresh();
  if (sleeptime>-1)
   Sleep(sleeptime);
  else
   getch();
}

// show a message, const char
void Show_Message(int x_pos, int y_pos, int color, const char *message, int sleeptime)
{
  int i, n=0;
  
  Change_Color(color);
  for (i=x_pos;i<x_pos+(int) strlen(message);i++) {
   gotoxy(i, y_pos);
  addch(message[n++]); }
  refresh();
  if (sleeptime>-1)
   Sleep(sleeptime);
  else
   getch();
}

// replace underscores with spaces
void replaceunderscoresandbrackets(char dataname[], int flag) // 0 place underscore, 1 place SPACE
{
  int i1;
  if (!flag) {
   for (i1=0;i1<(int) strlen(dataname);i1++) {
    if (dataname[i1]==SPACE)
     dataname[i1]='_';
    if (dataname[i1]=='#')
  dataname[i1]='`'; } }
  if (flag) {
   for (i1=0;i1<(int) strlen(dataname);i1++) {
    if (dataname[i1]=='_')
     dataname[i1]=SPACE;
    if (dataname[i1]=='`')
  dataname[i1]='#'; } }
  dataname[i1]='\0';
}

// limit significant numbers
int limitsignificantnumbers(char *s, int digits)
{
  int i=0;
  
   while (*s) {
    if (*s=='.') {
     while (i<digits && *s) 
      if (isdigit(*s++))
       ++i;
    *s='\0'; }
   ++s; }
   if (i<digits)
    return -1;
    
  return 0;
}

// sort records from sequence in parameter array
int sortrecords(int field_id, int recordssequence[], int mode) // 0 ascending 1 descending
{
  int i, n, tlength, loopflag=1, sortflag=0, trecord;
  vector <int> trecordsequece;

   while (loopflag) { // one more turn of string swapping
    for (loopflag=0, i=recordsnumber-1;i>0;sortflag=0, i--) {
     tlength=((int) strlen(records[(recordssequence[i]*fieldsperrecord)+field_id].text)<(int) strlen(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text)) ? (int) strlen(records[(recordssequence[i]*fieldsperrecord)+field_id].text) : (int) strlen(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text);
     for (n=0;n<tlength;n++) {
      if ((int)(records[(recordssequence[i]*fieldsperrecord)+field_id].text[n])<(int)(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text[n]) && !sortflag) {
       sortflag=1; loopflag=1;
       trecord=recordssequence[i-1];
       recordssequence[i-1]=recordssequence[i];
      recordssequence[i]=trecord; }
      if ((int)(records[(recordssequence[i]*fieldsperrecord)+field_id].text[n])>(int)(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text[n]))
    sortflag=1; } } }

    // if mode, reverse array
    if (mode) {
     for (i=recordsnumber-1;i>-1;i--)
      trecordsequece.push_back(recordssequence[i]);
     for (i=0;i<recordsnumber;i++)
    recordssequence[i]=trecordsequece[i]; }
    
  return 0;
}

// return table with sorted by x.pt fields
void sortfieldsbyxpt(int field_id, vector <int> &fieldxidentities)
{
 int i;

   for (i=0;i<fieldsperrecord;i++)
    if ( record[field_id].pt.y == record[i].pt.y && record[i].editable && record[i].active )
     fieldxidentities.push_back(i);
   sortxy(fieldxidentities, X);
}

// return table with sorted by y.pt fields
void sortfieldsbyypt(int field_id, vector <int> &fieldyidentities)
{
 int i;

   for (i=0;i<fieldsperrecord;i++)
    if (record[field_id].pt.x==record[i].pt.x && record[i].editable && record[i].active)
     fieldyidentities.push_back(i);
   sortxy(fieldyidentities, Y);
}

// find first/last active & editable field
int findfieldege(int flag) // 0 first, 1 last
{
  int i, tfield=-1;  
    
   tfield=-1;
   if (!flag) {
   for (i=0;i<fieldsperrecord;i++)
    if (record[i].editable && record[i].active) {
     tfield=i;
   break; } }
   else {
   for (i=fieldsperrecord-1;i>0;i--)
    if (record[i].editable) {
     tfield=i;
   break; } }
    
 return tfield;
}

// locate field by mouse click
int locatefieldbymouseclick()
{
   int x, y, i, field=-1;
   
    for (i=0;i<fieldsperrecord;i++)
     for (x=record[i].pt.x;x<record[i].pt.x+record[i].size.x;x++)
      for (y=record[i].pt.y;y<record[i].pt.y+record[i].size.y;y++)
       if ( mouse.x+1==x && mouse.y+1==y && record[i].editable && record[i].active ) {
        field=i;
         break;
       }
       
  return field;
}

// right mouse button clicked
bool rightmousebuttonclicked()
{
  if (mouse.bstate & BUTTON3_CLICKED)
   return true;

 return false;       
}

// left mouse button double clicked
bool leftmousebuttondoubleclicked()
{
  if (mouse.bstate & BUTTON1_DOUBLE_CLICKED)
   return true;

 return false;       
}

// middle mouse button held down
bool middlemousebuttonpressed()
{
  if (mouse.bstate & BUTTON2_PRESSED)
   return true;

 return false;       
}

// mouse wheel up or down
int wheelmousemove()
{
  if (mouse.bstate & BUTTON4_PRESSED)
   return SHIFT_LEFT;
#ifdef  BUTTON5_PRESSED
  if (mouse.bstate & BUTTON5_PRESSED)
   return SHIFT_RIGHT;
#endif
  
 return 0;
}

// handle ctrl+c
void INThandler(int sig)
{
 char c;
 
    signal(sig, SIG_IGN);
    if ( sig == SIGSEGV )
     End_Program(SEGMENTATIONFAULT);
    if ( sig == SIGFPE )
     End_Program(FLOATINGPOINTEXCEPTION);
    
    Show_Menu_Bar(1);
    Change_Color(RED);
    gotoxy(1,24);
    printw("really quit ?");
    blockunblockgetch(QUITUNBLOCK);
    c = sgetch();
    if ( c == ESC || c == 'y' )
     End_Program();
     
    signal(SIGINT, INThandler);
    Show_Menu_Bar();
}

// does file contain binary
int filecontainsbinary(ifstream* file)
{
 int t;
 
  t=file->peek();
  if ( t > 126 )
   return 1;

 return 0;
}

// add spaces to bring text in the middle of width
char* bringstringtomiddle(char *text, int width)
{
 int i, spacesneeded=0;
 static char newtext[MAXSTRING];

  if ((int) strlen(text)>=width)
   return &text[0];
  spacesneeded=((width-(int) strlen(text))/2)-1;
  
   strcpy(newtext, " \b");
   for (i=0;i<spacesneeded;i++)
    strcat(newtext, " ");
   strcat(newtext, text);
   for (i=(int) strlen(newtext);i<width+1;i++)
    strcat(newtext, " ");
   strcpy(text, newtext);
    
 return &newtext[0];   
}

// scan text for command
int scantextforcommand(char *text, char *command, char separator)
{
   char s[2]= { separator, '\0' };
   char *firstpart, *tcommand;
   tcommand=firstpart=NULL; // empty pointer
   int result=0;
   
// get the first part
   firstpart = strtok(text, s);
   
// obtain command 
   tcommand = strtok(NULL, s);
   
   if (tcommand!=NULL) {
    strcpy(command, tcommand);
   result=1; }

 return result;
}

// kbhit from old and wise
int kbhit(void) {
    static int initialized = 0;

    if (! initialized) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = 1;
    }

    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

// decimate string to key
int decimatestringtokey(char *text)
{
  int key=0, i;
  char ttext[MAXSTRING];
  strcpy(ttext, text);
  
   for (i=0;i<(int) strlen(ttext);i++)
    ttext[i]=tolower(ttext[i]);
  
   if (!strcmp(ttext, "down"))
    key=DOWN;
   if (!strcmp(ttext, "up"))
    key=UP;
   if (!strcmp(ttext, "left"))
    key=LEFT;
   if (!strcmp(ttext, "right"))
    key=RIGHT;
   if (!strcmp(ttext, "shift_left"))
    key=SHIFT_LEFT;
   if (!strcmp(ttext, "shift_right"))
    key=SHIFT_RIGHT;
   if (!strcmp(ttext, "escape") || !strcmp(ttext, "esc"))
    key=ESC;
   if (!strcmp(ttext, "enter"))
    key=ENTER;
   if (!strcmp(ttext, "space"))
    key=SPACE;
   if (!strcmp(ttext, "backspace") || !strcmp(ttext, "bs"))
    key=BACKSPACE;
   if (!strcmp(ttext, "delete") || !strcmp(ttext, "del"))
    key=DELETE;
   if (!strcmp(ttext, "insert") || !strcmp(ttext, "ins"))
    key=INSERT;
   if (!strcmp(ttext, "home"))
    key=HOME;
   if (!strcmp(ttext, "end"))
    key=END;
   if (!strcmp(ttext, "copy"))
    key=COPY;
   if (!strcmp(ttext, "paste"))
    key=PASTE;
   if (!strcmp(ttext, "tab"))
    key=TAB;
   if (!strcmp(ttext, "shit_tab"))
    key=SHIFT_TAB;
   
   // no string found
   if (!key)
    key=(int) tolower(ttext[0]);

 return key;
}


// break text into parts, add to keys vector if suitable
int breaktexttokeys(char *text)
{
  int i;
  char ttext[MAXSTRING];
  const char s[2] = "-";
  char *token;
   
   strcpy(ttext, text);
   /* get the first token */
   token = strtok(ttext, s);
   
   /* walk through other tokens */
   while( token != NULL ) {
    if ((i=decimatestringtokey(token)))
     keyonnextloop.push_back(i);
    
    token = strtok(NULL, s);
   } 
    
 return keyonnextloop.size();
}

// block, unblock getch
int blockunblockgetch(int delay)
{
  wtimeout(win1, delay);
  
 return (delay==-1) ? 0 : 1;
}

// see if automatic value contains format instructions
int isautomaticvalueformatinstruction(int field_id)
{ 
   for (int i=0;i<(int) strlen(record[field_id].automatic_value);i++)
    if (record[field_id].automatic_value[i]==INSTRUCTION)
     return 1;
    
 return 0;
}

// see if automatic value contains script command
int isfieldscriptdirector(int field_id)
{
   if ( record[field_id].type != STRING || record[field_id].decimals == 0 || record[field_id].decimals > ONENTRYANDEXIT || record[field_id].buttonbox != NOBUTTON )
    return NOSCRIPT;
    
   for (int i=0;i<(int) strlen(record[field_id].automatic_value);i++)
    if (record[field_id].automatic_value[i]==COMMAND)
     return i+1;
    
 return 0;
}

// separate thousands and decimals
char *formatmonetarystring(char *text)
{
  if (separatort==0)
   return text;
  
  int i, i1, separatord, length;
  separatord=(separatort==46) ? 44 : 46;
  length=(int) strlen(text);
  
  for (i=0;i<(int) strlen(text);i++)
   if (text[i]=='.') {
    text[i]=separatord;
    break;
   }
   
  while (i) {
   for (i1=0;i1<3 && i;i1++)
    --i;
   if (i1==3 && i) {
    for (i1=length+1;i1>i;i1--)
     text[i1]=text[i1-1];
    text[i1]=separatort;
    ++length;
   }
  }
   
 return text;   
}

// append suffix to numerical fields
char *appendsuffix(char *text, int field_id)
{
  char ttext[MAXSTRING];
  
  if (!strcmp(record[field_id].suffix, EMPTYSTRING))
   return text;
  
  if (!suffixposition) {
   strcpy(ttext, record[field_id].suffix);
  strcat(ttext, text); }
  else {
   strcpy(ttext, text);
   strcat(ttext, record[field_id].suffix);
  }
  
  strcpy(text, ttext);
   
 return text;
}

// align text for single sized y fields
void aligntextsingley(Annotated_Field *field, int alignment, int row)
{
  int y, x, i, spaces;
  int ptx, pty, sizex;
  char ttext[MAXSTRING], ttext2[MAXSTRING], screenpart[MAXSTRING], spacesstring[MAXSTRING];
  
  strcpy(ttext, field->text);
  limitspaces(ttext);
  ptx=record[field->id].pt.x;
  pty=record[field->id].pt.y;
  sizex=record[field->id].size.x;
    
    i=spaces=0; y=pty+row;
    if (row<record[field->id].size.y) {
     for (x=ptx;x<ptx+sizex;x++)
      screenpart[i++]=screen[x][y];
     screenpart[i]='\0';
     spaces=limitspaces(screenpart);
     if (alignment==CENTER)
      spaces/=2;
    }
    for (i=0;i<spaces;i++)
     spacesstring[i]=SPACE;
    spacesstring[i]='\0';
    
    switch (alignment) {
     case TOLEFT:
      strcat(ttext, spacesstring);
     break; 
     case CENTER:
      strcpy(ttext2, spacesstring);
      strcat(spacesstring, ttext);
      strcat(spacesstring, ttext2);
      strcpy(ttext, spacesstring);
     break;
     case TORIGHT:
      strcat(spacesstring, ttext);
      strcpy(ttext, spacesstring);
    break; }
    
   strcpy(field->text, ttext);
    
}

// align text
char* aligntext(char text[MAXSTRING], Annotated_Field *field, int alignment)
{
  int i, printablechars, row=0, operation=1;
  char alltext[MAXSTRING], fieldcopy[MAXSTRING], ttext[MAXSTRING];
  Annotated_Field tfield=*field;
  strcpy(fieldcopy, text);
  
   while (operation) {
    operation=printablechars=0;
    
    for (i=0;i<(int) strlen(fieldcopy);i++) {
     
     if (fieldcopy[i]=='\\') { // instruction follows
      switch (fieldcopy[i+1]) {
       case 'a':
        i+=3;
       break;
       case 'c':
        i+=2;
        while (isdigit(fieldcopy[i]))
         ++i;
       break;
       case 'n': // not allowed
        return &text[0];
//         fieldcopy[i]=' '; // clear next line escape code
//         fieldcopy[i+1]=' ';
//         i+=2;
       break;
       case 'b':
        i+=2;
       break;
       case 'f':
        i+=2;
       break;
       case 'p':
        i+=3;
      break; }
     }
     if (fieldcopy[i]!='\\')
      ++printablechars;
     else
      --i;
     
     if (printablechars==record[field->id].size.x) {
      operation=1;
       break;
     }
    }
    i=(i>(int) strlen(fieldcopy)) ? (int) strlen(fieldcopy) : i;
    extracttextpart(fieldcopy, ttext, 0, i);
    strcpy(tfield.text, ttext);
    aligntextsingley(&tfield, alignment, row);   
    ++row;
    if (row==1)
     strcpy(alltext, tfield.text);
    else
     strcat(alltext, tfield.text);
   }
   
   strcpy(text, alltext);

 return &text[0];
}

// does current page have clock fields
int pagehasclockfields()
{
  for (int i=0;i<fieldsperrecord;i++)
   if (record[i].type==CLOCK)
    return 1;

 return 0;
}

// does field text contain internal link
int isfieldtextlink(Annotated_Field *tfield, int linkparameters[]) // 0 record, 1 field
{
  int i, i1;
  char ttext[MAXSTRING], destination[2][MAXSTRING];
  i=i1=0;
    
   if (record[tfield->id].buttonbox!=NOBUTTON)
    return 0;
   while (tfield->text[i]!='\\' && i<(int) strlen(tfield->text))
    ++i;
   while (tfield->text[i]!='>' && i<(int) strlen(tfield->text))
    ++i;
   if (i++==(int) strlen(tfield->text))
    return 0;
   while (tfield->text[i]!='<' && i<(int) strlen(tfield->text))
    ttext[i1++]=tfield->text[i++];
   ttext[i1]='\0';
   if (i==(int) strlen(tfield->text) && ttext[i1-1]!='<')
    return 0;
   
   if ((assignstringvaluestoarray(ttext, destination, 2)!=2))
    return 0;
   linkparameters[0]=atoi(destination[0]);
   linkparameters[1]=atoi(destination[1]);
//    linkparameters.push_back(atoi(destination[0]));
//    linkparameters.push_back(atoi(destination[1]));
   if (linkparameters[0]<1 || linkparameters[0]>recordsnumber)
    return 0;
   if (linkparameters[1]<1 || linkparameters[1]>fieldsperrecord)
    return 0;
    
 return i1;
}

// is field eligible for pushspaceonfield with words separated by spaces
int isfieldmultipleselection(int field_id)
{
  int i;
  char separator[5];
  strcpy(separator, " ~<>");
  
   i=separatewords(records[(currentrecord*fieldsperrecord)+field_id].text, separatedwords, separator);
   if ( i < 2 || separatedwords[0].formulaPosition[STARTOFWORD] == 0 || (multiplechoiceinstructions(field_id)) > 1 )
    i=0;
          
 return i;
}

// count multiple choice instruction in field text
int multiplechoiceinstructions(int field_id)
{
  int i1, instructioncounter=0;
  
   for (i1=0;i1<(int)strlen(records[(currentrecord*fieldsperrecord)+field_id].text);i1++)
    if ( records[(currentrecord*fieldsperrecord)+field_id].text[i1] == '~' )
     ++instructioncounter;
    
 return instructioncounter;
}

// select multiple field, move instruction
int selectmultiplechoicefield(int field_id)
{
  vector<int> tmultiplechoicefields;
  fieldsadjoiningfields(&records[(currentrecord*fieldsperrecord)+field_id], tmultiplechoicefields);
  multiplechoicefields.clear();
  int i, totalinstructions=0, fieldwithinstruction;
  
   for (i=0;i<(int)tmultiplechoicefields.size();i++)
    if ( isfieldmultipleselection(tmultiplechoicefields[i]) )
     multiplechoicefields.push_back(tmultiplechoicefields[i]);
   if ( tmultiplechoicefields.size() != multiplechoicefields.size() )
    return 0;
   tmultiplechoicefields.clear();
   for (i=0;i<(int)multiplechoicefields.size();i++)
    totalinstructions+=multiplechoiceinstructions(multiplechoicefields[i]);
   if ( totalinstructions > 1 )
    return 0;
   fieldwithinstruction=multiplechoicefields[0];
  
   for (i=0;i<(int)multiplechoicefields.size();i++) {
    if ( multiplechoiceinstructions(multiplechoicefields[i]) == 1 ) {
     fieldwithinstruction=multiplechoicefields[i];
     break;
    }
   }
   moveinstructioninfieldtext(fieldwithinstruction);
   
 return 1;
}

// move instruction in adjoiningfields
void moveinstructioninfieldtext(int field_id)
{
 int i1, instructionpoint, fieldpositioninvector;
 
    if ( isfieldmultipleselection(field_id) == 0 )
     return;
    for (fieldpositioninvector=0;fieldpositioninvector<(int)multiplechoicefields.size();fieldpositioninvector++)
     if ( multiplechoicefields[fieldpositioninvector] == field_id )
      break;

    for (instructionpoint=0;instructionpoint<(int)strlen(records[(currentrecord*fieldsperrecord)+field_id].text);instructionpoint++) {
     if ( records[(currentrecord*fieldsperrecord)+field_id].text[instructionpoint] == '~' ) {
      records[(currentrecord*fieldsperrecord)+field_id].text[instructionpoint]=SPACE;
      for (i1=0;i1<(int)separatedwords.size();i1++) {
       if ( separatedwords[i1].formulaPosition[STARTOFWORD] - 1 == instructionpoint && i1 < (int) separatedwords.size() - 1 ) {
        instructionpoint=separatedwords[i1+1].formulaPosition[STARTOFWORD] - 1;
        break;
       }
       if ( multiplechoicefields.size() == 1 && i1 == (int)separatedwords.size() - 1 )
        break;
      }
      break;
     }
    }
    if ( instructionpoint == (int)strlen(records[(currentrecord*fieldsperrecord)+field_id].text) || i1 == (int)separatedwords.size() ) {
     if ( i1 == (int)separatedwords.size() ) {
      if ( fieldpositioninvector == (int)multiplechoicefields.size() - 1 )
       fieldpositioninvector=0;
      else
       ++fieldpositioninvector;
      isfieldmultipleselection( multiplechoicefields[fieldpositioninvector] ); // separate words
      field_id=multiplechoicefields[fieldpositioninvector];
     }
     instructionpoint=separatedwords[0].formulaPosition[STARTOFWORD] - 1;
    }
    if ( multiplechoicefields.size() > 1 || i1 != (int) separatedwords.size() - 1 )
     records[(currentrecord*fieldsperrecord)+field_id].text[instructionpoint]='~';
    
    if ( autosave ) // save all adjoining
     Write_Fields_Int_Vector(multiplechoicefields);
}

// using readstringentry, assign strings to array of pointers
int assignstringvaluestoarray(char *line, char array[MAXWORDS][MAXSTRING], int entries)
{
  int actualentries=0;
  char tline[MAXSTRING];

   while ((readstringentry(line, tline)) && actualentries<entries)
    strcpy(array[actualentries++], tline);

 return actualentries;
}

// fields adjoining field
int fieldsadjoiningfields(Annotated_Field *tfield, vector<int>& adjoiningfields, int possibilityoption)
{
  int i1, recordid=tfield->id, pos=0;
  adjoiningfields.push_back(recordid);
  Annotated_Field *ttfield;
  
   while (pos<(int) adjoiningfields.size()) {
    ttfield=&records[(currentrecord*fieldsperrecord)+adjoiningfields[pos++]];
    recordid=ttfield->id;
     
     if ( possibilityoption || ttfield->text[(int)strlen(ttfield->text)-1]=='>' ) {
      for (i1=0;i1<fieldsperrecord;i1++)
       if ( possibilityoption || records[(currentrecord*fieldsperrecord)+i1].text[0]=='<' ) {
        if ( (findinintvector(i1, adjoiningfields)) || ((arefieldsneighbours(recordid, i1, possibilityoption)==0)) )
         continue;
        else 
         if ( tfield->id != i1 )
          adjoiningfields.push_back(i1); 
       }
     }
     if ( possibilityoption || ttfield->text[0]== '<' ) {
      for (i1=0;i1<fieldsperrecord;i1++) {
       if ( possibilityoption || records[(currentrecord*fieldsperrecord)+i1].text[(int) strlen(records[(currentrecord*fieldsperrecord)+i1].text)-1]=='>' ) {
        if ( (findinintvector(i1, adjoiningfields)) || ((arefieldsneighbours(i1, recordid, possibilityoption)==0)) ) {
         continue;
        }
       else
        if ( tfield->id != i1 )
         adjoiningfields.push_back(i1);
       }
      }
     }
       
    }
    sortxy(adjoiningfields, X);
    sortxy(adjoiningfields, Y);
//     sortxy(adjoiningfields, XY);

 return (int)adjoiningfields.size();
}

// sort fields by x or y
void sortxy(vector<int>& fieldstosort, int preference) // 0 x, 1 y
{
  int i;
  
    if ( preference == XY ) {
     for (i=X;i<XY;i++)
      sortxy(fieldstosort, i);
     return;
    }
    int operation=1, t, pta, ptb;
      
    while ( operation ) {
     operation=0;
     for (i=0;i<(int) fieldstosort.size()-1;i++) {
      switch ( preference ) {
       case X:
        pta=record[fieldstosort[i+1]].pt.x;
        ptb=record[fieldstosort[i]].pt.x;
       break;
       case Y:
        pta=record[fieldstosort[i+1]].pt.y;
        ptb=record[fieldstosort[i]].pt.y;
       break;
      }
      if (pta<ptb) {
       operation=1;
       t=fieldstosort[i];
       fieldstosort[i]=fieldstosort[i+1];
       fieldstosort[i+1]=t;
      }
     }
    }
}

// fields touch borders
int arefieldsneighbours(int id1, int id2, int possibilityoption) // id1 is always to the left or above of id2, possibilityoption = equal in size
{
  int x1, y1, x2, y2, tresult, result;
  tresult=result=0;

    // horizontal neighbours  
    if (record[id1].pt.x+record[id1].size.x==record[id2].pt.x)
     for (y1=record[id1].pt.y;y1<record[id1].pt.y+record[id1].size.y+1;y1++)
      for (y2=record[id2].pt.y;y2<record[id2].pt.y+record[id2].size.y+1;y2++)
       if (y1==y2)
        tresult=HORIZONTALLY;
    if ( (possibilityoption == 0 && tresult) || (possibilityoption && tresult && record[id1].size.x == record[id2].size.x && record[id1].size.y == record[id2].size.y) )
     result+=tresult;
    
    // vertical neighbours
    tresult=0;
    if (record[id1].pt.y+record[id1].size.y==record[id2].pt.y)
     for (x1=record[id1].pt.x;x1<record[id1].pt.x+record[id1].size.x+1;x1++)
      for (x2=record[id2].pt.x;x2<record[id2].pt.x+record[id2].size.x+1;x2++)
       if (x1==x2)
        tresult=VERTICALLY;
    if ( (possibilityoption == 0 && tresult) || (possibilityoption && tresult && record[id1].size.x == record[id2].size.x && record[id1].size.y == record[id2].size.y) )
     result+=tresult;
    
 return result;
}

// int vector contais element
int findinintvector(int element, vector<int>& tv)
{
  int i;
  
   for (i=0;i<(int) tv.size();i++)
    if (element == tv[i])
     break;
    
 return (i==(int) tv.size()) ? 0 : i;
} 

// generate calendar
char *Generate_Calendar(int m, int y)
{
  const int SPACES=4;
  int i, day, d=1, spaces, daysinmonth;
  static char calendar[256], ttext[MAXSTRING];
    
    sprintf(calendar, "          %s %d>", months[m], y);
    for (i=4;i<7;i++) {
     leavespaces(calendar, SPACES-3);
    sprintf(ttext, "%.3s", daysofweek[i]); 
    strcat(calendar, ttext); }
    for (i=0;i<4;i++) {
     leavespaces(calendar, SPACES-3);
     sprintf(ttext, "%.3s", daysofweek[i]); 
    strcat(calendar, ttext); }
    strcat(calendar, ">");
    day=CalcDayNumFromDate(y, m+1, d);
    if (day>3)
     spaces=(day-4)*SPACES;
    else
     spaces=(SPACES*3)+((day)*SPACES);
    leavespaces(calendar, spaces);
    daysinmonth=(m==1) ? (28 + isleapyear(y)) : 31 - m % 7 % 2;
    for (i=d;i<daysinmonth+1;i++) {
     spaces=(d>9) ? 1 : 0;
     leavespaces(calendar, (SPACES-1)-spaces);
     sprintf(ttext, "%d", d);
     strcat(calendar, ttext);
     day=CalcDayNumFromDate(y, m+1, d);
     if (day==3) // next line after Saturday
      strcat(calendar, ">");
    ++d; }
    
 return &calendar[0];
}

// leave spaces (calendar required)
void leavespaces(char *calendar, int spaces)
{
  int i;
  
   for (i=0;i<spaces;i++)
    strcat(calendar, " ");
}

// is record field eligible for display
int isfielddisplayable(int field_id)
{
  if ( record[field_id].active == 0 || ( record[field_id].type>MIXEDTYPE && record[field_id].type!=CLOCK) || record[field_id].buttonbox == AUTOMATICSCRIPT )
   return 0;

 return 1;
}

// load ascii to multiple fields
int loadasciitofields(int field_id, char *filename)
{
  vector<int> tmultiplechoicefields;
  fieldsadjoiningfields(&records[(currentrecord*fieldsperrecord)+field_id], tmultiplechoicefields, 1);
  int i, i1, correction;
  char c;
  FILE* f=fopen(filename, "r");
  i=i1=0;
  
  if ( !f )
   return 0;
    
    while ( (c=fgetc(f)) != EOF && i < (int)tmultiplechoicefields.size() ) {
     if ( isprintablecharacter(c) )
      records[(currentrecord*fieldsperrecord)+tmultiplechoicefields[i]].text[i1++]=c;
     correction = ( i == 0 ) ? 0 : 1;
     if ( i1 == (int)(record[tmultiplechoicefields[i]].size.x *  record[tmultiplechoicefields[i]].size.y) + correction ) {
      records[(currentrecord*fieldsperrecord)+tmultiplechoicefields[i]].text[i1++]='>';
      records[(currentrecord*fieldsperrecord)+tmultiplechoicefields[i]].text[i1]='\0';
      i1=0; i++;
      if ( i < (int)tmultiplechoicefields.size() - 1 )
       records[(currentrecord*fieldsperrecord)+tmultiplechoicefields[i]].text[i1++]='<';
     }
    }
    records[(currentrecord*fieldsperrecord)+tmultiplechoicefields[i-1]].text[i1]='\0';
   
    if ( autosave ) // save all adjoining
     Write_Fields_Int_Vector(tmultiplechoicefields);
  
   fclose(f);
    
 return (int)tmultiplechoicefields.size();
}

// copy records from adjoining fields to Annotated_Field vector
vector<Annotated_Field>& Records_From_Adjoining_Fields(int field_id)
{
  vector<int> tadjoiningfields;
  int i;
  static vector<Annotated_Field> adjoininingrecordfields;
  fieldsadjoiningfields(&records[(currentrecord*fieldsperrecord)+field_id], tadjoiningfields);
  
   adjoininingrecordfields.clear();
   for (i=0;i<(int)tadjoiningfields.size();i++)
    adjoininingrecordfields.push_back(records[(currentrecord*fieldsperrecord)+tadjoiningfields[i]]);
  
 return adjoininingrecordfields; 
}


// save fields with ids in vector
void Write_Fields_Int_Vector(vector<int> tv, int recordid)
{
 int i;
 
   if ( recordid == -1 )
    recordid=currentrecord;
  
   for (i=0;i<(int)tv.size();i++)
    Read_Write_Field(records[(recordid*fieldsperrecord)+tv[i]], fieldposition(recordid, tv[i]), 1);
}


// save fields with ids in vector
void Write_Fields_AnnotatedField_Vector(vector<Annotated_Field> tv, int recordid)
{
 int i;
 
   if ( recordid == -1 )
    recordid=currentrecord;
  
   for (i=0;i<(int)tv.size();i++)
    Read_Write_Field(records[(recordid*fieldsperrecord)+tv[i].id], fieldposition(recordid, tv[i].id), 1);
}



