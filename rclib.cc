int ctoi(char c);
char *dtoa(long double val);
char *itoa(long int val, int base=10);
char charcoder(char d, int mode=0);
int filecodedecode(char *source, char *destination, int mode=0);
void stringcodedecode(char *source, char *destination, int mode=0);
int tryfile(char *file);
void Show_File_Error(char *filename);
int Copy_File(char *source, char *dest);
void checkpoint(int id, int color=58);
int Scan_Input(int flag=0, int lim_a=0, int lim_b=0, int length=80);
char Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color);
void Scan_Date(int x_pos, int y_pos, char tdate[]);
char *addmissingzeros(char tstring[], int zeros);
void terminatestringatcharactersend(char *ttext);
void addleadingzeros(char ttext[], Annotated_Field *tfield);
int fieldlength(char *fieldtext);
long int filesize(char *filename);
void stringquotesencloser(char *tstring, int flag=0);
void stringquotesopener(char *tstring);
int isinquotes(char tstring[]);
void limitspaces(char *tstring);
int isnotempty(char *tstring);
int numberofdigits(long int n);
int sgetch(int x_pos=78, int y_pos=24, int sleeptime=250, int showflag=1);
void cleanstdin();
void Show_Message(int x_pos, int y_pos, int color, char *message, int sleeptime=1500);
void Show_Message(int x_pos, int y_pos, int color, const char *message, int sleeptime=1500);
void Sleep(int sleepMs) { usleep(sleepMs*1000); } // sleep routine;=)
void replaceunderscoresandbrackets(char dataname[], int flag);
char *ctos(int t);
double bringfractionpartofdivision(int param1, int param2, int scale=10);
int isspace(char t);
int isdecimalseparator(char t);
int isprintablecharacter(char t);
int iscorruptstring(char *tstring);
int limitsignificantnumbers(char *s, int digits);
int find(char text[], char token[]);
int findsimple(char text[], char token[]);
int sortrecords(int field_id, int recordssequence[], int mode=0);
void sortfieldsbyxpt(vector <int> &fieldxidentities);
int findfieldege(int flag=0);
int CalcDayNumFromDate(int y, int m, int d);

// definitions
#define MAXSTRING 80 // characters in a regular string
#define MAXTOKENS 10
#define QUOTE 34
#define COMMA 44

// variables
char input_string[MAXSTRING];
const char *daysofweek[] = {
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
    "Monday",
    "Tuesday"
};

//functions
// remainder from division
int mod (double a, double b)
{
int tmp =a/b;

return a-(b*tmp);
}

// return numerical value of char
int ctoi(char c)
{
 return c-'0';
}

// integer to string
char *itoa(long int val, int base)
{
 	static char buf[32] = {0};
	int sign_flag=0, v = 30;
    buf[31]='\0';
    
	if (!val) {
     buf[v]='0';
    return &buf[v]; }
    if (val<0) {
     val*=-1;
    sign_flag=1; }
    
	for(; val && v ; --v, val /= base)
	 buf[v] = "0123456789abcdef"[val % base];
    if (sign_flag) {
     buf[v]='-';
	return &buf[v]; }
	else
     return &buf[v+1];
}

// dtoa to string
char *dtoa(long double val)
{
  static char result[50];
  int i;
  std::string str = std::to_string(val);
  
  for (i=0;i<str.size();i++)
   result[i]=str[i];
  result[i]='\0';
  
 return &result[0];
}

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
}

// use charcoder to encode-decode string
void stringcodedecode(char *source, char *destination, int mode) // 0 code, 1 decode
{
  while (*source)
   *destination++=charcoder(*source++, mode);
}

// int try if file exists
int tryfile(char *file) 
{
  FILE *f;
   
  f=fopen(file, "r");
  if (!f)
   return 0;
  fclose (f);
  
 return 1;
}

// show error for file access
void Show_File_Error(char *filename)
{
  Change_Color(1);
  gotoxy(1,24);
  if (!strlen(filename))
   strcpy(filename, "<none>");
  printw("error: could not access file %s", filename);
  refresh();
  getchar();
}

