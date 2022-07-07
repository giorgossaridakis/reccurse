// reccurse, the filemaker of ncurses
#include "reccurse.h"

const double version=0.467;

int main(int argc, char *argv[])
{
  int i;
  
  Init_Screen();
  Change_Color(WHITEONBLACK);
  // wait until terminal window becomes 80x24
  struct winsize w;
  ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
  while (w.ws_row != 24 || w.ws_col != 80) {
   gotoxy(1,1);
   printw("terminal size %dx%d, reccurse requires 80x24               ", w.ws_col, w.ws_row);
   refresh();
   sleep(1);
   ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
  }
  clear();
  char tmessage[MAXSTRING*MAXPAGES], tfile[MAXSTRING];
  signal(SIGINT, INThandler);
  initiatemathematicalfunctions();
  Set_Mouse_Menus();

   Intro_Screen();
   
    if (argc>1) {
     strcpy(rcfile, argv[1]);
     strcpy(dbfile, argv[1]);
     Reccurse_File_Extension(rcfile, 1);
    Reccurse_File_Extension(dbfile, 2); }
    if (!tryfile(rcfile) && !tryfile(dbfile) ) {
     strcpy(tmessage, "open file:");
     Show_Message(8, 19, 4, tmessage, 0);
     if ( argc > 1 )
      strcpy(input_string, argv[1]);
     Scan_Input(input_string, 18, 19, 5);
     limitspaces(input_string);
     if (!strlen(input_string)) {
      Show_File_Error(input_string);
     End_Program(-1); }
     strcpy(rcfile, input_string);
     strcpy(dbfile, rcfile);
     Reccurse_File_Extension(rcfile, 1); 
    Reccurse_File_Extension(dbfile, 2); }
    // try to find .dbfile, if not read_rc_file routine
    strcpy(tmessage, "loaded file: ");
    if (!tryfile(dbfile)) {
     i=Read_rc_File();
     if (i==-1) {
      // create field, write to file, do not exit
      ofstream outdbfile(dbfile, ios::binary);
      outdbfile.close();
      ofstream outrcfile(rcfile);
      outrcfile << "#date&time 0 000000000 3 10 5 10 2 000000000 5 1 1 1 2 $ 0 0 1 1 . " << endl << "#";
      outrcfile.close();
      strcpy(tmessage, "created file: ");
    Read_rc_File(); } }
    // setup pages workgroup
    strcpy(pages[pagesnumber++], dbfile);
    strcat(tmessage, dbfile);
    for (i=2;i<argc && i<MAXPAGES+1;i++) {
     strcpy( tfile, argv[i] );
     Reccurse_File_Extension(tfile, 2);
     if ( tryfile(tfile) ) 
      strcpy(pages[pagesnumber++], tfile);
    }
    if ( pagesnumber > 1 ) {
     strcat(tmessage, " and ");
     strcat(tmessage, itoa(pagesnumber-1));
    strcat(tmessage, " more"); }
    // read record fields and records from dbfile
    Load_Database(currentpage);
    Show_Message(8, 20, 2, tmessage, 2250);
    Show_Record_and_Menu();
   
  End_Program(0);
  
 return 0;
}

// introduction screen
void Intro_Screen()
{
   char c;
   FILE *f;
   const char *pic=
   "       OOOOOO  OOOOOOO  OOOOOO  OOOOOO OO    OO OOOOOO  OOOOOOO OOOOOOO\n       OO   OO OO      OO      OO      OO    OO OO   OO OO      OO\n       OO   OO OO      OO      OO      OO    OO OO   OO OO      OO\n       OO   OO OO      OO      OO      OO    OO OO   OO OO      OO\n       OOOOOO  OOOOO   OO      OO      OO    OO OOOOOO  OOOOOOO OOOOO  \n       OO   OO OO      OO      OO      OO    OO OO   OO      OO OO \n       OO   OO OO      OO      OO      OO    OO OO   OO      OO OO \n       OO   OO OO      OO      OO      OO    OO OO   OO      OO OO \n       OO   OO OOOOOOO  OOOOOO  OOOOOO  OOOOOO  OO   OO OOOOOOO OOOOOOO\n";
    
    // intro screen
     Change_Color(34);
     gotoxy(27,1);
     printw("Reccurse version %.3f", version);
     attron(A_BLINK);
     gotoxy(1,6);
     while (*pic) {
      Change_Color(BLACK);
      if (isalpha(*pic))
       Change_Color(24);
      addch(*(pic++));
     }
     attroff(A_BLINK);
     Change_Color(2);
     gotoxy(55, 5);
     printw("multipage edition");
     Change_Color(MAGENTA);
     gotoxy(8,15);
     printw("the record maker with Linux ncurses");
    
  refresh();
}

// goodbye
int End_Program(int code)
{
  if (!code)
   checkalteredparameters();
  
  Clean_Database(dbfile); 
  Read_Write_Current_Parameters(4, 1);
  char tmessage[MAXSTRING];
  strcpy(tmessage, "database closed, exiting reccurse with code ");
  strcat(tmessage, itoa(code, 10));
  strcat(tmessage, "...");
    
  Show_Menu_Bar(1);
  Show_Message(1, 24, 1, tmessage, 1500);
  End_Screen();
  
 exit(EXIT_SUCCESS);
}

// altered parameters
int checkalteredparameters()
{
 char c;   
    
  if (!alteredparameters)
   return 0;

   Show_Message(1, 24, 2, "save altered parameters (y/n):", 0);
   c=sgetch();
   if (tolower(c)=='y')
    Read_Write_db_File(4);
   alteredparameters=0;

  return 1;
}

// filename extension add/remove
char* Reccurse_File_Extension(char *filename, int flag) // 0 remove, 1 add .rc, 2 add .db, 3 add .out
{
  int i;
  
  switch (flag) {
   case 0:
    i=strlen(filename);
    while (i)
     if (filename[i--]=='.')
    filename[i+1]='\0';
   break;
   case 1:
    Reccurse_File_Extension(filename, 0);
    strcat(filename, ".rc");
   break;
   case 2:
    Reccurse_File_Extension(filename, 0);
    strcat(filename, ".db");
   break;
   case 3:
    Reccurse_File_Extension(filename, 0);
    strcat(filename, ".out");
  break; }
  
 return &filename[0];
}

// read .rc file
int Read_rc_File()
{
  int i;
  char c;
  Field tfield;
  
  ifstream rcinput(rcfile); 
  if (!rcinput)
   return -1;
  
  record.clear();
  records.clear();
  
   while (c!='#') // read until start of fields
    rcinput.get(c);
   c='.';
   while (rcinput) {
    c=rcinput.peek();
    if (c=='#' || c==EOF)
     break;
    tfield.id=fieldsperrecord;
    Read_Record_Field(rcinput, tfield);
    record.push_back(tfield);
   ++fieldsperrecord; }
   vector<Field>::iterator p=record.end(); // delete last read
   --p;
   record.erase(p);
   --fieldsperrecord;
   rcinput.close();
    
   Reccurse_File_Extension(dbfile, 2);
   Read_Write_db_File(2);
   Initialize_Record();
   
 return 0;
}

int Write_rc_File(char *file)
{
  int i;
  ofstream outrcfile(file);
  if (!file)
   return -1;

    outrcfile << "#" << endl;
    for (i=0;i<record.size();i++) {
     Write_Record_Field(outrcfile, record[i]);
    outrcfile << endl; }
    outrcfile << "#";
    outrcfile.close();
     
  return 0;
}

// read-write .db file
int Read_Write_db_File(int mode) // 0 read, 1 write, 2 create from file, 3&4 recreate from istringstream
{
  int i, i1=0;
  char c;
  char ttext2[FIELDSIZE];
  Annotated_Field tfield;
  Field tfield2;
  string ttext;
  vector<Field>::iterator p;
  vector<Annotated_Field>::iterator s;
  
  fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);
  dbfileaccess.seekg(0, ios::beg);
  dbfileaccess.seekp(0, ios::beg);
  
   if (!mode) { // out of switch due to initializations
    int activefields=0;
    startofrecords=0;
    fieldsperrecord=0;
    record.clear();
    dbfileaccess.seekg(0, ios::beg);
    while (c!='#') {
     dbfileaccess.get(c);
     c=charcoder(c, 1);
     ++startofrecords;
    ttext+=c; }
    char tttext[ttext.size()+1];
    for (i=0;i<ttext.size();i++)
     tttext[i]=ttext[i];
    tttext[i]='\0';
    istringstream ttext2(tttext);
    if (iscorruptstring(tttext)) {
     Show_Message(1, 24, 1, "corrupt database file!", 1500);
    End_Program(-3); }
    while (ttext2) {
     c=ttext2.peek();
     if (c=='#' || c==EOF)
      break;
     tfield2.id=fieldsperrecord;
     Read_Record_Field(ttext2, tfield2);
     record.push_back(tfield2);
    ++fieldsperrecord; }
    --fieldsperrecord;
    p=record.end(); // delete last read
    --p;
    record.erase(p);
    // now read records
    dbfileaccess.close();
    records.clear();
    recordsnumber=0;
    i=0;
    while (!i) {
     i=Read_Write_Field(tfield, fieldserialtofieldposition(i1++));
     if (record[tfield.id].active)
      ++activefields;
    records.push_back(tfield); }
    s=records.end(); // delete last read
    --s;
    records.erase(s);
    recordsnumber=records.size()/fieldsperrecord;
    if (!activefields) {
     Show_Message(1, 24, 1, "no active fields in database");
    End_Program(-2); }
   Read_Write_Relationships(); }
   
   if (mode==3) { // recreate rc data into string and rewrite dbfile, relationships
    char ttext3[MAXSTRING*5];
    dbfileaccess.close();
    remove(dbfile);
    startofrecords=0;
    ofstream outdbfile(dbfile, ios::out | ios::binary);
    for (i=0;i<record.size();i++) {
     Write_Record_Field(ttext3, record[i]);
     stringcodedecode(ttext3, ttext3);
     startofrecords+=strlen(ttext3);
     for (i1=0;i1<strlen(ttext3);i1++)
    outdbfile.put(ttext3[i1]); }
    outdbfile.put(charcoder('#', 0));
    ++startofrecords;
    outdbfile.close(); 
   Read_Write_Relationships(1); }

   if (mode==4) { // only rewrite rcdata, relationships
    char ttext3[MAXSTRING*5];
    fstream outdbfile(dbfile, ios::in | ios::out | ios::binary);
    startofrecords=0;
    for (i=0;i<record.size();i++) {
     Write_Record_Field(ttext3, record[i]);
     stringcodedecode(ttext3, ttext3);
     startofrecords+=strlen(ttext3);
     for (i1=0;i1<strlen(ttext3);i1++)
    outdbfile.put(ttext3[i1]); }
    outdbfile.put(charcoder('#', 0));
    ++startofrecords;
    outdbfile.close(); 
   Read_Write_Relationships(1); }
   
   switch (mode) {
    case 1:
     for (i=0;i<records.size();i++)
      Read_Write_Field(records[i], fieldserialtofieldposition(i), 1);
     if (currentfield<findfieldege() && currentfield<findfieldege(1))
      currentfield=findfieldege();
     for (i=0;i<5;i++)
      Read_Write_Current_Parameters(i, 1);
    break;
    case 2:
    // create dbfile from source file
     ifstream inrcfile(rcfile);
     ofstream outdbfile(dbfile, ios::out | ios::binary);
     while (c!='#')
      inrcfile.get(c);
     if (!isalpha(inrcfile.peek()))
      inrcfile.get(c);
     c='.';
     while (c!='#' && inrcfile) {
      inrcfile.get(c);
      ++startofrecords;
      if (c!=EOF) 
     outdbfile.put(charcoder(c, 0)); }
     inrcfile.close();
     outdbfile.close();
     Read_Write_Relationships(1);
    break; }
    
   dbfileaccess.flush();
   dbfileaccess.close();
  
 return 0;
}