/* copy source to destination file */
int Copy_File(char *source, char *dest)
{
  FILE *f;
  FILE *w;
  char c;
  char buf[BUFSIZ];
  size_t size;
  
   f=fopen(source, "rb");
   w=fopen(dest, "wb");
   if (!f || !w)
	return -1;
   
    while (size = fread(buf, 1, BUFSIZ, f)) 
     fwrite(buf, 1, size, w);
	
	fclose(f);
	fclose(w);	
  return 0;
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
  int i, x, y;
  char c;
  int res=-32765;
  getyx(win1, y, x);
  
  for (i=0;i<MAXSTRING;i++)
   input_string[i]=SPACE;
  while (res<lim_a || res>lim_b) {
   i=0;
   while (i<length) {
    c=getch();
    if (isprintablecharacter(c) && c!='\n')
     addch(c);
    if (isspace(input_string[i+1])) {
     addch(UNDERSCORE);
    gotoxy(x+i+2, y+1); }
    refresh();
    if (c==0 || c==254) {
     c=getch();
     if (c==BACKSPACE && i) {
      gotoxy(x+i, y+1);
      printw("  ");
      --i;
     gotoxy(x+i+1,y+1); } }
    else   
     input_string[i++]=c;
    if (c=='\n' || c==ESC) {
     input_string[--i]='\0';
     i=length;
    break; } }
    if (flag)
     res=atoi(input_string);
    else 
     res=lim_a; 
    if (c=='\n')
   break; }
   terminatestringatcharactersend(input_string);
  
 return res;
}

// scan input overloaded
char Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color)
{
  int i, t, column=strlen(istring)-1, fieldreferenceflag=0, fieldreferencelist, fieldreferencerecord=currentrecord;
  char tstring[MAXSTRING], iistring[MAXSTRING];
  
  if (record.size() && records.size()) {
   if (record[records[(currentrecord*fieldsperrecord)+currentfield].id].fieldlist) {
   fieldreferenceflag=1;
  fieldreferencelist=record[records[(fieldreferencerecord*fieldsperrecord)+currentfield].id].fieldlist-1; } }
  
   strcpy(tstring, istring);
   for (i=0;i<79;i++)
    if (i>column || tstring[i]=='\n')
     tstring[i]=SPACE;
   tstring[i]='\0';
   Change_Color(color);
   column=x_pos;
   while (t!=ESC && t!='\n') {
    gotoxy(x_pos, y_pos);
    if (fieldreferenceflag==2) {
     fieldreferenceflag=1;
    strcpy(tstring, records[(fieldreferencerecord*fieldsperrecord)+fieldreferencelist].text); }
    printw("%s", tstring);
    for (i=strlen(tstring);i<79;i++)
     addch(SPACE);
    gotoxy(column, y_pos);
    if (tstring[column-x_pos-1]==SPACE)
     addch(UNDERSCORE);
    gotoxy(column, y_pos);
    refresh();
    cleanstdin();
    t=getch();
    if (t==PASTE && strlen(clipboard))
     strcpy(tstring, clipboard);
    if (isprintablecharacter(t) && column<80) {
     tstring[column-x_pos]=t;
     if (column<79)
    ++column; }
    if (t==0 || t==254) {
     t=getch();
     switch (t) {
      case LEFT:
       if (column>x_pos) {
       --column; }
      break;
      case RIGHT:
       if (column<79) {
       ++column; }
      break;
      case UP:
       if (fieldreferenceflag && fieldreferencerecord>0) {
        --fieldreferencerecord;
       fieldreferenceflag=2; }
      break;
      case DOWN:
       if (fieldreferenceflag && fieldreferencerecord<recordsnumber-1) {
        ++fieldreferencerecord;
       fieldreferenceflag=2; }
      break;
      case INSERT:
       if (fieldreferenceflag)
        fieldreferenceflag=2;
       else {
        t=0;
        for (i=0;i<strlen(tstring)-1;i++) {
         if (i==column-1)
          iistring[t++]=SPACE;
         iistring[t++]=tstring[i]; }
         iistring[MAXSTRING]='\0';
        strcpy(tstring, iistring); }
      break;
      case BACKSPACE:
       if (column>x_pos) {
        tstring[column-x_pos-1]=SPACE;
        for (i=column-2;i<79;i++)
         tstring[i]=tstring[i+1];
        tstring[i++]=SPACE;
        tstring[i]='\0';
       --column; }
      break;
      case HOME:
       column=1;
      break;
      case END:
       column=strlen(tstring)-1;
       while (isspace(tstring[column]) && column>0)
        --column;
       ++column;
      break;
      case DELETE:
       if (column<79) {
        for (i=column-1;i<79;i++)
          tstring[i]=tstring[i+1];
        tstring[i++]=SPACE;
       tstring[i]='\0'; }
    break; } } }
    
    terminatestringatcharactersend(tstring);
    if (t=='\n')
     strcpy(istring, tstring);
    
 return t;
}