// read external .db files to vectors
int Read_External_Database(int externaldatabaseid)
{
  int i, activefields=0, tfieldsperrecord=0, tstartofrecords=0;
  char c, ttext3[FIELDSIZE];
  string ttext;
  Field tfield2;
  Annotated_Field tfield;
  vector<Annotated_Field>::iterator s;
  vector<Field>::iterator p;
  ifstream externaldatabase(relationships[externaldatabaseid].extDbname, ios::binary);
  if (!externaldatabase)
   return -1;

  // clear all relationships, only requested in memory
  for (i=0;i<relationships.size();i++) {
   externalrecord[i].clear();
  externalrecords[i].clear(); }
   
    while (c!='#') {
     externaldatabase.get(c);
     c=charcoder(c, 1);
    ttext+=c; }
    char tttext[ttext.size()+1];
    for (i=0;i<ttext.size();i++)
     tttext[i]=ttext[i];
    tttext[i]='\0';
    istringstream ttext2(tttext);
    if (iscorruptstring(tttext)) {
     Show_Message(1, 24, 1, "corrupt database file!", 1500);
    End_Program(-3); }
    while (ttext2) {
     c=ttext2.peek();
     if (c=='#' || c==EOF)
      break;
     tfield2.id=tfieldsperrecord;
     Read_Record_Field(ttext2, tfield2);
     ++tfieldsperrecord;
    externalrecord[externaldatabaseid].push_back(tfield2); }
    p=externalrecord[externaldatabaseid].end(); // delete last read
    --p;
    externalrecord[externaldatabaseid].erase(p);
    // now read records
    for (i=0;i<RELATIONSHIPSLENGTH+11;i++)
     externaldatabase.get(c);
    while (externaldatabase) {
     for (i=0;i<FIELDSIZE;i++) {
      if (!externaldatabase)
       break;
     externaldatabase.get(ttext3[i]);
     ttext3[i]=charcoder(ttext3[i], 1); }
     istringstream ttfield(ttext3);
     ttfield >> tfield.id;
     ttfield >> tfield.number;
     ttfield >> tfield.text;
     ttfield >> tfield.formula; 
     replaceunderscoresandbrackets(tfield.text, 1);
     replaceunderscoresandbrackets(tfield.formula, 1);
     if (externalrecord[externaldatabaseid][tfield.id].active)
      ++activefields;
    externalrecords[externaldatabaseid].push_back(tfield); }
    s=externalrecords[externaldatabaseid].end(); // delete last read
    --s;
    externalrecords[externaldatabaseid].erase(s);
    if (!activefields)
   return -1;
    
   externaldatabase.close();
    
 return 0;
}

// read & write current record
int Read_Write_Current_Parameters(int item, int mode) // item:0 currentrecord, 1 currentmenu, 2 menubar 3 autosave 4 currentfield| mode:0 read, 1 write
{
  int i, tvalue;
  char *ttext=new char[5];
  int parameterpositions[]={ 0, 4, 5, 6, 7 };
  int parametersize=(!item || item==4) ? 4 : 1;
  fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);

  if (!mode) {
   dbfileaccess.seekg(RELATIONSHIPSLENGTH+startofrecords+parameterpositions[item], ios::beg);
   for (i=0;i<parametersize;i++)
    dbfileaccess.get(ttext[i]);
   ttext[i]='\0';
   stringcodedecode(ttext, ttext, 1);
  tvalue=atoi(ttext); }
  else {
   dbfileaccess.seekp(RELATIONSHIPSLENGTH+startofrecords+parameterpositions[item], ios::beg);
   switch (item) {
    case 0:
     i=currentrecord;
    break;
    case 1:
     i=currentmenu;
    break;
    case 2:
     i=menubar;
    break;
    case 3:
     i=autosave;
    break;
    case 4:
     i=currentfield;
   break; }
   strcpy(ttext, itoa(i));
   for (i=strlen(ttext);i<4;i++)
    ttext[i]=SPACE;
   ttext[i]='\0';
   stringcodedecode(ttext, ttext);
   for (i=0;i<parametersize;i++)
  dbfileaccess.put(ttext[i]); }
  dbfileaccess.close();
    
 return tvalue;
}

// read record fields from ifstream 
void Read_Record_Field(ifstream &instream, Field &tfield)
{    
    instream >> tfield.title;
    replaceunderscoresandbrackets(tfield.title, 1);
    instream >> tfield.title_position;
    instream >> tfield.title_attributes;
    instream >> tfield.title_color;
    instream >> tfield.pt.x;
    instream >> tfield.pt.y;
    instream >> tfield.size.x;
    instream >> tfield.size.y;
    instream >> tfield.attributes;
    instream >> tfield.color;
    instream >> tfield.box;
    instream >> tfield.box_color;
    instream >> tfield.type;
    instream >> tfield.decimals;
    instream >> tfield.suffix;
    replaceunderscoresandbrackets(tfield.suffix, 1);
    instream >> tfield.formula;
    instream >> tfield.fieldlist;
    instream >> tfield.editable;
    instream >> tfield.active;
    instream >> tfield.automatic_value;
    replaceunderscoresandbrackets(tfield.automatic_value, 1);
}

// read record fields from istringstream 
void Read_Record_Field(istringstream &instream, Field &tfield)
{    
    instream >> tfield.title;
    replaceunderscoresandbrackets(tfield.title, 1);
    instream >> tfield.title_position;
    instream >> tfield.title_attributes;
    instream >> tfield.title_color;
    instream >> tfield.pt.x;
    instream >> tfield.pt.y;
    instream >> tfield.size.x;
    instream >> tfield.size.y;
    instream >> tfield.attributes;
    instream >> tfield.color;
    instream >> tfield.box;
    instream >> tfield.box_color;
    instream >> tfield.type;
    instream >> tfield.decimals;
    instream >> tfield.suffix;
    replaceunderscoresandbrackets(tfield.suffix, 1);
    instream >> tfield.formula;
    instream >> tfield.fieldlist;
    instream >> tfield.editable;
    instream >> tfield.active;
    instream >> tfield.automatic_value;
    replaceunderscoresandbrackets(tfield.automatic_value, 1);
}

// write record fields to ofstream
void Write_Record_Field(ofstream &outstream, Field &tfield)
{
    replaceunderscoresandbrackets(tfield.title, 0);
    outstream << tfield.title;
    replaceunderscoresandbrackets(tfield.title, 1);
    outstream << " ";
    outstream << tfield.title_position;
    outstream << " ";
    outstream << tfield.title_attributes;
    outstream << " ";
    outstream << tfield.title_color;
    outstream << " ";
    outstream << tfield.pt.x;
    outstream << " ";
    outstream << tfield.pt.y;
    outstream << " ";
    outstream << tfield.size.x;
    outstream << " ";
    outstream << tfield.size.y;
    outstream << " ";
    outstream << tfield.attributes;
    outstream << " ";
    outstream << tfield.color;
    outstream << " ";
    outstream << tfield.box;
    outstream << " ";
    outstream << tfield.box_color;
    outstream << " ";
    outstream << tfield.type;
    outstream << " ";
    outstream << tfield.decimals;
    outstream << " ";
    replaceunderscoresandbrackets(tfield.suffix, 0);
    outstream << tfield.suffix;
    replaceunderscoresandbrackets(tfield.suffix, 1);
    outstream << " ";
    outstream << tfield.formula;
    outstream << " ";
    outstream << tfield.fieldlist;
    outstream << " ";
    outstream << tfield.editable;
    outstream << " ";
    outstream << tfield.active;
    outstream << " ";
    replaceunderscoresandbrackets(tfield.automatic_value, 0);
    outstream << tfield.automatic_value;
    replaceunderscoresandbrackets(tfield.automatic_value, 1);
    outstream << " ";
}

// write record fields to string
void Write_Record_Field(char *ttext, Field &tfield)
{
    replaceunderscoresandbrackets(tfield.title, 0);
    strcpy(ttext, tfield.title);
    replaceunderscoresandbrackets(tfield.title, 1);
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.title_position));
    strcat(ttext, " ");
    strcat(ttext, tfield.title_attributes);
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.title_color));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.pt.x));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.pt.y));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.size.x));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.size.y));
    strcat(ttext, " ");
    strcat(ttext, tfield.attributes);
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.color));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.box));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.box_color));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.type));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.decimals));
    strcat(ttext, " ");
    replaceunderscoresandbrackets(tfield.suffix, 0);
    strcat(ttext, tfield.suffix);
    replaceunderscoresandbrackets(tfield.suffix, 1);
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.formula));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.fieldlist));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.editable));
    strcat(ttext, " ");
    strcat(ttext, itoa(tfield.active));
    strcat(ttext, " ");
    replaceunderscoresandbrackets(tfield.automatic_value, 0);
    strcat(ttext, tfield.automatic_value);
    replaceunderscoresandbrackets(tfield.automatic_value, 1);
    strcat(ttext, "\n");
}

// initialize in array and in file
void Initialize_Record()
{
  int i;
  char tdatetime[MAXSTRING];
  
  for (i=0;i<fieldsperrecord;i++) {
   Annotated_Field tfield(i, 0, " ", " ");
  Bring_DateTime_Stamp(tdatetime, tfield.id);
  if (record[tfield.id].type==CALENDAR)
   strcpy(tfield.text, tdatetime);
  records.push_back(tfield); }
  ++recordsnumber;
  
  Read_Write_db_File(1);
  Read_Write_db_File(); 
}

// add a field
int Add_Field(int type, char *name, char *textvalue)
{
  int i, tx=1, ty=1;
  vector <Annotated_Field>::iterator p;
  char ttext[MAXSTRING]; // for Generate_Field_String
  
   if (!recordsnumber) // initialization precaution
    return -1;
   if (fieldsperrecord>MAXFIELDS-1)
    return -1;
   
    if (name==NULL) {
     name=(char *) malloc(MAXSTRING);
    strcpy(name, EMPTYSTRING); }
    if (textvalue==NULL) {
     textvalue=(char *) malloc(MAXSTRING);
    strcpy(textvalue, " "); }
    if (type==VARIABLE) {
     tx=99; ty=99; 
    }
    Field tfield(fieldsperrecord, name, 0, const_cast <char *> ("000000000"), 58, tx, ty, 1, 1, const_cast <char *> ("000000000"), 58, 0, 58, type, const_cast <char *> (EMPTYSTRING), 2, 0, 0, 1, (type==VARIABLE || type==PROGRAM) ? 0 : 1, (type==VARIABLE) ? textvalue : const_cast <char *> (EMPTYSTRING));
    record.push_back(tfield);
    Annotated_Field ttfield(fieldsperrecord, atof(textvalue), textvalue, " ");
    for (i=0;i<recordsnumber-1;i++) {
     p=records.begin();
     p+=(i*(fieldsperrecord+1))+fieldsperrecord;
     records.insert(p, 1, ttfield);
   Generate_Field_String(&records[(i*fieldsperrecord)+fieldsperrecord+1], ttext); }
   // append to end 
   records.push_back(ttfield);
   Generate_Field_String(&records[records.size()-1], ttext);
   ++fieldsperrecord;
   
   Read_Write_db_File(3);
   Read_Write_db_File(1);
   
 return fieldsperrecord;
}

// delete field
void Delete_Field(int field_id)
{
  int i;
  vector <Annotated_Field>::iterator p=records.begin();
  vector <Field>::iterator t=record.begin();
  
   t+=field_id;
   record.erase(t);
   while (p!=records.end()) {
    if (p->id==field_id)
     records.erase(p--); 
   ++p; }
   --fieldsperrecord;
   p=records.begin();
   while (p!=records.end())
    for (i=0;i<fieldsperrecord;i++)
     p++->id=i;
   
   Renumber_Field_References(field_id);
   Renumber_Field_Relationships(field_id);
   Read_Write_db_File(3);
   Read_Write_db_File(1);
}

// merge logistically and physically a field with it's right next
int Join_Fields(int field_id, int mode)
{
  int i, n=0;
  char ttext[MAXSTRING];
  vector <int> fieldidentities;
  
  if (mode==HORIZONTALLY)
   sortfieldsbyxpt(field_id, fieldidentities);
  else
   sortfieldsbyypt(field_id, fieldidentities);
  for (i=0;i<fieldidentities.size();i++) 
   if (field_id==fieldidentities[i])
    break;
  if (i==fieldidentities.size()-1)
   return -1;
  Field trecord=record[fieldidentities[i]];
  // adjust to bigger y size if HORIZONTAL join
  if (mode==HORIZONTALLY) {
   n=(record[fieldidentities[i]].size.y>record[fieldidentities[i+1]].size.y) ? record[fieldidentities[i]].size.y : record[fieldidentities[i+1]].size.y;
   trecord.size.y=n;
   trecord.size.x+=record[fieldidentities[i+1]].size.x; 
   if ( trecord.size.x % 2 && record[fieldidentities[i+1]].size.x % 2 )
    trecord.size.x++;
  }
  else
   trecord.size.y+=record[fieldidentities[i+1]].size.y;
  if ( trecord.size.x * trecord.size.y > MAXSTRING ) {
   Show_Menu_Bar(1);
   Show_Message(1, 24, 1, "joined size would exceed maximum!", 1500);
   return -1;
  }
  record[fieldidentities[i]]=trecord;
  strcat(records[(currentrecord*fieldsperrecord)+field_id].text, " ");
  strcat(records[(currentrecord*fieldsperrecord)+field_id].text, records[(currentrecord*fieldsperrecord)+fieldidentities[i+1]].text);
  Delete_Field(fieldidentities[i+1]);
  
  Read_Write_db_File(3);
  Read_Write_db_File(1);
  
 return fieldidentities[i];
}

// renumber field references and fieldlists
void Renumber_Field_References(int startingfield)
{
  int i, i1, n, fieldid, newreference;
  char transformedtext[MAXSTRING], s[MAXSTRING];

  for (fieldid=0;fieldid<fieldsperrecord;fieldid++) {
   n=0;
   for (i=0;i<strlen(record[fieldid].automatic_value);i++) {
    if (record[fieldid].automatic_value[i]!='#')
     transformedtext[n++]=record[fieldid].automatic_value[i];
   else {
    i1=0;
    while (isdigit(record[fieldid].automatic_value[++i]) && i<strlen(record[fieldid].automatic_value))
     s[i1++]=record[fieldid].automatic_value[i];
    s[i1]='\0';
    newreference=atoi(s);
    strcpy(s, "#");
    --i;
    if (newreference<fieldsperrecord && newreference!=startingfield+1) {
     newreference-=(newreference<=startingfield) ? 0 : 1;
    strcat(s, itoa(newreference)); }
     else {
      if (iscalculationsign(record[fieldid].automatic_value[i+1]))
       ++i;
    strcpy(s, " "); }
    for (i1=0;i1<strlen(s);i1++)
   transformedtext[n++]=s[i1]; } }
   transformedtext[n]='\0';
  strcpy(record[fieldid].automatic_value, transformedtext); }
}

// renumber field ids in relationships
void Renumber_Field_Relationships(int startingfield) // autoremove if same field
{
  int i;
  vector<Relationship>::iterator p;

    for (i=0;i<relationships.size();i++) {
     if (relationships[i].localFields[0]-1==startingfield || relationships[i].localFields[1]-1==startingfield) {
      p=relationships.begin();
      p+=i;
     relationships.erase(p); }
     if (relationships[i].localFields[0]>startingfield)
      --relationships[i].localFields[0];
     if (relationships[i].localFields[1]>startingfield)
   --relationships[i].localFields[1]; }
}

// divide a field into two parts
int Divide_Field(int field_id, int mode)
{
  int i, add;
  add=( record[field_id].size.x % 2 ) ? 1 : 0;
  
  if (mode==HORIZONTALLY && record[field_id].size.x<2)
   return -1;
  if (mode==VERTICALLY && record[field_id].size.y<2)
   return -1;
  Add_Field();
  record[records[records.size()-1].id]=record[field_id];
  if (mode==HORIZONTALLY) {
   record[field_id].size.x/=2;
   record[records[records.size()-1].id].size.x/=2;
   record[records[records.size()-1].id].size.x+=add;
   record[records[records.size()-1].id].pt.x+=record[field_id].size.x; }
  else {
   record[field_id].size.y/=2;
   record[records[records.size()-1].id].size.y/=2;
  record[records[records.size()-1].id].pt.y+=record[field_id].size.y; }
  
  Read_Write_db_File(3);
  Read_Write_db_File(1);
    
 return records[records.size()-1].id;
}

// bring current datetime stamp in char array
void Bring_DateTime_Stamp(char tdatetime[MAXSTRING], int field_id) 
{
  int i;
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  char calendarformat[MAXSTRING];
  
  if (record[field_id].type==CLOCK)
   strcpy(calendarformat, "%X");
  else
   strcpy(calendarformat, "%x %X");
      
  if (isautomaticvalueformatinstruction(field_id))
   strcpy(calendarformat, record[field_id].automatic_value);
  
   strftime(tdatetime, MAXSTRING,const_cast <char *> (calendarformat), timeinfo);
  
  for (i=0;i<strlen(tdatetime);i++)
   if (tdatetime[i]=='\n')
    tdatetime[i]=SPACE;    
}
    
// read-write record from-to dbfile
int Read_Write_Field(Annotated_Field &tfield, long int field_position, int mode) // 0 read, 1 write
{
  int i;
  char ttext[FIELDSIZE];

  fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);
  if (!mode)
   dbfileaccess.seekg(field_position, ios::beg);
  else {
   dbfileaccess.seekp(field_position, ios::beg);
   replaceunderscoresandbrackets(tfield.text, 0);
   replaceunderscoresandbrackets(tfield.formula, 0);
   strcpy(ttext, itoa(tfield.id));
   strcat(ttext, " ");
   strcat(ttext, dtoa(tfield.number));
   strcat(ttext, " ");
   strcat(ttext, tfield.text);
   strcat(ttext, " ");
   strcat(ttext, tfield.formula);
   replaceunderscoresandbrackets(tfield.text, 1);
   replaceunderscoresandbrackets(tfield.formula, 1);
   for (i=strlen(ttext);i<FIELDSIZE;i++)
    ttext[i]=SPACE;
  stringcodedecode(ttext, ttext); }
   
  for (i=0;i<FIELDSIZE;i++) {
   if (!dbfileaccess)
    return -1;
   if (!mode) {
    dbfileaccess.get(ttext[i]);
   ttext[i]=charcoder(ttext[i], 1); }
   else
  dbfileaccess.put(ttext[i]); } 
  
  if (!mode) {
   istringstream ttfield(ttext);
   ttfield >> tfield.id;
   ttfield >> tfield.number;
   ttfield >> tfield.text;
   ttfield >> tfield.formula; 
   replaceunderscoresandbrackets(tfield.text, 1);
  replaceunderscoresandbrackets(tfield.formula, 1);  }
  
 return 0;
}
  
// return position value for field
long int fieldposition(int record_id, int field_id)
{
  int i;
  long int position=startofrecords+RELATIONSHIPSLENGTH+11; // position after parameters and relationships
  
  for (i=0;i<record_id*fieldsperrecord;i++) 
   position+=FIELDSIZE;
  for (i=0;i<field_id;i++)
   position+=FIELDSIZE;
   
 return position;
}

// serial in array to field position
long int fieldserialtofieldposition(int position)
{ 
  int i, n=0, record_id=0, field_id=0;

   for (i=0;i<position;i++) {
    ++n;
    if (n==fieldsperrecord) {
     ++record_id;
   n=0; } }
   field_id=n;
    
 return fieldposition(record_id, field_id);
}

// serial in array record id & field id
void fieldserialtoparameters(int position, int &record_id, int &field_id)
{
  int i, n=0;
  
  record_id=0;
  field_id=0;

   for (i=0;i<position;i++) {
    ++n;
    if (n==fieldsperrecord) {
     ++record_id;
    n=0; } }
    field_id=n;
}

// parameters to serial id of field
int fieldparameterstoserial(int record_id, int field_id)
{
  return (record_id*fieldsperrecord)+field_id;
} 
  
// delete a record from the series
void Delete_Record(int record_id)
{
  int i;
  vector<Annotated_Field>::iterator p=records.begin();
  p+=record_id*fieldsperrecord;
  --recordsnumber;
  if (currentrecord)
   --currentrecord;
  
   records.erase(p, p+fieldsperrecord);
   Read_Write_db_File(3);
   Read_Write_db_File(1);
}

// duplicate entire record
void Duplicate_Record(int record_id)
{
  int i;
  
   Initialize_Record();
   for (i=0;i<fieldsperrecord;i++) {
    records[((recordsnumber-1)*fieldsperrecord)+i].number=records[(record_id*fieldsperrecord)+i].number;
    strcpy(records[((recordsnumber-1)*fieldsperrecord)+i].text, records[(record_id*fieldsperrecord)+i].text);
   strcpy(records[((recordsnumber-1)*fieldsperrecord)+i].formula, records[(record_id*fieldsperrecord)+i].formula); }
   
    Read_Write_db_File(1);
}