// scan date
void Scan_Date(int x_pos, int y_pos, char tdate[])
{
  int weekday, c, y, m, d;
  int highlight=0; // 0 y, 1 m, 2 d
  time_t now = time(0);
  char date[MAXSTRING];
  tm *ltm = localtime(&now);
  
//    cout << "Year" << 1900 + ltm->tm_year<<endl;
//    cout << "Month: "<< 1 + ltm->tm_mon<< endl;
//    cout << "Day: "<<  ltm->tm_mday << endl;
//    cout << "Time: "<< 1 + ltm->tm_hour << ":";
//    cout << 1 + ltm->tm_min << ":";
//    cout << 1 + ltm->tm_sec << endl;
   y=1900 + ltm->tm_year;
   m=1 + ltm->tm_mon;
   d=ltm->tm_mday;
   while (c!=ESC && c!='\n') {
    Show_Menu_Bar(1);
    Change_Color(3);
    strcpy(date, addmissingzeros(itoa(d), 2));
    strcat(date, " ");
    strcat(date, addmissingzeros(itoa(m), 2));
    strcat(date, " ");
    strcat(date, itoa(y));
    strcat(date, " ");
    strcat(date, daysofweek[CalcDayNumFromDate(y, m, d)]);
    gotoxy(x_pos, y_pos);
    printw("date->%s", date);
    Change_Color(24);
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
      if (highlight<2)
       ++highlight;
     break;
     case RIGHT:
      if (highlight)
       --highlight;
     break;
     case UP:
      switch (highlight) {
       case 0:
        ++y;
       break;
       case 1:
        ++m;
       break;
       case 2:
        ++d;
      break; }
     break;
     case DOWN:
      switch (highlight) {
       case 0:
        --y;
       break;
       case 1:
        --m;
       break;
       case 2:
        --d;
      break; }
  break; } }

  if (c=='\n')
   strcpy(tdate, date);
}

// add zeros before first number in string
char *addmissingzeros(char tstring[], int length)
{    
  int i;
  char ttstring[strlen(tstring)+(length-strlen(tstring))];
  if (strlen(tstring)==length)
   return &tstring[0];
  strcpy(ttstring, tstring);
  
   for (i=0;i<length-strlen(tstring);i++)
    ttstring[i]='0';
   ttstring[i]='\0';
   strcat(ttstring, tstring);
   strcpy(tstring, ttstring);
   
 return &tstring[0];
}

// terminate string when characters end to cut off extra spaces
void terminatestringatcharactersend(char *ttext)
{
  ttext+=strlen(ttext)-1;
    
   while (isspace(*ttext) && *ttext--);
   if (!strlen(ttext))
    *++ttext=' ';
  *++ttext='\0';
}

// add leading zeros to cover field string length
void addleadingzeros(char ttext[], Annotated_Field *tfield)
{
  int i, n;
  char ttext2[MAXSTRING];
    
  n=record[records[(currentrecord*fieldsperrecord)+tfield->id].id].size.x-strlen(ttext);
  for (i=0;i<n;i++)
   ttext2[i]='0';
  ttext2[i]='\0';
  strcat(ttext2, ttext);
  strcpy(ttext, ttext2);
}

// count text length without spaces
int fieldlength(char *fieldtext)
{
  int size=(strlen(fieldtext)==1 && isspace(fieldtext[0])) ? 0 : strlen(fieldtext);
 
 return size;
}