// show record
int Show_Record_and_Menu()
{
  int i, i1, n, trecordsnumber, run=1, c, backupmenu=0, replaychar=0;
  int previousfield=-1, scriptdirection=NOSCRIPT;
  int findresults[MAXSEARCHDEPTH+1][MAXRECORDS], tsortresults[MAXRECORDS];
  char input_text[MAXSTRING], ttext[MAXSTRING], tattributes[9];
  FindSchedule tfindschedule;
  vector<Annotated_Field> trecords;
  vector<int>::iterator p;
  
   while (run) {
    // clear screen array
    for (i=1;i<81;i++)
     for (i1=1;i1<25;i1++)
      screen[i][i1]=SPACE;
    // handle field repetitions
    if (changedrecord) {
     lastfieldrepeated=-1;
     previousfield=-1;
     for (i=0;i<fieldsperrecord;i++)
      fieldrepetitions[i]=((record[i].color) ? record[i].color : -1);
    }
    changedrecord=0;
    // show fields
    if (renewscreen)
     clear();
    vector<int> adjoiningfields;
    fieldsadjoiningfields(&records[(currentrecord*fieldsperrecord)+currentfield], adjoiningfields);
    for (i=0;i<fieldsperrecord;i++)
     for (i1=0;i1<adjoiningfields.size();i1++)
      if (adjoiningfields[i1]!=i)
       Show_Field(&records[(currentrecord*fieldsperrecord)+i], printscreenmode);
    if (printscreenmode) {
     clear();
     for (i1=1;i1<25;i1++) {
      for (i=1;i<81;i++) {
       gotoxy(i, i1);
       printw("%c", screen[i][i1]);
       }
      }
    }
    for (i=0;i<adjoiningfields.size();i++)
     Show_Field(&records[(currentrecord*fieldsperrecord)+adjoiningfields[i]], 1);
    refresh();

    if (!recordsdemo) {
     for (i=0;i<=recordsnumber;i++)
      findresults[0][i]=i;
     for (;i<MAXRECORDS;i++)
      findresults[0][i]=-1;
    }
    
    // if changed field had script command in automatic value
    if ( previousfield != currentfield ) {
        
     if ( scriptdirection == ONEXIT || scriptdirection == ONENTRYANDEXIT )
      Execute_Script_Command(previousfield);
     
     scriptdirection=Determine_Script_Direction(currentfield);
     if ( scriptdirection > ONENTRYANDEXIT )
      scriptdirection=NOSCRIPT;
     if ( scriptdirection == ONENTRY || scriptdirection == ONENTRYANDEXIT )
      Execute_Script_Command(currentfield);

    }     
    previousfield=currentfield; 
       
    Show_Menu_Bar();
    cleanstdin();
    i=strlen(menutexts[currentmenu])+1;
    if (!menubar)
     i=1;
    if (menubar==2) {
     i=strlen(infotext)+1;
     if (recordsdemo)
    i+=14; }
    if (menubar==3)
     i=strlen(records[(currentrecord*fieldsperrecord)+currentfield].text)+1;
    gotoxy(i, menulines[currentmenu]);
    
    if (runscript) {
     runscript=commandparser(scriptcommand);
     Sleep(scriptsleeptime); 
    }
    
    if (kbhit() && runscript) {
     strcpy(scriptcommand, "stop");
    runscript=commandparser(scriptcommand);
    }
    
    if (!runscript || (keyonnextloop.size() && keyonnextloop[keyonnextloop.size()-1]!=SPACE)) { // also process keyonnextloop from scripts, but not space
     if (recordsdemoall && printscreenmode)
      blockunblockgetch(PRINTUNBLOCK);
     else
     blockunblockgetch(pagehasclockfields() ? UNBLOCK : UNBLOCK/3); // necessary to give live clock operational status
     if (mousemenucommands.size())
      c=fetchmousemenucommand();
     else
      c=(replaychar) ? replaychar : sgetch();
     replaychar=0;
     blockunblockgetch();
     renewscreen=c=negatekeysforcurrentmenu(c);
     if (!alteredparameters && currentmenu!=5)
      for (i=0;i<strlen(alterscreenparameterskeys);i++)
       if (c==alterscreenparameterskeys[i])
        alteredparameters=1;
     if ( altpressed ) {
      for (i=0;i<strlen(programkeys);i++) {
       if (c==programkeys[i]) {
        addorplayprogram(c);
        c=0;
       }
      }
      altpressed=0;
     }
       
     if (recordsdemoall) {
      if (c==ESC)
       c='w';
      if (c!='w') {
       c='>';
       renewscreen=1;
       if (!printscreenmode) // a little extra time to view record
      Sleep(750); }
     }
     
     // execute automatic record scripts
     if (!runscript) {
      for (i=0;i<fieldsperrecord;i++) {
       if (record[i].buttonbox==AUTOMATICSCRIPT && i!=lastfieldrepeated && fieldrepetitions[i]) {
        pushspaceonfield(i);
        break;
       } 
      }
      if (i==fieldsperrecord)  //one or none fields to be repeated
       lastfieldrepeated=-1;
     }

      
     switch (c) {
      // print screen
      case 'z':
       if (printscreenmode) {
        printscreenmode=0;
        mousemask(ALL_MOUSE_EVENTS, NULL);
        fclose(out);
       break; }
       else
        printscreenmode=2; // to use in flag for Show_Field
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "filename:", 0);
       strcpy(input_string, pages[currentpage]);
       Reccurse_File_Extension(input_string, 3);
       i=Scan_Input(input_string, 11, 24, 5);
       if (i==ESC)
        break;
       out=fopen(input_string, "a");
       mousemask(0, NULL);
      break;
      case 'q':
       if (!printscreenmode)
        break;
       outputscreenarraytofile();
      break;
      case 'w': // demonstrate all records
       if (recordsdemoall) {
        recordsdemoall=0;
       break; }
       if (recordsnumber>1)
        recordsdemoall=1;
       currentrecord=findresults[0][0];
      break;
      // from menu 0
      case 'e':
       currentmenu=2;
       Read_Write_Current_Parameters(1, 1);
      break;
      case 'o':
       currentmenu=1;
       Read_Write_Current_Parameters(1, 1);
      break;
      case 't':
       currentmenu=3;
       Read_Write_Current_Parameters(1, 1);
      break;
      case '`': {
       if (currentmenu==5)
        currentmenu=backupmenu;
       else {
        int buttons=0, screens=0;
        for (i1=0;i1<fieldsperrecord;i1++) {
         if (record[i1].buttonbox==BUTTONBOX)
          ++buttons;
         if (record[i1].buttonbox==BUTTONSCREEN)
        ++screens; }
        if (!(buttons+screens))
         break;
        backupmenu=currentmenu;  // keep old menu from calculator mode
       currentmenu=5; }
       Read_Write_Current_Parameters(1, 1);  
       Initialize_Database_Parameters(1); }
      break;
      case 'm': // from all menus
       ++menubar;
       menubar=(menubar==4) ? 0 : menubar;
       if (autosave)
        Read_Write_Current_Parameters(2, 1);
      break;
      case ESC: // from all menus
       if (recordsdemo) {
        recordsdemo=0;
        currentrecord=trecordsnumber; // restore currentrecord before find
       break; }
       if (currentmenu==7) {
        currentmenu=6;
        if (autosave)
         Read_Write_Current_Parameters(1, 1);
       break; }
       if (currentmenu==6) {
        currentmenu=2;
        if (autosave)
         Read_Write_Current_Parameters(1, 1);
       break; }
       if (currentmenu==0) {
        currentmenu=4;
        menubar=1;
        Show_Menu_Bar();
        gotoxy(strlen(menutexts[currentmenu])+1, menulines[currentmenu]);
        c=sgetch();
        if (c==ESC || c=='y')
         run=0;
        else
         currentmenu=0; }
       else
        currentmenu=0;
       if (autosave)
        Read_Write_Current_Parameters(1, 1);
      break;
      case '>': // from all menus
       for (i=0;i<recordsnumber+1;i++) 
        if (findresults[0][i]>currentrecord) {
         currentrecord=findresults[0][i];
         changedrecord=1;
       break; }
       if (recordsdemoall && printscreenmode) // output to file in print mode
        outputscreenarraytofile();
       if (recordsdemoall && i==recordsnumber+1)
        recordsdemoall=0;
       if ((currentrecord>recordsnumber-1 && currentrecord+1<MAXRECORDS)) {
        if (recordsdemoall) {
         recordsdemoall=0;
         --currentrecord;
        break; }
        if (recordsdemo) {
         --currentrecord;
        break; }
        Show_Menu_Bar(1);
        Change_Color(RED);
        gotoxy(1,24);
        printw("initiate record (y/n):");
        c=sgetch();
        if (tolower(c)=='y') {
         Initialize_Record();
        --currentrecord; }
        else
       --currentrecord; }
       if (!recordsdemo && autosave) 
        Read_Write_Current_Parameters(0, 1);
       Show_Menu_Bar();
      break;
      case '<': // from all menus
       for (i=currentrecord;i>-1;i--) 
        if (findresults[0][i]<currentrecord && findresults[0][i]>-1) {
         currentrecord=findresults[0][i];
         changedrecord=1;
       break; }
       if (!recordsdemo && autosave) 
        Read_Write_Current_Parameters(0, 1);
      break;
      case HOME:
       currentfield=findfieldege();
      break;
      case END:
       currentfield=findfieldege(1);
      break;
      case START_OF_RECORDS: // from all menus
       currentrecord=findresults[0][0];
       Read_Write_Current_Parameters(0, 1);
      break;
      case END_OF_RECORDS: // from all menus
       currentrecord=findresults[0][recordsnumber-1];
       Read_Write_Current_Parameters(0, 1);
      break;
      case 'j':
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "join field with it's <r>ight or <b>elow field (r/b):", 0);
       c=sgetch();
       if (c!='r' && c!='b')
        break;
       switch (c) {
        case 'r':
         Join_Fields(currentfield, HORIZONTALLY);
        break;
        case 'b':
         Join_Fields(currentfield, VERTICALLY);
       break; }
      break;
      case 'v':
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "divide field <h>orizontically or <v>ertically (h/v):", 0);
       c=sgetch();
       if (c!='h' && c!='v')
        break;
       switch (c) {
        case 'h':
         Divide_Field(currentfield, HORIZONTALLY);
        break;
        case 'v':
         Divide_Field(currentfield, VERTICALLY);
       break; }
      break;
      case DOWN: // from all menus
       if (recordsdemo)
        break;
       {
         vector <int> fieldyidentities;
         n=0;
         sortfieldsbyypt(currentfield, fieldyidentities);
         for (i=0;i<fieldyidentities.size();i++) 
          if (currentfield==fieldyidentities[i])
           break;
         if (i<fieldyidentities.size()-1)
          currentfield=fieldyidentities[i+1];
         else {
          ++currentfield;
          if (currentfield>fieldsperrecord-1)
           currentfield=0;
          while (!record[currentfield].active || !record[currentfield].editable) {
           ++currentfield;
           if (currentfield>fieldsperrecord-1)
            currentfield=0;
          }
         }
       }
      break;
      case TAB: // keep a simple navigational button
       ++currentfield;
       if (currentfield>fieldsperrecord-1)
        currentfield=0;
       while (!record[currentfield].active || !record[currentfield].editable) {
        ++currentfield;
        if (currentfield>fieldsperrecord-1)
      currentfield=0; }
      break;
      case UP:// from all menus
       if (recordsdemo)
        break;
       { 
         vector <int> fieldyidentities;
         n=0;
         sortfieldsbyypt(currentfield, fieldyidentities);
         for (i=0;i<fieldyidentities.size();i++) 
          if (currentfield==fieldyidentities[i])
           break;
         if (i)
          currentfield=fieldyidentities[i-1];
         else {
          --currentfield;
          if (currentfield<0)
           currentfield=fieldsperrecord-1;
          while (!record[currentfield].active || !record[currentfield].editable) {
           --currentfield;
           if (currentfield<0)
            currentfield=fieldsperrecord-1;
          }
         }
       }
      break;
      case SHIFT_TAB: // one more simple navigational button
       --currentfield;
       if (currentfield<0)
        currentfield=fieldsperrecord-1;
       while (!record[currentfield].active || !record[currentfield].editable) {
         --currentfield;
         if (currentfield<0)
       currentfield=fieldsperrecord-1; }
      break;
      case RIGHT:
       if (recordsdemo)
        break;
       { 
         vector<int> fieldxidentities;
         n=0;
         sortfieldsbyxpt(currentfield, fieldxidentities);
         for (i=0;i<fieldxidentities.size();i++) 
          if (currentfield==fieldxidentities[i])
           break;
         if ( i < fieldxidentities.size()-1 )
          currentfield=fieldxidentities[i+1];
         if ( i == fieldxidentities.size()-1 )
          currentfield=fieldxidentities[0];   
       }
      break;
      case LEFT:
       if (recordsdemo)
        break;
       {
         vector <int> fieldxidentities;
         n=0;
         sortfieldsbyxpt(currentfield, fieldxidentities);
         for (i=0;i<fieldxidentities.size();i++) 
          if (currentfield==fieldxidentities[i])
           break;
         if ( i )
          currentfield=fieldxidentities[i-1];
         if ( i == 0 )
          currentfield=fieldxidentities[fieldxidentities.size()-1];
       }
      break;
      case KEY_MOUSE:
       if (recordsdemo)
        break;
       if (leftmousebuttondoubleclicked()) {
        if (mouse.y+1 == 24  && menubar==1 ) {
         if ((i=Activate_Menubar_Choice(mouse.x)))
          keyonnextloop.push_back(i);
         break;
        }
        i=locatefieldbymouseclick();
        if (i>-1) {
         currentfield=i;
         if (currentmenu == 2)
          replaychar='d';
         else
          pushspaceonfield(); // in case it's a link
        }
        break;
       }
       if (middlemousebuttonpressed()) {
        Pages_Selector();
        break;
       }
       if ((i=wheelmousemove())) {
        keyonnextloop.push_back(i);
        break;
       }
       if (rightmousebuttonclicked()) {
        keyonnextloop.push_back(ESC);
        break;
       }
       else {
        i=locatefieldbymouseclick();
        currentfield=(i>-1) ? i : currentfield;
       }
      break;
      case 'g': // from all menus
       if (recordsdemo)
        break;
       Show_Menu_Bar(1);
       Change_Color(WHITEONBLACK);
       gotoxy(1, 24);
       printw("jump to record:");
       i=Scan_Input(1, 1, recordsnumber)-1;
       if (i>-1 && i<recordsnumber)
        currentrecord=i;
      break;
      case '?': // from all menus
       Show_Help_Screen();
      break;
      // from menu 1
      case 'a':
       toggleautosave();
      break;
      case 'l':
       if (currentmenu==1) {
        Show_Menu_Bar(1);
        strcpy(input_string, dbfile);
        i=Scan_Input(input_string, 1, 24, 5);
        if (i==ESC)
         break;
        Reccurse_File_Extension(input_string, 2);
        if (!tryfile(input_string)) {
         Show_Message(1, 24, 1, "nonexisting file!", 1500);
        break; }
        strcpy(dbfile, input_string);
        strcpy(pages[0], dbfile);
        currentpage=0;
        pagesnumber=1;
        Load_Database(currentpage);
        Show_Message(1, 24, 2, "database loaded", 1500);
       }
       if (currentmenu==6) {
        currentmenu=7;
        if (autosave)
         Read_Write_Current_Parameters(1, 1);
       }
      break;      
     break;
     case 's':
      Show_Menu_Bar(1);
      Show_Message(1, 24, 5, "filename:", 0);
      strcpy(input_string, dbfile);
      i=Scan_Input(input_string, 1, 24, 5);
      if (i==ESC)
       break;
      Reccurse_File_Extension(input_string, 2);
      strcpy(dbfile, input_string);
      Read_Write_db_File(3);
      Read_Write_db_File(1);
      alteredparameters=0;
      Show_Message(1, 24, 2, "database saved", 1500);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 5, "export .rc file (y/n):", 0);
      c=sgetch();
      if (c=='y') {
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "filename:", 0);
       Scan_Input();
       i=Write_rc_File(input_string);
       Reccurse_File_Extension(input_string, 1);
       Show_Menu_Bar(1);
       if (!i)
        Show_Message(1, 24, 4, ".rc file recreated", 1500);
      Show_Menu_Bar(); }
     break;
     case 'h':
      Show_Help_Screen();
     break;
     // from menu 2
     case 'd':
      if (currentmenu==2) {
      if (record[currentfield].type!=CALENDAR && record[currentfield].type!=CLOCK && record[currentfield].buttonbox==NOBUTTON) {
       if (record[currentfield].buttonbox==BUTTONSCREEN)
        record[currentfield].type=NUMERICAL; // trick to bring reversepolishcalculator
       if (!record[currentfield].fieldlist) 
        for (i=0;i<fieldsperrecord;i++)
         Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
       else {
        Show_Menu_Bar(1);
        if (menubar==1)
         Show_Message(1, 24, 5, "fieldlist entry. <up> and <down> arrows, <insert> to bring relevant data", 1750);
        if (fieldhasdependancy!=2) {
         attron(A_BLINK);
        Show_Field(&records[(currentrecord*fieldsperrecord)+record[currentfield].fieldlist-1], 1);
        attroff(A_BLINK); } }
      Screen_String_Editor(records[(currentrecord*fieldsperrecord)+currentfield]); }
      if (isautomaticvalueformatinstruction(currentfield) && (record[currentfield].type==STRING || record[currentfield].type==MIXEDTYPE))
       strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, formatreplacer(record[currentfield].automatic_value, currentfield));
      if (record[currentfield].type==CALENDAR) {
       strcpy(input_string, records[(currentrecord*fieldsperrecord)+currentfield].text);
       Scan_Date(1, 24, input_string);
       strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, input_string);
      }
      if (autosave)
       Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1);
      // jump to next BUTTONCOMMAND script, if any
      if (record[currentfield].fieldlist && record[record[currentfield].fieldlist-1].buttonbox==BUTTONCOMMAND) {
       currentfield=record[currentfield].fieldlist-1;
      pushspaceonfield(); }
      if (record[currentfield].buttonbox==BUTTONBOX)
       pushspaceonfield();
      }
      if (currentmenu==6) {
       if (recordsnumber<2)
        break;
       Show_Menu_Bar(1);
       Show_Message(1, 24, 1, "delete entire record (y/n):", 0);
       c=sgetch();
       if (tolower(c)=='y')
        Delete_Record(currentrecord);
     }
     break;
     case 'p':
      if (record[records[(currentrecord*fieldsperrecord)+currentfield].id].type==CALENDAR) {
       Bring_DateTime_Stamp(records[(currentrecord*fieldsperrecord)+currentfield].text, record[records[(currentrecord*fieldsperrecord)+currentfield].id].id);
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1); }
     break;
     case 'k':
      // calendar requires 8 consecutive fields with least x size 29, y size 1
      for (i=0;i<8;i++)
       if (record[currentfield+i].type!=STRING || record[currentfield+i].size.x<29 || record[currentfield+i].size.y!=1)
        break;
      for (i1=0;i1<6;i1++)
       if ((arefieldsneighbours(currentfield+i1, currentfield+i1+1)!=VERTICALLY))
        break;
      if (i<8 || i1<6) {
       Show_Menu_Bar(1);
       Show_Message(1, 24, 1, "suitable records not found", 750);
       break;
      }
      Scan_Date(1, 24, input_string, 1);
      int m, y, i2; char *calendar;
      m = atoi(strtok(input_string, "/"));
      y = atoi(strtok(NULL, "/"));
      calendar=Generate_Calendar(m-1, y);
      // now fill current&neighbouring fields
      for (i=i1=i2=0;i1<strlen(calendar);i1++) {
       records[(currentrecord*fieldsperrecord)+currentfield+i].text[i2++]=calendar[i1];
       if (calendar[i1]=='>') {
        records[(currentrecord*fieldsperrecord)+currentfield+i].text[i2++]='\0';
        i2=0; ++i; 
        records[(currentrecord*fieldsperrecord)+currentfield+i].text[i2++]='<';
       }
      }
      records[(currentrecord*fieldsperrecord)+currentfield+i].text[i2++]='\0';
      if (i==6) // last field may not be needed
       Delete_Field_Entry(currentfield+7);
      if (autosave)
       for (i=0;i<8;i++)
        Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield+i], fieldposition(currentrecord, currentfield+i), 1);
     break;
     case SPACE:
      pushspaceonfield();
     break;
     case '+':
      if (currentmenu!=5)
       record[currentfield].attributes[6]=(record[currentfield].attributes[6]=='1') ? '0' : '1';
     break;
     case '-':
      if (currentmenu!=5)
       record[currentfield].attributes[2]=(record[currentfield].attributes[2]=='1') ? '0' : '1';
     break;
     case '*':
      if (currentmenu!=5)
       record[currentfield].attributes[1]=(record[currentfield].attributes[1]=='1') ? '0' : '1';
     break;
     case '/':
      if (currentmenu!=5)
       record[currentfield].attributes[4]=(record[currentfield].attributes[4]=='1') ? '0' : '1';
     break;
     case '.':
     if (currentmenu!=5)
      record[currentfield].attributes[5]=(record[currentfield].attributes[5]=='1') ? '0' : '1';
     break;
     case '!':
      if (currentmenu!=5)
       record[currentfield].color+=record[currentfield].color<58 ? 1 : -58;
     break;
     case '@':
      if (currentmenu!=5)
       record[currentfield].color-=record[currentfield].color>2 ? 1 : -57;
     break;
     case 'c':
      for (i=0;i<fieldsperrecord;i++)
       Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 3, "source field:", 0);
      Change_Color(BLUE);
      i=Scan_Input(1, 1, MAXFIELDS);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 3, "destination field:", 0);
      Change_Color(BLUE);
      n=Scan_Input(1, 1, MAXFIELDS);
      if (!n)
       break;
      strcpy(records[(currentrecord*fieldsperrecord)+n-1].text, records[(currentrecord*fieldsperrecord)+i-1].text); 
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+n-1], fieldposition(currentrecord, n-1), 1);
     break;
     case DELETE: {
      vector<int> fieldstodelete;
      fieldsadjoiningfields(&records[(currentrecord*fieldsperrecord)+currentfield], fieldstodelete);
      for (i=0;i<fieldstodelete.size();i++) {
       Delete_Field_Entry(fieldstodelete[i]);
       if (autosave)
        Read_Write_Field(records[(currentrecord*fieldsperrecord)+fieldstodelete[i]], fieldposition(currentrecord, fieldstodelete[i]), 1);
      }
     break; }
     case INSERT:
      if (currentmenu==2) {
       currentmenu=6;
       Read_Write_Current_Parameters(1, 1);
      }
     break;
     case 'u':
      if (currentmenu==3) {
       Field_Editor();
       // determine button boxes
       for (i=0;i<record.size();i++)
        Determine_Button_Box(i);
       changedrecord=1;
       currentrecord=Read_Write_Current_Parameters(0);
      }
      if (currentmenu==6)
       Duplicate_Record(currentrecord);
     break;
     case 'x':
      Show_Menu_Bar(1);
      Show_Message(1, 24, 5, "filename:", 0);
      Scan_Input();
      Export_Database(input_string);
     break;
     case COPY:
      copytoclipboard();
     break;
     case PASTE:
      pastefromclipboard();
     break;
     // menu 3
     case 'i':
      if (currentmenu==3)
       Show_DB_Information();
      if (currentmenu==6) {
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "filename:", 0);
       Scan_Input();
       Import_Database(input_string);
      }
      if (currentmenu==7) {
       Show_Menu_Bar(1);
       Show_Message(1, 24, 4, "external dbfile:", 0);
       Scan_Input();
       i=Import_External_db_File(input_string);
      }
     break;
     case 'r':
      if (currentmenu==7) {
       References_Editor();
      break; }
      for (i=0;i<fieldsperrecord;i++)
       Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
      findschedule.clear();
      trecords.clear();
      findresults[0][recordsnumber]=-1;
      for (i=0;i<recordsnumber;i++)
       tsortresults[i]=findresults[0][i];
      while (findschedule.size()<MAXSEARCHDEPTH) {
       Show_Menu_Bar(1);
       Change_Color(BLUE);
       gotoxy(1, 24);
       tfindschedule.field_id=Scan_Input(1, 1, MAXFIELDS);
       if (!tfindschedule.field_id)
        break;
       --tfindschedule.field_id;
       Show_Menu_Bar(1);
       Change_Color(BLUE);
       gotoxy(1, 24);
       Scan_Input();
       strcpy(tfindschedule.texttolookfor, input_string);
      findschedule.push_back(tfindschedule); }
      for (i=0;i<findschedule.size();i++) {
       i1=0;
       i1=(tolower(findschedule[i].texttolookfor[0])=='a') ? i1 : 1;
      sortrecords(findschedule[i].field_id, tsortresults, i1); }
      // rearrange records order
      for (i1=0,i=0;i<recordsnumber;i1++,i++)
       for (n=0;n<fieldsperrecord;n++)
        trecords.push_back(records[(tsortresults[i1]*fieldsperrecord)+n]);
      records.clear();
      records=trecords;
      Read_Write_db_File(1);
      Read_Write_db_File();
      Show_Menu_Bar(1);
      Show_Message(1, 24, 4, "database sorted", 1500); 
     break;
     case 'f':
      for (i=0;i<fieldsperrecord;i++)
       Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
      findschedule.clear();
      findresults[0][recordsnumber]=-1;
      for (n=1;n<MAXSEARCHDEPTH+1;n++) // first row already arranged in loop
       for (i=0;i<MAXRECORDS;i++)
        findresults[n][i]=-1;
      while (findschedule.size()<MAXSEARCHDEPTH) {
       Show_Menu_Bar(1);
       Change_Color(BLUE);
       gotoxy(1, 24);
       tfindschedule.field_id=Scan_Input(1, 1, MAXFIELDS);
       if (!tfindschedule.field_id) // return or non-numerical input_string
        break;
       --tfindschedule.field_id;
       Show_Menu_Bar(1);
       Change_Color(BLUE);
       gotoxy(1, 24);
       Scan_Input();
       strcpy(tfindschedule.texttolookfor, input_string);
      findschedule.push_back(tfindschedule); }
      // now run find for all findschedule size
      for (i1=0, trecordsnumber=0, i=0;i<findschedule.size();i1=0, trecordsnumber=0, i++)
       while ((n=findresults[i][trecordsnumber++])>-1)
        if (find(records[(n*fieldsperrecord)+findschedule[i].field_id].text, findschedule[i].texttolookfor))
         findresults[i+1][i1++]=n;
       if (findresults[1][0]==-1) {
        Show_Menu_Bar(1);
        Show_Message(1, 24, 4, "no records with given criteria found", 1500);
       break; }
       for (trecordsnumber=MAXRECORDS;trecordsnumber>-1;trecordsnumber--)
        if (findresults[i][trecordsnumber]>-1)
         break;
        ++trecordsnumber;
       recordsdemo=1;
       // copy last find results into findresults[0], when in demo check if currentrecord is there to show
       for (n=0;n<MAXRECORDS;n++)
        findresults[0][n]=findresults[i][n];
       strcpy(input_string, "found "); // just use input_string
       strcat(input_string, itoa(trecordsnumber));
       strcat(input_string, " record");
       if (trecordsnumber>1)
        strcat(input_string, "s");
       strcat(input_string, " with given criteria");
       Show_Message(1, 24, 4, input_string, 1500);
       trecordsnumber=currentrecord; // keep value of currentrecord
       currentrecord=findresults[0][0];
    break; } 
       
    // calculator keys
    if (currentmenu==5 && c) {
     ttext[0]=c;
     ttext[1]='\0';
     switch (c) {
      case ENTER:
       strcpy(ttext, "EXEC");
      break;
      case DELETE:
       strcpy(ttext, "AC");
      break;
      case BACKSPACE:
       strcpy(ttext, "DEL");
     break; }
     for (i=0;i<fieldsperrecord;i++) 
      if (record[i].buttonbox==BUTTONBOX && !strcmp(ttext, record[i].automatic_value))
       pushspaceonfield(i);
     }
    }
   }
  
 return 0;
}

// string editor in screen
int Screen_String_Editor(Annotated_Field &tfield)
{
  char t, tstring[MAXSTRING];
  long int tfieldposition;
  tfieldposition=fieldposition(currentrecord, tfield.id);
  fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);
   
   strcpy(tstring, tfield.text);
    // ttext to tfield
    Show_Menu_Bar(1);
    t=Scan_Input(tstring, 1, 24, record[tfield.id].color);
    if (t==ESC)
     return -1;    
    strcpy(tfield.text, tstring);
    tfield.number=atof(tfield.text);

 return 0;
}

// negate input char if outside of appropriate menu
int negatekeysforcurrentmenu(int t)
{
  int i;
    
  if (t==SHIFT_RIGHT) t='>'; // shift+right arrow
  if (t==SHIFT_LEFT) t='<'; // shift+left arrow
  if (t==ESC || t==LEFT || t==RIGHT || t==UP || t==DOWN || t==TAB || t==SHIFT_TAB || t=='m' || t=='g' || t=='?' || t==HOME || t==END || t=='<' || t=='>' || t==']' ||  t==START_OF_RECORDS || t==END_OF_RECORDS || t==KEY_MOUSE || t=='z' || t=='q' || t=='w')
   return t;

  if (recordsdemo)
   return 0;
  
  if (t==6) { currentmenu=3; t='f'; return t; } // enter find mode
  if (t==5 || t==15 || t==20 || t==PAGES_SELECTOR_KEY) { // direct menu access with ctrl
   switch (t) {
    case 5:currentmenu=2; break; case 15:currentmenu=1; break; case 20:currentmenu=3; break; case PAGES_SELECTOR_KEY: if (!printscreenmode) Pages_Selector();
   break; }
   Read_Write_Current_Parameters(1, 1);
  return 1; } // 1 will set renewscreen

  if (currentmenu==2 || currentmenu==6)
   if (t==INSERT || t==DELETE || t==SPACE || t==COPY || t==PASTE)
    return t;
   
  if (currentmenu==2 && t=='\n')
   return 'd';
  
  if (currentmenu==5)
   if (t==ENTER || t==BACKSPACE || t==DELETE || t==SPACE)
    return t;
   
  if (currentmenu<4)
   for (i=0;i<strlen(programkeys);i++)
    if (t==(int) programkeys[i])
     return t;
    
  for (i=0;i<strlen(menukeys[currentmenu]);i++)
   if (t==menukeys[currentmenu][i])
    return t;
      
 return 0;
}