// measure file size
long int filesize(char *filename)
{
  long int i=0;
  char t;
  ifstream testfile(filename, ios::binary);
  
   while ((t=testfile.get())!=EOF && testfile)
    ++i;
   testfile.close();
    
 return i;
}

// enclose string in quotes, add comma if wanted
void stringquotesencloser(char *tstring, int flag) // 1 do not add comma at the end of string
{
  int i;
  char ttstring[MAXSTRING];
  ttstring[0]=QUOTE;
 
   for (i=0;i<strlen(tstring);i++)
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
 
   for (i=1;i<strlen(tstring)-1;i++)
    ttstring[n++]=tstring[i];
   ttstring[n]='\0';
   strcpy(tstring, ttstring);
}

// is string in quotes ?
int isinquotes(char tstring[])
{
  if (tstring[0]==QUOTE && tstring[strlen(tstring)-1]==QUOTE)
   return 1;
         
 return 0;
}

// trim spaces at start and end of string
void limitspaces(char *tstring)
{
  int i=0, n=0;
  char ttstring[MAXSTRING];
  
  while (isspace(tstring[i]) && i<strlen(tstring))
   ++i;
  for (;i<strlen(tstring);i++)
   ttstring[n++]=tstring[i];
  ttstring[n]='\0';
  i=strlen(ttstring)-1;
  while (isspace(ttstring[i]))
   --i;
  ttstring[++i]='\0';
  if (!strlen(ttstring))
   strcpy(ttstring, " ");
  strcpy(tstring, ttstring);
}

// is string composed solely of spaces ?
int isnotempty(char *tstring)
{
  int i=0;
  
   while (*tstring)
    if (isprintablecharacter(*tstring) && !isspace(*tstring++))
     ++i;
    
 return i;
}

// number of digits
int numberofdigits(long int n)
{
  int i=0;
  
   while (n) {
    n/=10;
   ++i; }
  
 return i;
}  

// show key pressed
int sgetch(int x_pos, int y_pos, int sleeptime, int showflag)
{
  int t;
  
  t=getch();
  if (isalpha(t)) {
   if (menubar && showflag) {
    printw(".%c", t);
    gotoxy(x_pos, y_pos);
    refresh(); 
   Sleep(sleeptime); }
  return tolower(t); }
  if (t==TAB)
   t=DOWN;
  if (t==0 || t==254) {
   t=getch();
   if (t==-109 || t==SHIFT_TAB) // possible SHIFT_TAB
  t=UP; }
  
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
  for (i=x_pos;i<x_pos+strlen(message);i++) {
   gotoxy(i, y_pos);
  addch(message[n++]); }
  refresh();
  Sleep(sleeptime);
}

// show a message, const char
void Show_Message(int x_pos, int y_pos, int color, const char *message, int sleeptime)
{
  int i, n=0;
  
  Change_Color(color);
  for (i=x_pos;i<x_pos+strlen(message);i++) {
   gotoxy(i, y_pos);
  addch(message[n++]); }
  refresh();
  Sleep(sleeptime);
}

// replace underscores with spaces
void replaceunderscoresandbrackets(char dataname[], int flag) // 0 place underscore, 1 place SPACE
{
  int i1;
  if (!flag) {
   for (i1=0;i1<strlen(dataname);i1++) {
    if (dataname[i1]==SPACE)
     dataname[i1]='_';
    if (dataname[i1]=='#')
  dataname[i1]='`'; } }
  if (flag) {
   for (i1=0;i1<strlen(dataname);i1++) {
    if (dataname[i1]=='_')
     dataname[i1]=SPACE;
    if (dataname[i1]=='`')
  dataname[i1]='#'; } }
  dataname[i1]='\0';
}

// char to null terminated string
char *ctos(int t)
{
  static char s[3];
  
   s[0]=(char) t;
   s[1]='\0';
   
 return s;
}


// divide param1/param2 and bring fraction part*scale
double bringfractionpartofdivision(int param1, int param2, int scale)
{
  double param = (double) param1/param2;
  double fractpart, intpart;
  
  fractpart = modf (param , &intpart);
 return fractpart*scale;
}