// menu bar
void Show_Menu_Bar(int mode) // 0 show, 1 remove
{  
  int i, recordsize=0;

  Change_Color(BLACK);
  gotoxy(1, menulines[currentmenu]);
  for (i=0;i<79;i++)
   addch(SPACE);

  if (!mode) {
   if (menubar==1 && !recordsdemo) {
    Change_Color(menucolors[currentmenu]);
    gotoxy(1, menulines[currentmenu]);
   printw("%s", menutexts[currentmenu]); }
   if (menubar==3) {
    Change_Color(record[currentfield].color);
    gotoxy(1, menulines[currentmenu]);
   printw("%s", records[(currentrecord*fieldsperrecord)+currentfield].text);  }
   gotoxy(1,menulines[currentmenu]);
   if (recordsdemo || menubar==2) {
    recordsize=0;
    for (i=0;i<fieldsperrecord;i++)
     recordsize+=fieldlength(records[(currentrecord*fieldsperrecord)+i].text);
    if (recordsdemo) {
     Change_Color(BLUE);
    printw("find results->"); }
    Change_Color(2);
    strcpy(infotext, "record ");
    strcat(infotext, itoa(currentrecord+1));
    strcat(infotext, " of ");
    strcat(infotext, itoa(recordsnumber));
    strcat(infotext, ".field ");
    strcat(infotext, itoa(currentfield+1));
    strcat(infotext, " of ");
    strcat(infotext, itoa(fieldsperrecord));
    strcat(infotext, ".record size:");
    strcat(infotext, itoa(recordsize));
    strcat(infotext, ".field size:");
    strcat(infotext, itoa(fieldlength(records[(currentrecord*fieldsperrecord)+currentfield].text)));
   printw("%s", infotext); } }

  refresh();
}

// show a help scren
void Show_Help_Screen()
{
  ifstream helpfile("reccurse.man");
  if (!helpfile)
   return;
  string helpinfo;
  int lines=0, c;
  clear();
  Change_Color(WHITEONBLACK);
    
   while (helpfile) {
    getline(helpfile, helpinfo);
    ++lines;
    if (lines>23) {
     c=getch();
     if (c==ESC)
      return;
     clear();
    lines=0; }
    printw("%s\n", helpinfo.c_str());
   }
   getch();
    
  helpfile.close();

}

// show database information screen
void Show_DB_Information()
{
  char dbinfo[1000];
  int i, n, averagerecordsize, averagefieldsize, tfieldsize=0, numericalfields=0, mixedfields=0, stringfields=0, datestampfields=0, variablefields=0, extrawindowlength;
  long int totalrecordssize=0;

   for (i=0;i<record.size();i++)
    switch (record[i].type) {
     case NUMERICAL:
      ++numericalfields;
     break;
     case CALENDAR:
      ++datestampfields;
     break;
     case STRING:
      ++stringfields;
     break;
     case MIXEDTYPE:
      ++mixedfields;
     break;
     case VARIABLE:
      ++variablefields;
    break; }
    
   for (i=0;i<recordsnumber;tfieldsize=0, i++) {
    for (n=0;n<fieldsperrecord;n++) {
     tfieldsize+=fieldlength(records[(i*fieldsperrecord)+n].text);
   totalrecordssize+=tfieldsize; } }
   averagefieldsize=totalrecordssize/(recordsnumber*fieldsperrecord);
   averagerecordsize=totalrecordssize/recordsnumber;
  
   extrawindowlength=(numberofdigits(totalrecordssize)<5) ? 0 : numberofdigits(totalrecordssize)-5;  
   Draw_Box(BOXCHAR, 6, 17, 32+extrawindowlength, 5, 16, 36);
   Change_Color(36);
   gotoxy(21,6);
   printw("Database Information");
   gotoxy(21,7);
   printw("--------------------");
   gotoxy(21,8);
   printw("name:%s", dbfile);
   gotoxy(21,9);
   printw("file size:%ld bytes", filesize(dbfile));
   gotoxy(21,10);
   printw("number of records:%d", recordsnumber);
   gotoxy(21,11);
   printw("fields per record:%d", fieldsperrecord);
   gotoxy(21,12);
   printw("total records size:%ld", totalrecordssize);
   gotoxy(21,13);
   printw("average records size:%d", averagerecordsize);
   gotoxy(21,14);
   printw("average field size:%d", averagefieldsize);
   gotoxy(21,15);
   printw("numerical fields:%d", numericalfields);
   gotoxy(21,16);
   printw("string fields:%d", stringfields);
   gotoxy(21,17);
   printw("mixed fields:%d", mixedfields);
   gotoxy(21,18);
   printw("calendar fields:%d", datestampfields);
   gotoxy(21,19);
   printw("variable fields:%d", variablefields);   
   gotoxy(21,20);
   printw("autosave is:%s", onoff[autosave]);
   refresh();
   getch();   
}
  
// show field
int Show_Field(Annotated_Field *field, int flag) // 1 highlight, 2 only in screen array
{
  Field *tfield=&record[field->id];
  int i, i1, lima, limb, tposx, tposy, tcolor, columninprint, rowinprint;
  char ttext[MAXSTRING*24], ttcolor[4];
  int attributestable[9]; // normal, standout, underline, reverse, blink, dim, bold, protect, invisible
  int linkparameters[2];

  if (!tfield->active || (tfield->type>MIXEDTYPE && tfield->type!=CLOCK))
   return -1;
  lima=(tfield->box) ? 80 : 81;
  limb=(tfield->box) ? 23 : 24;
  if ((tfield->pt.x+tfield->size.x)>lima || (tfield->pt.y+tfield->size.y)>limb)
   return -1;
  if (tfield->size.x>MAXSTRING || tfield->size.y>20)
   return -1;
  
  // box
  if (tfield->box)
   Draw_Box(/*BOXCHAR,*/ tfield->box_color, tfield->pt.x-1, tfield->size.x+1, tfield->pt.y-1, tfield->size.y+1);
  // contents
  
  //title
  if (strcmp(tfield->title, EMPTYSTRING)) {
   switch (tfield->title_position) {
    case 0:
     tposx=tfield->pt.x;
     tposy=tfield->pt.y-1;
    break;
    case 1:
     tposx=tfield->pt.x+tfield->size.x+1;
     tposy=tfield->pt.y;
    break;
    case 2:
     tposx=tfield->pt.x;
     tposy=tfield->pt.y+tfield->size.y;
    break;
    case 3:
     tposx=tfield->pt.x-strlen(tfield->title)-1;
     tposy=tfield->pt.y;
    break;
    case 4:
     tposx=tfield->pt.x;
     tposy=tfield->pt.y;
   break; }
   if (tposx>0 && tposx<80 && tposy>0 && tposy<24 && strcmp(tfield->title, EMPTYSTRING)) {
   for (i=0;i<9;i++)
    attributestable[i]=ctoi(record[tfield->id].title_attributes[i]);
    if (!printscreenmode)
     for (i=0;i<9;i++) {
      if (attributestable[i])
       Change_Attributes(i);
    }
    if (!printscreenmode)
     Change_Color(tfield->title_color);
    gotoxy(tposx, tposy);
    if (flag<2)
     printw("%s", tfield->title);
    for (i=0;i<strlen(tfield->title);i++)
     screen[tposx+i][tposy]=tfield->title[i];
    // remove attributes
    Change_Attributes(NORMAL);
   } }

   // field string to field size and lines
   fieldhasdependancy=Generate_Dependant_Field_String(field, ttext);
   if (fieldhasdependancy!=1)
    Generate_Field_String(field, ttext);
   if (field->number || record[field->id].type==NUMERICAL || record[field->id].type==MIXEDTYPE || record[field->id].buttonbox==BUTTONSCREEN)
    addleadingzeros(ttext, field);
     
   // add attributes
   for (i=0;i<9;i++)
    attributestable[i]=ctoi(record[tfield->id].attributes[i]);
   for (i=0;i<9;i++) {
    if (attributestable[i])
     Change_Attributes(i); }
   tcolor=tfield->color;
   
   // see if there is an alignment request
   i=0;
   while (isspace(ttext[i]))
    ++i;
   if (ttext[i]=='\\' && ttext[i+1]=='p' && isdigit(ttext[i+2]) && flag<2) {
    Show_Field(field, 2);
    aligntext(ttext, field, ctoi(ttext[i+2])); 
   }
   // highlight field
   if (flag == 1) {
    tcolor=(tfield->color==highlightcolors[0]) ? highlightcolors[1] : highlightcolors[0];
    for (i=strlen(ttext);i<((tfield->size.x)+1)*((tfield->size.y)+1);i++)
     ttext[i]=SPACE;
   ttext[i]='\0';
   }
   Change_Color(tcolor); 
   columninprint=tfield->pt.x, rowinprint=tfield->pt.y;
   for (i=0;i<strlen(ttext);i++) {
    if (strlen(ttext)==1 && isspace(ttext[0]))
     break;
    if (ttext[i]=='\\') {
     switch (ttext[i+1]) {
      case 'a': // format requested
       i+=3;
       Change_Attributes(ctoi(ttext[i-1]));
      break;
      case 'c': // color requested
       i+=2; i1=0;
       while (isdigit(ttext[i]))
        ttcolor[i1++]=ttext[i++];
       ttcolor[i1]='\0';
       if (!flag && atoi(ttcolor) && !printscreenmode)
        Change_Color(atoi(ttcolor));
       if (!flag && atoi(ttcolor)==0) // restore original
        Change_Color(tcolor);
      break;
      case 'n': // newline
       i+=2;
       if (rowinprint+1<=tfield->pt.y+tfield->size.y) {
        for (i1=columninprint;i1<tfield->pt.x+tfield->size.x;i1++)
         addch(SPACE);
        columninprint=tfield->pt.x;
       ++rowinprint; }
      break;
      case 'b': // bell
       i+=2;
       if (field->id==currentfield)
        beep();
      break;
      case 'f': // flash
       i+=2;
       if (field->id==currentfield)
        flash();
      break;
      case 'p': // alignment, handled before
       i+=3;
      break;
      case '>': // internal link
       if ((i1=isfieldtextlink(field, linkparameters))==0) {
        i+=1;
        break;
       }
       i+=i1 + 3;
      break;
      default:
     break; }
     if (ttext[i]=='\\') { // if next char is \, will not appear and command will be skipped
      --i;
     continue; }
    }
    if (rowinprint+1>tfield->pt.y+tfield->size.y)
     break; 
    if (i>=strlen(ttext))
     break;
    gotoxy(columninprint, rowinprint);
    if (flag<2)
     addch(ttext[i]);
    screen[columninprint][rowinprint]=ttext[i];
    ++columninprint;
    if (columninprint>tfield->pt.x+tfield->size.x-1) {
     columninprint=tfield->pt.x;
    ++rowinprint; }
   }
   // remove attributes
   Change_Attributes(NORMAL);
    
 return 0;   
}

// show field id and highlight
int Show_Field_ID(Annotated_Field *tfield)
{
  int trecord=tfield->id;
  int highlightcolor=(record[trecord].color==highlightcolors[1]) ? highlightcolors[0] : highlightcolors[1];
  
  if (!record[trecord].active)
   return -1;
  
   Change_Color(highlightcolor);
   attron(A_BLINK);
   gotoxy(record[trecord].pt.x, record[trecord].pt.y);
   printw("%d", tfield->id+1);
   refresh();
   attroff(A_BLINK);
   
 return 0;
}
  
// arrange string for field text
void Generate_Field_String(Annotated_Field *field, char *ttext)
{
 int i, n=0;
 char formula[LMAXCOMMAND];
     
  Field *tfield=&record[field->id];
  if (record[tfield->id].formula)
   strcpy(field->formula, field->text);
  
  switch (tfield->type) {
   case NUMERICAL:
    if (strcmp(tfield->automatic_value, EMPTYSTRING) && !isautomaticvalueformatinstruction(tfield->id))
     field->number=atof(tfield->automatic_value);
    if (tfield->formula) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, EMPTYSTRING))
      strcpy(formula, tfield->automatic_value);
     i=parseformulaforfunctions(formula);
     if (!i)
      i=parseformulaforerrors(formula);
     if (!i)
      i=parenthesesincluderforpolishreversecalculator(formula);
     if (!i) {
      reversepolishcalculatorequalizer(formula, currentrecord);
     field->number=reversepolishcalculator(formula); }
     else
    field->number=0; }
//     field->number=0; }
    strcpy(ttext, dtoa(field->number));
    limitsignificantnumbers(ttext, tfield->decimals);
    if (isautomaticvalueformatinstruction(tfield->id))
     formatmonetarystring(ttext);
    appendsuffix(ttext, tfield->id);
   break;
   case CALENDAR:
    strcpy(ttext, field->text);
   break;
   case STRING:
    strcpy(ttext, field->text);
    if ( strcmp(tfield->automatic_value, EMPTYSTRING) && !isautomaticvalueformatinstruction(tfield->id) ) {
     strcpy(ttext, tfield->automatic_value);
     if ( (i=isfieldscriptdirector(tfield->id)) )
      ttext[i-1]='\0';
    }
    if (tfield->formula) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, EMPTYSTRING))
      strcpy(formula, tfield->automatic_value);
     i=stringformulacalculator(formula, currentrecord);
     if (i) {
      cropstring(formula, MAXSTRING);
    strcpy(ttext, formula); } }
   break; 
   case MIXEDTYPE:
    strcpy(ttext, field->text);
    if ( strcmp(tfield->automatic_value, EMPTYSTRING) )
     strcpy(ttext, tfield->automatic_value);
    if (tfield->formula) {
     if ( strcmp(tfield->automatic_value, EMPTYSTRING) )
      strcpy(field->text, tfield->automatic_value);
     strcpy(formula, field->text);
     // parse string functions
     stringformulacalculator(formula, currentrecord);
     // now mathematical formulae
     i=parseformulaforfunctions(formula);
     if (!i)
      i=parseformulaforerrors(formula);
     if (!i)
      i=parenthesesincluderforpolishreversecalculator(formula);
     if (!i)
      n=reversepolishcalculatorequalizer(formula, currentrecord);
     if (n)
      field->number=reversepolishcalculator(formula);
     else
    field->number=0; }