// isspace
int isspace(char t)
{
  if (t==32) 
   return 1;
  
 return 0;
}

// isdecimalseparator
int isdecimalseparator(char t)
{
  if (t==',' || t=='.')
   return 1;
  
 return 0;
}

// is printable character
int isprintablecharacter(char t)
{
  if (t>31 && t<127)
   return 1;
  
 return 0;
}

// corrupt string check
int iscorruptstring(char *tstring)
{
    while (*tstring) {
     if (!isprintablecharacter(*tstring) && (int)*tstring>0 && *tstring!='\n')
      return 1;
     ++tstring; }
    
  return 0;
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

// find part in text
int find(char text[], char token[])
{ 
  int i, n, arraypositions=0, hits=0, tokenstried=0;
  char ttoken[MAXSTRING], tokens[MAXTOKENS][MAXSTRING];
  
   for (i=0;i<strlen(token);i++) {
    n=0;
    while (token[i]!='*' && i<strlen(token))
    ttoken[n++]=tolower(token[i++]);
    ttoken[n]='\0';
   strcpy(tokens[arraypositions++], ttoken); }
   i=0;
   while (tokenstried<arraypositions && i<strlen(text)) { // token to look for
    for (;i<strlen(text);i++) {
     if (tolower(text[i])==tokens[tokenstried][0]) {
      for (n=0;n<strlen(tokens[tokenstried]);n++)
       ttoken[n]=tolower(text[i+n]);
      ttoken[n]='\0';
      if (!strcmp(ttoken, tokens[tokenstried])) {
       ++hits;
      i+=n;
   ++tokenstried; } } } }
   
 return (hits==arraypositions) ? 1 : 0;;
}

// find command in text
int findsimple(char text[], char token[])
{ 
  int i, n, hit=0;
  char ttoken[MAXSTRING];
  
   for (i=0;i<strlen(text);i++) {
    if (tolower(text[i])==token[0]) {
     hit=i+1;
     for (n=0;n<strlen(token);n++)
      ttoken[n]=tolower(text[i+n]);
     ttoken[n]='\0';
     if (strcmp(ttoken, token))
      hit=0; 
     else
   break; } }
   
 return hit;
}

// sort records from sequence in parameter array
int sortrecords(int field_id, int recordssequence[], int mode) // 0 ascending 1 descending
{
  int i, n, tlength, loopflag=1, sortflag=0, trecord;
  vector <int> trecordsequece;

   while (loopflag) { // one more turn of string swapping
    for (loopflag=0, i=recordsnumber-1;i>0;sortflag=0, i--) {
     tlength=(strlen(records[(recordssequence[i]*fieldsperrecord)+field_id].text)<strlen(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text)) ? strlen(records[(recordssequence[i]*fieldsperrecord)+field_id].text) : strlen(records[(recordssequence[i-1]*fieldsperrecord)+field_id].text);
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
void sortfieldsbyxpt(vector <int> &fieldxidentities)
{
 int i, t, operation=1;

   for (i=0;i<fieldsperrecord;i++)
    if (record[records[(currentrecord*fieldsperrecord)+currentfield].id].pt.y==record[records[(currentrecord*fieldsperrecord)+i].id].pt.y && record[i].editable && record[i].active)
     fieldxidentities.push_back(i);
   i=0;
   while (operation) {
    operation=0;
    for (i=0;i<fieldxidentities.size()-1;i++) {
     if (fieldxidentities[i+1]<fieldxidentities[i]) {
      operation=1;
      t=fieldxidentities[i];
      fieldxidentities[i]=fieldxidentities[i+1];
   fieldxidentities[i+1]=t; } } }
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
    if (record[records[(currentrecord*fieldsperrecord)+i].id].editable) {
     tfield=i;
   break; } }
    
 return tfield;
}

// ----------------------------------------------------------------------
// Given the year, month and day, return the day number.
// (see: https://alcor.concordia.ca/~gpkatch/gdate-method.html)
// ----------------------------------------------------------------------
int CalcDayNumFromDate(int y, int m, int d)
{
  m = (m + 9) % 12;
  y -= m / 10;
  int dn = 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d - 1);

  return dn % 7;
}