//     field->number=0; }
    if (field->number || record[field->id].buttonbox==BUTTONSCREEN) {
     strcpy(ttext, dtoa(field->number));
     limitsignificantnumbers(ttext, tfield->decimals);
     if (isautomaticvalueformatinstruction(tfield->id))
      formatmonetarystring(ttext);
    appendsuffix(ttext, tfield->id); }
    if (tfield->formula && !field->number) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, EMPTYSTRING))
      strcpy(formula, tfield->automatic_value);
     i=stringformulacalculator(formula, currentrecord);
     if (i) {
      cropstring(formula, MAXSTRING);
    strcpy(ttext, formula); } }
    break;
    case CLOCK:
     Bring_DateTime_Stamp(ttext, tfield->id);     
    default:
  break; }
  strcpy(field->text, ttext);
  // clear neighbouring signs
  if (ttext[0]=='<') {
   for (i=0;i<strlen(ttext);i++)
    ttext[i]=ttext[i+1];
   ttext[i]='\0';
  }
  if (ttext[strlen(ttext)-1]=='>')
   ttext[strlen(ttext)-1]='\0';
  
  // restore button screen
  if (record[field->id].buttonbox==BUTTONSCREEN)
   record[field->id].type=STRING;
}

// generate (if any) dependant field string
int Generate_Dependant_Field_String(Annotated_Field *field, char *ttext)
{
  int i, n;
  dummyfieldsperrecord=0; 
  dummyrecordsnumber=0;
  
   // see if dependancy occurs
   for (i=0;i<relationships.size();i++)
    if (field->id==relationships[i].localFields[1]-1)
     break;
   if (i==relationships.size())
    return 0;
   Read_External_Database(i);
   
   // read external database
   dummyfieldsperrecord=externalrecord[i].size();
   dummyrecordsnumber=externalrecords[i].size()/externalrecord[i].size();
   if (record[relationships[i].localFields[1]-1].fieldlist) {
    relationships[i].extFields[0]=1;
   relationships[i].localFields[0]=1; }
   if (relationships[i].extFields[0]<1 || relationships[i].extFields[1]<1 || relationships[i].localFields[0]<1 || relationships[i].localFields[1]<1 || relationships[i].extFields[0]>dummyfieldsperrecord || relationships[i].extFields[1]>dummyfieldsperrecord || relationships[i].localFields[0]>fieldsperrecord || relationships[i].localFields[1]>fieldsperrecord)
    return 0;
   
   // destination local field is fieldlist, return success and keep dummyrecords for reference in Scan_Input
   if (record[relationships[i].localFields[1]-1].fieldlist) {
    externalreferencedatabase=i;
   return 2; }
   // dependancy check, if localFields[1] is not fieldlist
   for (n=0;n<dummyrecordsnumber;n++)
    if (!strcmp(externalrecords[i][(n*dummyfieldsperrecord)+relationships[i].extFields[0]-1].text, records[(currentrecord*fieldsperrecord)+relationships[i].localFields[0]-1].text))
     break;
   // no equal field found, return 0
   if (n==dummyrecordsnumber)
    return 0;
   // copy external to local field
   strcpy(records[(currentrecord*fieldsperrecord)+field->id].text, externalrecords[i][(n*dummyfieldsperrecord)+relationships[i].extFields[1]-1].text);
   strcpy(ttext, records[(currentrecord*fieldsperrecord)+field->id].text);
   
  return 1;
}

// export to comma separated values file
int Export_Database(char *filename)
{
  int i, n, flag;
  char tstring[MAXSTRING];
  ofstream outfile(filename);
  if (!outfile) {
   Show_File_Error(filename);
  return -1; }
  
   for (i=0;i<recordsnumber;i++) {
    for (n=0;n<fieldsperrecord;n++) {
     strcpy(tstring, records[(i*fieldsperrecord)+n].text);
     flag=(n<fieldsperrecord-1) ? 0 : 1;
     stringquotesencloser(tstring, flag);
    outfile << tstring << " "; }
   outfile << endl; }
   outfile.close();
   
   strcpy(tstring, itoa(recordsnumber));
   strcat(tstring, " database record");
   if (recordsnumber>1)
    strcat(tstring, "s");
   strcat(tstring, " exported");
   Show_Message(1, 24, 4, tstring, 1500);
   Show_Menu_Bar();
   
 return 0;
}

// import from comma separated values file
int Import_Database(char *filename)
{
  int i1, i, n, fieldsreadperline=0, trecordsnumber=0, recordid=0;
  char t, tstring[MAXSTRING*2];
  vector <Annotated_Field> trecords;
  ifstream infile(filename);
  if (!infile) {
   Show_File_Error(filename);
  return -1; }
  
  while (infile) {
   i1=0;
   while (infile) {
    t=infile.get();
    if (t==COMMA) {
     n=0;
     while (isspace(tstring[n++]));
     if (tstring[--n]!=QUOTE)
    break; }
    if (t==COMMA) {
     n=i1-1;
     while (isspace(tstring[n--]));
     if (tstring[n+1]==QUOTE)
    break; }
    if (t!='\n')
     tstring[i1++]=t; 
    else
   break; }
   tstring[i1]='\0';
   ++fieldsreadperline;
    if (fieldsreadperline>fieldsperrecord)
     break;
   limitspaces(tstring);
   if (isinquotes(tstring))
    stringquotesopener(tstring);
   Annotated_Field tfield(recordid++, 0, tstring, const_cast <char *> (" "));
   trecords.push_back(tfield); 
   if (t=='\n') {
    fieldsreadperline=0;
    recordid=0;
   ++trecordsnumber; } }
   vector<Annotated_Field>::iterator p=trecords.end();
   --p;
   trecords.erase(p); // erase last created
   --fieldsreadperline;
   infile.close();
   
   if (fieldsreadperline>fieldsperrecord) {
    Show_Message(1, 24, 1, "incompatible data types file", 1750);
   return -1; }

   for (i1=0;i1<trecords.size();i1++)
    records.push_back(trecords[i1]);
   Read_Write_db_File(1);
   Read_Write_db_File();
   strcpy(tstring, itoa(trecordsnumber));
   strcat(tstring, " database record");
   if (trecordsnumber>1)
    strcat(tstring, "s");
   strcat(tstring, " imported");
   Show_Message(1, 24, 4, tstring, 1500);
   Show_Menu_Bar();

 return 0;
}

// selectively import fields from external dbfile
int Import_External_db_File(char *filename)
{
  int i, fieldshown=0, t;
  char tstring[MAXSTRING];
  strcpy(tstring, dbfile);
  strcpy(dbfile, filename);
  Reccurse_File_Extension(dbfile, 2);
  if (!tryfile(dbfile)) {
   Show_Message(1, 24, 1, "external database file unreachable", 2000);
  return -1; }
  
  Read_Write_db_File();
  dummyrecord=record;
  dummyrecords=records;
  dummyfieldsperrecord=fieldsperrecord;
  dummyrecordsnumber=recordsnumber;
  strcpy(dbfile, tstring);
  Reccurse_File_Extension(dbfile, 2);
  Read_Write_db_File();
  int endofappend=recordsnumber, chosendestinationfield;

  Show_Menu_Bar(3);
  Change_Color(YELLOW);
  gotoxy(1, 24);
  printw("external .db file has %d fields per record and a total of %d records", dummyfieldsperrecord, dummyrecordsnumber);
  refresh();
  Sleep(2000);
  
   Change_Color(BLUE);
   while (t!=ESC && t!='\n') {
    Show_Menu_Bar(1);
    Change_Color(BLUE);
    gotoxy(1, 24);
    printw("id:%d title:%.15s type:%d formula:%d automatic value:%.10s", fieldshown+1, dummyrecord[fieldshown].title, dummyrecord[fieldshown].type, dummyrecord[fieldshown].formula, dummyrecord[fieldshown].automatic_value);
    refresh();
    t=sgetch(); 
    if (t==RIGHT)
     t=DOWN;
    if (t==LEFT)
     t=UP;
    switch (t) {
     case DOWN:
      if (fieldshown<dummyfieldsperrecord-1)
       ++fieldshown;
     break;
     case UP:
      if (fieldshown)
       --fieldshown;
    break; } }
    if (t==ESC)
     return -1;       
    for (i=0;i<fieldsperrecord;i++)
     Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
    Show_Menu_Bar(1);
    Show_Message(1, 24, 4, "to local field id:", 0);
    chosendestinationfield=Scan_Input(1, 1, fieldsperrecord);
    if (chosendestinationfield<1 || chosendestinationfield>fieldsperrecord)
     return -1;
    --chosendestinationfield;
    if (dummyrecordsnumber>recordsnumber) {
     Show_Menu_Bar(1);
     Show_Message(1, 24, 1, "external .db file has more records than current. append (y/n):", 0);
     t=sgetch();
     if (t=='y') {
      endofappend=dummyrecordsnumber; 
      for (i=recordsnumber;i<dummyrecordsnumber;i++)
    Initialize_Record(); } }
    
    // now copy to endofappend
    for (i=0;i<endofappend;i++)
     strcpy(records[(i*fieldsperrecord)+chosendestinationfield].text, dummyrecords[(i*dummyfieldsperrecord)+fieldshown].text);
    Read_Write_db_File(1);

   Show_Menu_Bar(1);
   Show_Message(1, 24, 4, "import from external .db file complete", 1500);
    
 return 0;
}

// read-write relationships format dbname ext_field local_field ext_field local_field, first pair equalizes relationship, MAXFIELDS denotes irrelevance
int Read_Write_Relationships(int mode) // 0 read all, 1 write/add
{
  int i;
  char c, ttext[RELATIONSHIPSLENGTH];
  Relationship trelationship;
  fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);
  dbfileaccess.seekg(startofrecords, ios::beg);
  dbfileaccess.seekp(startofrecords, ios::beg);
  
   if (!mode) {
    relationships.clear();
    for (i=0;i<RELATIONSHIPSLENGTH;i++) {
     dbfileaccess.get(c);
    ttext[i]=charcoder(c, 1); }
    ttext[i]='\0';
    istringstream trelationships(ttext);
    while (trelationships) {
     trelationships >> trelationship.extDbname;
     trelationships >> trelationship.extFields[0];
     trelationships >> trelationship.extFields[1];
     trelationships >> trelationship.localFields[0];
     trelationships >> trelationship.localFields[1];
    relationships.push_back(trelationship); } 
    vector<Relationship>::iterator p=relationships.end();
    --p;
   relationships.erase(p); }
   
   else {
    for (i=0;i<relationships.size();i++) {
     if (!i)
      strcpy(ttext, relationships[i].extDbname);
     else
      strcat(ttext, relationships[i].extDbname);
     strcat(ttext, " ");
     strcat(ttext, itoa(relationships[i].extFields[0]));
     strcat(ttext, " ");
     strcat(ttext, itoa(relationships[i].extFields[1]));
     strcat(ttext, " ");
     strcat(ttext, itoa(relationships[i].localFields[0]));
     strcat(ttext, " ");
     strcat(ttext, itoa(relationships[i].localFields[1]));
    strcat(ttext, " "); }
    for (i=strlen(ttext);i<RELATIONSHIPSLENGTH;i++)
     ttext[i]=SPACE;
    stringcodedecode(ttext, ttext);
    for (i=0;i<strlen(ttext);i++)
   dbfileaccess.put(ttext[i]); }
    
  dbfileaccess.close();
    
 return 0;
}

// setup necessary variables
void Initialize_Database_Parameters(int mode)
{
 int i;
 
  if (!mode) {
   currentrecord=Read_Write_Current_Parameters(0);
   currentmenu=Read_Write_Current_Parameters(1);
   menubar=Read_Write_Current_Parameters(2);
   autosave=Read_Write_Current_Parameters(3);
   currentfield=Read_Write_Current_Parameters(4);
  }
   
   // determine button boxes
   for (i=0;i<record.size();i++)
    Determine_Button_Box(i);
}

// load database
void Load_Database(int pagenumber)
{
  strcpy(dbfile, pages[pagenumber]);
  Read_Write_db_File();
  Initialize_Database_Parameters();
}

// pages selector - editor
int Pages_Selector(int pagetochange)
{
  int i, t, tpage;
  
  tpage=currentpage;
  
  if (pagetochange==-1) {
   Change_Color(BLUE);
   while (t!=ESC && t!='\n') {
    Show_Menu_Bar(1);
    Change_Color(BLUE);
    gotoxy(1, 24);
    printw("database #%d:%s", currentpage+1, pages[currentpage]);
    refresh();
    t=sgetch(); 
    if (t==RIGHT)
     t=DOWN;
    if (t==LEFT)
     t=UP;
    // handle a bit of mouse
    if (t==KEY_MOUSE)
     if (((t=wheelmousemove())))
      t=(t==SHIFT_RIGHT) ? DOWN : UP;
    switch (t) {
     case INSERT:
      memset(input_string, 0, sizeof(input_string));
      Scan_Input(input_string, 1, 24, 5);
      Reccurse_File_Extension(input_string, 2);
      for (i=0;i<pagesnumber;i++) {
       if (!strcmp(input_string, pages[i])) {
        Show_Message(1, 24, 1, "database already exists in pages", 1500);
      break; } }
      if (i<pagesnumber)
       break;
      if (tryfile(input_string) && pagesnumber<MAXPAGES-1) {
       strcpy(pages[pagesnumber], input_string);
       ++pagesnumber;
      Show_Message(1, 24, 1, "page added to databases list", 1500); }
      else
       Show_Message(1, 24, 1, "nonexisting database file", 1500);
     break;
     case DELETE:
      if (pagesnumber>1) {
       for (i=currentpage;i<pagesnumber-1;i++)
        strcpy(pages[i], pages[i+1]);
       --pagesnumber;
       memset(pages[pagesnumber], 0, sizeof(pages[pagesnumber]));
       if (currentpage>0)
        --currentpage;
       Show_Menu_Bar(1);
      Show_Message(1, 24, 1, "page removed", 1500); }
     break;
     case HOME:
      currentpage=0;
     break;
     case END:
      currentpage=pagesnumber-1;
     break;
     case DOWN:
      if (currentpage<pagesnumber-1)
       ++currentpage;
     break;
     case UP:
      if (currentpage)
       --currentpage;
    break; } }
  }
  else
   currentpage=pagetochange;
  
  if (currentpage!=tpage) {
   changedrecord=1;
   checkalteredparameters();
  }
  Read_Write_Current_Parameters(4, 1); // write out currentfield in case of change
  Load_Database(currentpage);
  
 return currentpage;
}

// remove garbage from database file
int Clean_Database(char *filename)
{
  char t;
  int i=0;
    
   fstream dbfileaccess(dbfile, ios::in | ios::out | ios::binary);
   if (!dbfileaccess)
    return -1;
   dbfileaccess.seekg(0, ios::beg);
   dbfileaccess.seekp(0, ios::beg);
    
    while (dbfileaccess) {
     t=(charcoder(dbfileaccess.get(), 1));
     ++i;
     if (!isprintablecharacter(t) && t!='\n')
      t=SPACE;
     dbfileaccess.seekp(i-1, ios::beg);
    dbfileaccess.put(charcoder(t)); }

 return 0;
}

// setup mouse menubar choices
void Set_Mouse_Menus()
{
  char entries[MAXWORDS][MAXSTRING];
  int nentries, i, c;

   nentries=assignstringvaluestoarray(const_cast<char *> (menubaroptions), entries, MAXWORDS);
   for (i=0;i<nentries;i+=4) {
    if ((c=atoi(entries[i+3]))==0)
     c=(int) entries[i+3][0];
    ButtonBarMenuEntry tentry(atoi(entries[i]), atoi(entries[i+1]), atoi(entries[i+2]), c);
    buttonbarmenus.push_back(tentry);
   }
}

int Activate_Menubar_Choice(int x)
{
  int c=0, i;
    
   for (i=0;i<buttonbarmenus.size();i++)
    if (currentmenu == buttonbarmenus[i].choiceMenuId && x>=buttonbarmenus[i].choiceStartx && x<buttonbarmenus[i].choiceEndx)
     break;
   if (i<buttonbarmenus.size()) {
    Change_Color(highlightcolors[0]);
    gotoxy(buttonbarmenus[i].choiceStartx + 1, menulines[currentmenu]);
    for (x=buttonbarmenus[i].choiceStartx;x<buttonbarmenus[i].choiceEndx - 1;x++)
     addch(menutexts[currentmenu][x]);
    refresh();
    Sleep(150);
    c=buttonbarmenus[i].choiceKey;
   }
    
 return c;
}

// parse mousemenucommands vector for commands
int fetchmousemenucommand()
{
  vector<int>::iterator p;
  int command;
  
   p=mousemenucommands.begin();
   command=*p;
   mousemenucommands.erase(p);
    
 return command;
}

// determine if record is buttonbox
int Determine_Button_Box(int field_id)
{
  int i;
  char command[MAXSTRING], tautomaticvalue[MAXSTRING];
  
    record[field_id].buttonbox=NOBUTTON;
    if (record[field_id].size.x==1 && record[field_id].size.y==1 && record[field_id].editable && record[field_id].type!=VARIABLE && record[field_id].type!=PROGRAM)
     record[field_id].buttonbox=TICKBOX; // tick box
    for (i=0;i<buttonkeystotal;i++) {
     if (!strcmp(buttonkeys[i], record[field_id].automatic_value) && record[field_id].type!=VARIABLE) { // button box
      if (record[field_id].fieldlist-1!=field_id)
       record[field_id].buttonbox=BUTTONBOX;
       record[record[field_id].fieldlist-1].buttonbox=BUTTONSCREEN; // button screen
       record[record[field_id].fieldlist-1].type=MIXEDTYPE;
//        record[record[field_id].fieldlist-1].fieldlist=0;
     strcpy(record[record[field_id].fieldlist-1].automatic_value, EMPTYSTRING); }
    }
     strcpy(tautomaticvalue, record[field_id].automatic_value);
     if (strcmp(tautomaticvalue, EMPTYSTRING) && (scantextforcommand(tautomaticvalue, command)) && record[field_id].fieldlist-1==field_id) // button command
      record[field_id].buttonbox=BUTTONCOMMAND;
     if (record[field_id].size.x==0 && record[field_id].size.y==0 && record[field_id].fieldlist-1==field_id) // automatic scripts, repetitions in color
      record[field_id].buttonbox=AUTOMATICSCRIPT;
      
     if (record[field_id].buttonbox>NOBUTTON)
      record[field_id].type=STRING;
   
 return record[field_id].buttonbox;
}

// determine if automatic value is a script command
int Determine_Script_Direction(int field_id)
{
  if ( isfieldscriptdirector(field_id) == NOSCRIPT )
   return NOSCRIPT;
    
 return record[field_id].decimals;
}

// execute script command
void Execute_Script_Command(int field_id)
{
  int startpt=isfieldscriptdirector(field_id)-1;
  runscript=1;
  scriptcommand=fetchcommand(record[field_id].automatic_value, startpt);
}

// return command from automatic value
char *fetchcommand(char *text, int startpt)
{
  static char command[MAXSTRING];
  char ttext[MAXSTRING];
  int i, i1=0;
  
  for (i=startpt;i<strlen(text);i++)
   ttext[i1++]=text[i];
  ttext[i1]='\0';

   scantextforcommand(restructurecommand(ttext), command);
  
 return &command[0];
}

// restructure command in automatic_value
char *restructurecommand(char *command)
{
  char tcommand[MAXSTRING];
    
   limitspaces(command);
   if (command[0]==COMMAND) {
    strcpy(tcommand, " ");
   strcat(tcommand, command); }
   else
    strcpy(tcommand, command);
   
   strcpy(command, tcommand);
    
 return command;
}


// reduce text by times at end char
void Delete_Field_Entry(int field_id)
{
  strcpy(records[(currentrecord*fieldsperrecord)+field_id].text, " ");
  records[(currentrecord*fieldsperrecord)+field_id].number=0;
}

// push space on field
void pushspaceonfield(int field_id)
{
  int i1;
  int linkparameters[2];
  char ttext[MAXSTRING];
  
  if (field_id==-1)
   field_id=currentfield;
    
      // tickboxes
      if (record[field_id].buttonbox==TICKBOX) {
       records[(currentrecord*fieldsperrecord)+field_id].text[0]=(records[(currentrecord*fieldsperrecord)+field_id].text[0]=='X') ? ' ' : 'X';
       if (autosave)
        Read_Write_Field(records[(currentrecord*fieldsperrecord)+field_id], fieldposition(currentrecord, field_id), 1); 
      return; }
      // buttonboxes
      if (record[field_id].buttonbox==BUTTONBOX) {
       for (i1=0;i1<buttonkeystotal;i1++)
        if (!strcmp(buttonkeys[i1], record[field_id].automatic_value))
         break;
        if (record[record[field_id].fieldlist-1].buttonbox==BUTTONSCREEN) {
         if (i1<=buttonkeystotal-4) {
          if (!isdigit(buttonkeys[i1][0]) && !isdecimalseparator(buttonkeys[i1][0]) && iscalculationsign(buttonkeys[i1][0]))
           sprintf(ttext, " %s ", buttonkeys[i1]);
          else
           strcpy(ttext, buttonkeys[i1]);
          strcat(records[(currentrecord*fieldsperrecord)+record[field_id].fieldlist-1].text, ttext);
         }
         else { // ac, del, exec
          switch (i1) {
           case buttonkeystotal-3: // AC
            Delete_Field_Entry(record[field_id].fieldlist-1);
           break;
           case buttonkeystotal-2: // DEL
            records[(currentrecord*fieldsperrecord)+record[field_id].fieldlist-1].text[strlen(records[(currentrecord*fieldsperrecord)+record[field_id].fieldlist-1].text)-1]='\0';
           break;
           case buttonkeystotal-1: // EXEC
            records[(currentrecord*fieldsperrecord)+field_id].number=0;
            record[record[field_id].fieldlist-1].type=NUMERICAL; // trick to bring reversepolishcalculator
          break; }
       Read_Write_Field(records[(currentrecord*fieldsperrecord)+record[field_id].fieldlist-1], fieldposition(currentrecord, record[field_id].fieldlist-1), 1); }
      } 
     return; }
     if (record[field_id].buttonbox==BUTTONCOMMAND) {
      if (runscript) {
       keyonnextloop.push_back(SPACE);
       runscript=0;
      return; }
      Execute_Script_Command(field_id);
    }
    if (record[field_id].buttonbox==AUTOMATICSCRIPT) {
     lastfieldrepeated=field_id;
     if (fieldrepetitions[field_id]>-1 && fieldrepetitions[field_id]!=59)
      --fieldrepetitions[field_id];
     if (fieldrepetitions[field_id]==59)
      fieldrepetitions[field_id]=0;
     Execute_Script_Command(field_id);
    }
    if (isfieldtextlink(&records[(currentrecord*fieldsperrecord)+currentfield], linkparameters)) {
     currentrecord=linkparameters[0] - 1;
     currentfield=linkparameters[1] - 1;
    }
}

// copy to clipboard
void copytoclipboard()
{
  strcpy(clipboard, records[(currentrecord*fieldsperrecord)+currentfield].text);
}

// paste from clipboard
void pastefromclipboard()
{
  strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, clipboard);
  if (autosave)
   Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1);
}

// toggle autosave on/off
void toggleautosave()
{
  autosave=(autosave) ? 0 : 1;
  Show_Menu_Bar(1);
  Show_Message(1, 24, 2, "autosave:", 0);
  Show_Message(10, 24, 2, onoff[autosave], 1500);
  Read_Write_Current_Parameters(3, 1);
}

// add or play program
int addorplayprogram(int programid)
{
  int i;
  programid=ctoi(programid);
  
   for (i=0;i<record.size();i++)
    if (record[i].type==PROGRAM && programid==ctoi(record[i].title[0]))
     break;
   
   if (i==record.size()) {
    Change_Color(MAGENTA);
    gotoxy(1, 24);
    printw("keys:");
    Scan_Input(input_string, 6, 24, 5);
    for (i=0;i<strlen(input_string);i++)
     input_string[i]=tolower(input_string[i]);
    Add_Field(PROGRAM, itoa(programid), input_string);
    strcpy(input_string, " ");
    Show_Menu_Bar(1);
    Show_Message(1, 24, 5, "program saved", 750);
   return record.size(); }
    
   breaktexttokeys(records[i].text); // text from any record will do
    
 return 0;
}

// output screen array to file
void outputscreenarraytofile()
{
  int i, i1;
  
   for (i1=1;i1<25;i1++) {
    for (i=1;i<81;i++)
     fputc(screen[i][i1], out);
    fputc('\n', out); }
   
   fflush(out);
}
