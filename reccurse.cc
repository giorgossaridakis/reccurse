// reccurse, the filemaker of ncurses, version 0.241

// included libraries
// C
#include <unistd.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <termios.h>
#include <sys/stat.h>
#include <signal.h>
// C++ 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
// stl
#include <vector>
#include <string>
#include <cstring>

// definitions
// numericals
#define MAXSTRING 80 // characters in a regular string
#define MAXTITLE 20 // characters in a a title string
#define MAXNUMBERDIGITS 18 // digits in a number
#define MAXSUFFIXCHARS 3 // max string for number suffixes
#define DEFAULT_DECIMALS 2 // decimal positions
#define MAXFIELDS 999 // fields per record
#define MAXRECORDS 9999 // records limit
#define FIELDSIZE MAXSTRING*2+MAXNUMBERDIGITS+15  // +7 would do
#define MAXSEARCHDEPTH 5
#define HORIZONTALLY 0
#define VERTICALLY 1
#define version 0.241

// keyboard
#define UP 53
#define LEFT 54
#define RIGHT 55
#define DOWN 52
#define ESC 27
#define SPACE 32
#define BACKSPACE 57 // 0 first
#define DELETE 124 // 0 first
#define INSERT 125 // 0 first
#define HOME 56 // 0 first
#define END 154 // 0 first
#define COPY 11 // 0 first
#define PASTE 22 // 0 first
#define TAB 9
#define SHIFT_TAB 147
#define END_OF_RECORDS 130
#define START_OF_RECORDS 131

using namespace std;

// global variables
int menubar;
int autosave=1;
int recordsdemo=0;
int currentmenu=0;
int currentfield;
int recordsnumber=0;
int currentrecord=0;
int startofrecords=0;
int fieldsperrecord=0;
int alteredparameters=0;
char rcfile[MAXSTRING], dbfile[MAXSTRING];
const char *onoff[]= { "off", "on" };
char clipboard[MAXSTRING];
int menucolors[5]={ 5, 6, 4, 3, 1 };
int menulines[5]={ 24, 24, 24, 24, 24 };
char infotext[MAXSTRING];
const char *menutexts[]={ "<tabshiftarrows|< >|home&end|<g>|<e>dit|<o>ptions|ex<t>ra|<m>enubar|<ESC>quit", "<a>utosave on/off|<l>oad database|<s>ave database|<h>elp page|<ESC>main menu", "e<d>it|<c>opy|<DEL>ete|<j>oin|di<v>ide|datestam<p>|<INS>more|<ESC>main menu", "database <i>nformation|<f>ind|so<r>t records|set<u>p database|<ESC>main menu", "really quit ?" };  //main, options, edit, extra, quit

struct Points {
 int x;
int y; } ;

class Field {
 public:
  // from .rc file
  int id;
  char title[MAXTITLE];
  int title_position;
  char title_attributes[9];
  int title_color;
  struct Points pt;
  struct Points size;
  char attributes[9];
  int color;
  int box;
  int box_color;
  int type; // 0 number, 1 date&time, 2 string
  int decimals;
  char suffix[MAXSUFFIXCHARS];
  int formula;
  int fieldlist;
  int editable;
  int active;
  char automatic_value[MAXSTRING];
  // constructors, destructor
  Field(int i1, char s1[MAXSTRING], int i2, char s2[9], int i3, int i4, int i5, int i6, int i7, char s3[9], int i8, int i9, int i10, int i11, char s4[MAXSUFFIXCHARS], int i12, int i13, int i14, int i15, int i16, char s5[MAXSTRING]) { id=i1; strcpy(title,s1); title_position=i2; strcpy(title_attributes,s2); title_color=i3; pt.x=i4; pt.y=i5; size.x=i6; size.y=i7; strcpy(attributes, s3); color=i8; box=i9; box_color=i10; type=i11; strcpy(suffix, s4);  decimals=i12, formula=i13; fieldlist=i14; editable=i15; active=i16;  strcpy(automatic_value, s5); } ;
  Field() { } ;
~Field() { } ; } ;

class Annotated_Field {
  // to be annotated in each record
  public:
   int id; // same as Field
   double number;
   char text[MAXSTRING];
   char formula[MAXSTRING]; 
   // constructors, destructor
   Annotated_Field(int i1, double f1, char s1[MAXSTRING], char s2[MAXSTRING]) { id=i1; number=f1; strcpy(text, s1); strcpy(formula, s2); } ;
   Annotated_Field(int i1, double f1, const char s1[MAXSTRING], const char s2[MAXSTRING]) { id=i1; number=f1; strcpy(text, s1); strcpy(formula, s2); } ;
   Annotated_Field() { } ;
~Annotated_Field() { } ; } ;

vector<Field> record, dummyrecord;
vector<Annotated_Field> records, dummyrecords;

// functions
void Intro_Screen();
int End_Program(int code=0);;
void Reccurse_File_Extension(char *filename, int flag=0);
int Read_rc_File();
int Write_rc_File(char *file);
int Read_Write_db_File(int mode=0);
int Read_Write_Current_Parameters(int item, int mode=0); // 0 read, 1 write
void Read_Record_Field(ifstream &instream, Field &tfield);
void Read_Record_Field(istringstream &instream, Field &tfield);
void Write_Record_Field(ofstream &outstream, Field &tfield);
void Write_Record_Field(char *ttext, Field &tfield);
void Initialize_Record();
int Add_Field();
void Delete_Field(int field_id);
int Join_Fields(int field_id, int mode);
void Renumber_Field_References(int startingfield);
int Divide_Field(int field_id, int mode);
void Bring_DateTime_Stamp(char tdatetime[MAXSTRING]);
int ispossibletickbox(int field_id);
int Read_Write_Field(Annotated_Field &tfield, long int field_position, int mode=0);
long int fieldposition(int record_id, int field_id);
long int fieldserialtofieldposition(int position);
void fieldserialtoparameters(int position, int &record_id, int &field_id);
int fieldparameterstoserial(int record_id, int field_id);
void Delete_Record(int record_id);
void Duplicate_Record(int record_id);
int Show_Record_and_Menu();
int Screen_String_Editor(Annotated_Field &tfield);
int negatekeysforcurrentmenu(int t);
void Show_Menu_Bar(int mode=0);
void Show_Help_Screen();
void Show_DB_Information();
int Show_Field(Annotated_Field *tfield, int flag=0);
int Show_Field_ID(Annotated_Field *tfield);
void Generate_Field_String(Annotated_Field *tfield, char *ttext);
int Export_Database(char *filename);
int Import_Database(char *filename);
int Import_External_db_File(char *filename);
int Clean_Database(char *filename);

// to be compiled together
#include "rcscr.cc"
#include "rclib.cc"
#include "rcpc.cc"
#include "rcsc.cc"
#include "rcfe.cc"

int main(int argc, char *argv[])
{
  int i;
  
  Init_Screen();
  Change_Color(58);
  char tmessage[MAXSTRING];
  signal(SIGINT, INThandler);

   Intro_Screen();
   
    if (argc>1) {
     strcpy(rcfile, argv[1]);
     strcpy(dbfile, argv[1]);
     Reccurse_File_Extension(rcfile, 1);
    Reccurse_File_Extension(dbfile, 2); }
    if (!tryfile(rcfile) && !tryfile(dbfile)) {
     strcpy(tmessage, "open file:");
     Show_Message(8, 19, 4, tmessage, 0);
     if (argc>1)
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
    strcpy(tmessage, "loaded file ");
    if (!tryfile(dbfile)) {
     i=Read_rc_File();
     if (i==-1) {
      // create field, write to file, do not exit
      ofstream outdbfile(dbfile, ios::binary);
      outdbfile.close();
      ofstream outrcfile(rcfile);
      outrcfile << "#date&time 0 000000000 3 10 5 10 2 000000000 5 1 1 1 2 $ 0 0 1 1 . " << endl << "#";
      outrcfile.close();
      strcpy(tmessage, "created file ");
    Read_rc_File(); } }
    strcat(tmessage, dbfile);
    // read record fields and records from dbfile
    Read_Write_db_File();
    Show_Message(8, 20, 2, tmessage, 1750);
    Show_Record_and_Menu();
   
  End_Program(0);
  
 return 0;
}

// introduction screen
void Intro_Screen()
{
   char c;
   FILE *f;
    
    // intro screen
    Change_Color(34);
    gotoxy(27,1);
    printw("Reccurse version %.3f", version);
    f=fopen("reccurse.pic", "rb");
    if (f) {
     attron(A_BLINK);
     gotoxy(1,6);
     while (c!=EOF) {
      c=charcoder(fgetc(f), 1);
      Change_Color(50);
      if (isalpha(c))
       Change_Color(24);
      if (c!=EOF)
     addch(c); }
     attroff(A_BLINK);
     fclose(f);
     Change_Color(5);
     gotoxy(8,15);
    printw("the record maker with Linux ncurses"); }
    
  refresh();
}

// goodbye
int End_Program(int code)
{
  if (!code) {
   if (alteredparameters) {
    char c;
    Show_Message(1, 24, 2, "save altered parameters (y/n):", 0);
    c=sgetch();
    if (tolower(c)=='y')
  Read_Write_db_File(4); } }
  
  Clean_Database(dbfile); 
  Read_Write_Current_Parameters(4, 1);
  char tmessage[MAXSTRING];
  strcpy(tmessage, "database closed, exiting reccurse with code ");
  strcat(tmessage, itoa(code, 10));
  strcat(tmessage, "...");
    
  Show_Menu_Bar(1);
  Show_Message(1, 24, 1, tmessage, 1500);
  End_Screen();
  
 exit(0);
}

// filename extension add/remove
void Reccurse_File_Extension(char *filename, int flag) // 0 remove, 1 add .rc, 2 add .db
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
  break; }
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
   End_Program(-2); } }
   
   if (mode==3) { // recreate rc data into string and rewrite rcfile & dbfile
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
    outdbfile.close(); }

   if (mode==4) { // only rewrite rcdata
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
   outdbfile.close(); }
   
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
    break; }
    
   dbfileaccess.close();
  
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
   dbfileaccess.seekg(startofrecords+parameterpositions[item], ios::beg);
   for (i=0;i<parametersize;i++)
    dbfileaccess.get(ttext[i]);
   ttext[i]='\0';
   stringcodedecode(ttext, ttext, 1);
  tvalue=atoi(ttext); }
  else {
   dbfileaccess.seekp(startofrecords+parameterpositions[item], ios::beg);
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
  Bring_DateTime_Stamp(tdatetime);
  
  for (i=0;i<fieldsperrecord;i++) {
   Annotated_Field tfield(i, 0, " ", " ");
  if (record[tfield.id].type==1)
   strcpy(tfield.text, tdatetime);
  records.push_back(tfield); }
  ++recordsnumber;
  
  Read_Write_db_File(1);
  Read_Write_db_File(); 
}

// add a field
int Add_Field(/* add parameters */)
{
  int i;
  vector <Annotated_Field>::iterator p;
  char ttext[MAXSTRING]; // for Generate_Field_String
  
   if (!recordsnumber) // initialization precaution
    return -1;
   if (fieldsperrecord>MAXFIELDS-1)
    return -1;
   Field tfield(fieldsperrecord, const_cast <char *> ("."), 0, const_cast <char *> ("000000000"), 58, 1, 1, 1, 1, const_cast <char *> ("000000000"), 58, 0, 58, 0, const_cast <char *> ("."), 2, 0, 0, 1, 1, const_cast <char *> ("."));
   record.push_back(tfield);
   Annotated_Field ttfield(fieldsperrecord, 0, " ", " ");
   for (i=0;i<recordsnumber;i++) {
    p=records.begin();
    p+=(i*(fieldsperrecord+1))+fieldsperrecord;
    records.insert(p, 1, ttfield);
   Generate_Field_String(&records[(i*fieldsperrecord)+fieldsperrecord+1], ttext); }
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
   Read_Write_db_File(3);
   Read_Write_db_File(1);
   Read_Write_db_File();  // precaution
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
  // adjust to bigger y size if HORIZONTAL join
  if (mode==HORIZONTALLY) {
   n=(record[fieldidentities[i]].size.y>record[fieldidentities[i+1]].size.y) ? record[fieldidentities[i]].size.y : record[fieldidentities[i+1]].size.y;
   record[fieldidentities[i]].size.y=n;
  record[fieldidentities[i]].size.x+=record[fieldidentities[i+1]].size.x; }
  else
   record[fieldidentities[i]].size.y+=record[fieldidentities[i+1]].size.y;
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

// divide a field into two parts
int Divide_Field(int field_id, int mode)
{
  int i;
  
  if (mode==HORIZONTALLY && record[field_id].size.x<2)
   return -1;
  if (mode==VERTICALLY && record[field_id].size.y<2)
   return -1;
  Add_Field();
  record[records[records.size()-1].id]=record[field_id];
  if (mode==HORIZONTALLY) {
   record[field_id].size.x/=2;
   record[records[records.size()-1].id].size.x/=2;
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
void Bring_DateTime_Stamp(char tdatetime[MAXSTRING])
{
 time_t now = time(0);
 int i;

  strcpy(tdatetime, ctime(&now));
  for (i=0;i<strlen(tdatetime);i++)
   if (tdatetime[i]=='\n')
    tdatetime[i]=SPACE;    
}

// possible tick-box if field is type string and size 1x1
int ispossibletickbox(int field_id)
{
  if (record[field_id].size.x==1 && record[field_id].size.y==1)
   return 1;
    
  return 0;
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
  long int position=startofrecords+11; // position after parameters
  
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
  int i, i1, n, trecordsnumber, run=1, c;
  int findresults[MAXSEARCHDEPTH+1][MAXRECORDS], tsortresults[MAXRECORDS];
  char input_text[MAXSTRING], tattributes[9];
  const char *alterscreenparameterskeys="/*-+.!@";
  struct FindSchedule {
   int field_id;
  char texttolookfor[MAXSTRING]; } ;
  vector <FindSchedule> findschedule;
  FindSchedule tfindschedule;
  vector <Annotated_Field> trecords;
  
   currentrecord=Read_Write_Current_Parameters(0);
   currentmenu=Read_Write_Current_Parameters(1);
   menubar=Read_Write_Current_Parameters(2);
   autosave=Read_Write_Current_Parameters(3);
   currentfield=Read_Write_Current_Parameters(4);
   while (run) {
    Clear_Screen();
    for (i=0;i<fieldsperrecord;i++)
     Show_Field(&records[(currentrecord*fieldsperrecord)+i]);
    if (currentfield>-1 && !recordsdemo)
     Show_Field(&records[(currentrecord*fieldsperrecord)+currentfield], 1);
    if (!recordsdemo)
     for (i=0;i<MAXRECORDS;i++)
      if (i<=recordsnumber)
       findresults[0][i]=i;
      else
       findresults[0][i]=-1;
     
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
    c=sgetch();
    c=negatekeysforcurrentmenu(c);
    if (!alteredparameters)
     for (i=0;i<strlen(alterscreenparameterskeys);i++)
      if (c==alterscreenparameterskeys[i])
       alteredparameters=1;
    switch (c) {
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
     case 'm': // from all menus
      ++menubar;
      menubar=(menubar==4) ? 0 : menubar;
      Read_Write_Current_Parameters(2, 1);
     break;
     case ESC: // from all menus
      if (recordsdemo) {
       recordsdemo=0;
       currentrecord=trecordsnumber; // restore currentrecord before find
      break; }
      if (!currentmenu) {
       currentmenu=4;
       menubar=1;
       Show_Menu_Bar();
       gotoxy(strlen(menutexts[currentmenu])+1, menulines[currentmenu]);
       c=sgetch();
       if (c==ESC || c=='y')
        run=0;
       else
        currentmenu=0; }
      else {
       currentmenu=0;
     Read_Write_Current_Parameters(1, 1); }
     break;
     case '>': // from all menus
      for (i=0;i<recordsnumber+1;i++) 
       if (findresults[0][i]>currentrecord) {
        currentrecord=findresults[0][i];
      break; }
      if (currentrecord>recordsnumber-1 && currentrecord+1<MAXRECORDS) {
       if (recordsdemo) {
        --currentrecord;
       break; }
       Show_Menu_Bar(1);
       Change_Color(1);
       gotoxy(1,24);
       printw("initiate record (y/n):");
       c=sgetch();
       if (tolower(c)=='y') {
        Initialize_Record();
       --currentrecord; }
       else
      --currentrecord; }
      if (!recordsdemo) 
       Read_Write_Current_Parameters(0, 1);
      Show_Menu_Bar();
     break;
     case '<': // from all menus
      for (i=currentrecord;i>-1;i--) 
       if (findresults[0][i]<currentrecord && findresults[0][i]>-1) {
        currentrecord=findresults[0][i];
      break; }
      if (!recordsdemo) 
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
      currentfield=0; } } }
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
      currentfield=fieldsperrecord-1; } } }
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
        if (i<fieldxidentities.size()-1)
       currentfield=fieldxidentities[i+1]; }
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
        if (i)
       currentfield=fieldxidentities[i-1]; }
     break;
     case 'g': // from all menus
      if (recordsdemo)
       break;
      Show_Menu_Bar(1);
      Change_Color(58);
      gotoxy(1, 24);
      printw("jump to record:");
      i=Scan_Input(1, 1, recordsnumber, 4)-1;
      if (i>-1 && i<recordsnumber)
       currentrecord=i;
     break;
     case '?': // from all menus
      Show_Help_Screen();
     break;
     // from menu 1
     case 'a':
      autosave=(autosave) ? 0 : 1;
      Show_Menu_Bar(1);
      Show_Message(1, 24, 2, "autosave:", 0);
      Show_Message(10, 24, 2, onoff[autosave], 1500);
      Read_Write_Current_Parameters(3, 1);
     break;
     case 'l':
      Read_Write_db_File();
      Show_Menu_Bar(1);
      Show_Message(1, 24, 2, "database loaded", 1500);
     break;
     case 's':
      Show_Menu_Bar(1);
      Read_Write_db_File(1);
      alteredparameters=0;
      Show_Menu_Bar(1);
      Show_Message(1, 24, 2, "database saved", 1500);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 5, "export .rc file (y/n):", 0);
      c=tolower(sgetch());
      if (c=='y') {
       Show_Menu_Bar(1);
       Show_Message(1, 24, 5, "filename:", 0);
       Scan_Input(0, 0, 1, 25);
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
      if (!strcmp(record[currentfield].automatic_value, ".") && record[currentfield].type!=1) {
       for (i=0;i<fieldsperrecord;i++)
        Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
       if (record[currentfield].fieldlist) {
        Show_Message(1, 24, 5, "fieldlist entry. <up> and <down> arrows, <insert> to bring relevant data", 1750);
        attron(A_BLINK);
        Show_Field(&records[(currentrecord*fieldsperrecord)+record[currentfield].fieldlist-1], 1);
        attroff(A_BLINK); }
      Screen_String_Editor(records[(currentrecord*fieldsperrecord)+currentfield]); }
      if (record[currentfield].type==1) {
       strcpy(input_string, records[(currentrecord*fieldsperrecord)+currentfield].text);
       Scan_Date(1, 24, input_string);
       strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, input_string);
       if (autosave)
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1); }
     break;
     case 'p':
      if (record[records[(currentrecord*fieldsperrecord)+currentfield].id].type && record[records[(currentrecord*fieldsperrecord)+currentfield].id].editable) {
       Bring_DateTime_Stamp(records[(currentrecord*fieldsperrecord)+currentfield].text);
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1); }
     break;
     case SPACE:
      if (record[currentfield].type==2 && !strcmp(record[currentfield].automatic_value, ".") && record[currentfield].editable && ispossibletickbox(records[(currentrecord*fieldsperrecord)+currentfield].id)) {
       records[(currentrecord*fieldsperrecord)+currentfield].text[0]=(records[(currentrecord*fieldsperrecord)+currentfield].text[0]=='X') ? ' ' : 'X';
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1); }
     break;
     case '+':
      record[currentfield].attributes[6]=(record[currentfield].attributes[6]=='1') ? '0' : '1';
     break;
     case '-':
      record[currentfield].attributes[2]=(record[currentfield].attributes[2]=='1') ? '0' : '1';
     break;
     case '*':
      record[currentfield].attributes[1]=(record[currentfield].attributes[1]=='1') ? '0' : '1';
     break;
     case '/':
      record[currentfield].attributes[4]=(record[currentfield].attributes[4]=='1') ? '0' : '1';
     break;
     case '.':
      record[currentfield].attributes[5]=(record[currentfield].attributes[5]=='1') ? '0' : '1';
     break;
     case '!':
      record[currentfield].color+=record[currentfield].color<58 ? 1 : -58;
     break;
     case '@':
      record[currentfield].color-=record[currentfield].color>2 ? 1 : -57;
     break;
     case 'c':
      for (i=0;i<fieldsperrecord;i++)
       Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 3, "source field:", 0);
      Change_Color(4);
      i=Scan_Input(1, 1, 10000, 5);
      Show_Menu_Bar(1);
      Show_Message(1, 24, 3, "destination field:", 0);
      Change_Color(4);
      n=Scan_Input(1, 1, 10000, 5);
      strcpy(records[(currentrecord*fieldsperrecord)+n-1].text, records[(currentrecord*fieldsperrecord)+i-1].text); 
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+n-1], fieldposition(currentrecord, n-1), 1);
     break;
     case 'u': // invoke editor
      Field_Editor();
      currentrecord=Read_Write_Current_Parameters(0);
     break;
     case DELETE:
      strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, " ");
      terminatestringatcharactersend(records[(currentrecord*fieldsperrecord)+currentfield].text);
      records[(currentrecord*fieldsperrecord)+currentfield].number=0;
      Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1);
     break;
     case INSERT:
      Show_Menu_Bar(1);
      Show_Message(1, 24, 4, "d<u>plicate, <d>elete record, <i>mport/e<x>port records, externa<l> .dbfile ?", 0);
      cleanstdin();
      c=tolower(sgetch());
      if (c!='u' && c!='d' && c!='x' && c!='i' && c!='l')
       break;
      switch (c) {
       case 'u':
        Duplicate_Record(currentrecord);
       break;
       case 'd':
        if (recordsnumber<2)
         break;
        Show_Menu_Bar(1);
         Show_Message(1, 24, 1, "delete entire record (y/n):", 0);
        c=sgetch();
        if (tolower(c)=='y')
         Delete_Record(currentrecord);
       break;
       case 'i':
        Show_Menu_Bar(1);
        Show_Message(1, 24, 5, "filename:", 0);
        Scan_Input(0, 0, 1, 25);
        Import_Database(input_string);
       break;
       case 'x':
        Show_Menu_Bar(1);
        Show_Message(1, 24, 5, "filename:", 0);
        Scan_Input(0, 0, 1, 25);
        Export_Database(input_string);
       break;
       case 'l':
        Show_Menu_Bar(1);
        Show_Message(1, 24, 4, "external dbfile:", 0);
        Scan_Input(0, 0, 1, 25);
        i=Import_External_db_File(input_string);
      break; }
     break;
     case COPY:
      strcpy(clipboard, records[(currentrecord*fieldsperrecord)+currentfield].text);
     break;
     case PASTE:
      strcpy(records[(currentrecord*fieldsperrecord)+currentfield].text, clipboard);
      if (autosave)
       Read_Write_Field(records[(currentrecord*fieldsperrecord)+currentfield], fieldposition(currentrecord, currentfield), 1);
     break;
     // menu 3
     case 'i':
      Show_DB_Information();
     break;
     case 'r':
      for (i=0;i<fieldsperrecord;i++)
       Show_Field_ID(&records[(currentrecord*fieldsperrecord)+i]);
      findschedule.clear();
      trecords.clear();
      findresults[0][recordsnumber]=-1;
      for (i=0;i<recordsnumber;i++)
       tsortresults[i]=findresults[0][i];
      while (findschedule.size()<MAXSEARCHDEPTH) {
       Show_Menu_Bar(1);
       Change_Color(4);
       gotoxy(1, 24);
       tfindschedule.field_id=Scan_Input(1, 1, 10000, 5);
       if (!tfindschedule.field_id)
        break;
       --tfindschedule.field_id;
       Show_Menu_Bar(1);
       Change_Color(4);
       gotoxy(1, 24);
       Scan_Input(0, 0, 1, 80);
       if (!strlen(input_string))
        break;
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
       Change_Color(4);
       gotoxy(1, 24);
       tfindschedule.field_id=Scan_Input(1, 1, 10000, 5);
       if (!tfindschedule.field_id)
        break;
       --tfindschedule.field_id;
       Show_Menu_Bar(1);
       Change_Color(4);
       gotoxy(1, 24);
       Scan_Input(0, 0, 1, 80);
       if (!strlen(input_string))
        break;
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
    break; } }
  
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
    if (record[records[(currentrecord*fieldsperrecord)+tfield.id].id].formula)
     strcpy(tfield.formula, tfield.text);
    if (autosave)
     Read_Write_Field(tfield, tfieldposition, 1);

 return 0;
}

// negate input char if outside of appropriate menu
int negatekeysforcurrentmenu(int t)
{
  int i;
  const char *menukeys[]={ "eot", "alsh", "dcpjv+-*/.!@", "ifru" }; // m works in all menus
  
  if (t==196) t='>'; // shift+right arrow
  if (t==187) t='<'; // shift+left arrow
  if (t==ESC || t==LEFT || t==RIGHT || t==UP || t==DOWN || t==TAB || t==SHIFT_TAB || t=='m' || t=='g' || t=='?' || t==HOME || t==END || t=='<' || t=='>' || t==START_OF_RECORDS || t==END_OF_RECORDS)
   return t;
  if (recordsdemo)
   return 0;
  if (t==6) { currentmenu=3; t='f'; return t; } // enter find mode
  if (t==5 || t==15 || t==20) { // direct menu access with ctrl
   switch (t) {
     case 5:currentmenu=2; break; break; case 15:currentmenu=1; break; case 20:currentmenu=3; 
   break; }
   Read_Write_Current_Parameters(1, 1);
  return 0; }
  if (currentmenu==2)
   if (t==INSERT || t==DELETE || t==SPACE || t==COPY || t==PASTE)
    return t;
  if (currentmenu==2 && t=='\n')
   return 'd';
  for (i=0;i<strlen(menukeys[currentmenu]);i++)
   if (t==menukeys[currentmenu][i])
    return t;
      
 return 0;
}

// menu bar
void Show_Menu_Bar(int mode) // 0 show, 1 remove
{  
  int i, recordsize=0;

  Change_Color(50);
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
     Change_Color(4);
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
  string helpinfo="                                <help screen>                                   reccurse is a custom design record keeper with advanced functions.              based on the ncurses library, reccurse is a clever tool for terminal use.       instruction keys are usually displayed in the bottom bar, the bottom bar itself can be switched from keyboard hints, to navigation information, to field text,  to disappearance with the <m> key. navigation through fields is done with arrow keys or tab/shift+tab and with home/end. move through records with shift+arrow  keys or < >. <g>o for record number. navigation keys work in all menus.         more keys -> ctrl+e to edit submenu, ctrl+o to options mode, ctrl+t to extra    submenu, ctrl+f find function, ctrl+k copy to clipboard, ctrl+v paste clipboard.+-*/. keys will add/remove attributes,!@ keys for color, ? for this help screen.<carriage return> enters edit mode, <space> ticks/unticks tickboxes (1x1fields).when find is selected, user is prompted to enter a field number, then a search  criteria (asterisks denote any text until the next alphanumeric character).     the sequence is repeated until a carriage return is given. this allows for      multiple searches, each sequential will operate on the previous find records    that match the requested criteria.                                              when sort is selected, user is prompted to enter a field number, then           <a>scending or <d>escending order. the sequence is repeated and records are     sorted each time according to the requested sort parameters.                    reccurse will record changes in fields after editing, if autosave is enabled in options submenu (default option). to prevent loss of data, keep a backup.       written in Aug-Sep 2019 by Giorgos Saridakis.                                              reccurse is distributed under the GNU Public Licence. :)";

   Clear_Screen();
   Change_Color(58);
   gotoxy(1,1);
   printw("%s", helpinfo.c_str());
   refresh();
   getch();
}

// show database information screen
void Show_DB_Information()
{
  char dbinfo[1000];
  int i, n, averagerecordsize, averagefieldsize, tfieldsize=0, numericalfields=0, mixedfields=0, stringfields=0, datestampfields=0, extrawindowlength;
  long int totalrecordssize=0;

   for (i=0;i<record.size();i++)
    switch (record[i].type) {
     case 0:
      ++numericalfields;
     break;
     case 1:
      ++datestampfields;
     break;
     case 2:
      ++stringfields;
     break;
     case 3:
      ++mixedfields;
    break; }
    
   for (i=0;i<recordsnumber;tfieldsize=0, i++) {
    for (n=0;n<fieldsperrecord;n++) {
     tfieldsize+=fieldlength(records[(i*fieldsperrecord)+n].text);
   totalrecordssize+=tfieldsize; } }
   averagefieldsize=totalrecordssize/(recordsnumber*fieldsperrecord);
   averagerecordsize=totalrecordssize/recordsnumber;
  
   extrawindowlength=(numberofdigits(totalrecordssize)<5) ? 0 : numberofdigits(totalrecordssize)-5;  
   Draw_Box(BOXCHAR, 6, 17, 32+extrawindowlength, 5, 15, 36);
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
   printw("autosave is:%s", onoff[autosave]);
   refresh();
   getch();   
}
  
// show field
int Show_Field(Annotated_Field *field, int flag) // 1 highlight
{
  Field *tfield=&record[field->id];
  int i, lima, limb, tposx, tposy, tcolor, columninprint, rowinprint;
  char ttext[MAXSTRING];
  int attributestable[9]; // normal, standout, underline, reverse, blink, dim, bold, protect, invisible

  if (!tfield->active)
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
  if (strcmp(tfield->title, ".")) {
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
   if (tposx>0 && tposx<80 && tposy>0 && tposy<24 && strcmp(tfield->title, ".")) {
   for (i=0;i<9;i++)
    attributestable[i]=ctoi(record[tfield->id].title_attributes[i]);
    for (i=0;i<9;i++) {
     if (attributestable[i]) {
      switch (i) {
       case 0:
        attron(A_NORMAL);
       break;
       case 1:
        attron(A_STANDOUT);
       break;
       case 2:
        attron(A_UNDERLINE);
       break;
       case 3:
        attron(A_REVERSE);
       break;
       case 4:
        attron(A_BLINK);
       break;
       case 5:
        attron(A_DIM);
       break;
       case 6:
        attron(A_BOLD);
       break;
       case 7:
        attron(A_PROTECT);
       break;
       case 8:
        attron(A_INVIS);
    break; } } }
    Change_Color(tfield->title_color);
    gotoxy(tposx, tposy);
    printw("%s", tfield->title);
    // remove attributes
    attroff(A_STANDOUT);
    attroff(A_UNDERLINE);
    attroff(A_REVERSE);
    attroff(A_BLINK);
    attroff(A_DIM);
    attroff(A_BOLD);
    attroff(A_PROTECT);
   attroff(A_INVIS); } }
 
   // field string to field size and lines
   Generate_Field_String(field, ttext); 
   if (field->number || !record[field->id].type)
    addleadingzeros(ttext, field);
   // add attributes
   for (i=0;i<9;i++)
    attributestable[i]=ctoi(record[tfield->id].attributes[i]);
   for (i=0;i<9;i++) {
    if (attributestable[i]) {
     switch (i) {
      case 0:
       attron(A_NORMAL);
      break;
      case 1:
       attron(A_STANDOUT);
      break;
      case 2:
       attron(A_UNDERLINE);
      break;
      case 3:
       attron(A_REVERSE);
      break;
      case 4:
       attron(A_BLINK);
      break;
      case 5:
       attron(A_DIM);
      break;
      case 6:
       attron(A_BOLD);
      break;
      case 7:
       attron(A_PROTECT);
      break;
      case 8:
       attron(A_INVIS);
   break; } } }
   tcolor=tfield->color;
   if (flag) {
    tcolor=(tfield->color==highlightcolors[0]) ? highlightcolors[1] : highlightcolors[0];
    for (i=strlen(ttext);i<80;i++)
   ttext[i]=SPACE; }
   Change_Color(tcolor); 
   columninprint=tfield->pt.x, rowinprint=tfield->pt.y;
   for (i=0;i<strlen(ttext);i++) {
    if (strlen(ttext)==1 && isspace(ttext[0]))
     break;
    gotoxy(columninprint, rowinprint);
    addch(ttext[i]);
    ++columninprint;
    if (columninprint>tfield->pt.x+tfield->size.x-1) {
     columninprint=tfield->pt.x;
    ++rowinprint; }
    if (rowinprint+1>tfield->pt.y+tfield->size.y)
   break; }
   // remove attributes
   attroff(A_STANDOUT);
   attroff(A_UNDERLINE);
   attroff(A_REVERSE);
   attroff(A_BLINK);
   attroff(A_DIM);
   attroff(A_BOLD);
   attroff(A_PROTECT);
   attroff(A_INVIS);
  
  refresh();
    
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
 int i;
 char formula[MAXSTRING];
     
  Field *tfield=&record[field->id];
  switch (tfield->type) {
   case 0:
    if (strcmp(tfield->automatic_value, "."))
     field->number=atof(tfield->automatic_value);
    if (tfield->formula) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, "."))
      strcpy(formula, tfield->automatic_value);
     i=parenthesesincluderforpolishreversecalculator(formula);
     if (!i) {
      reversepolishcalculatorequalizer(formula, currentrecord);
     field->number=reversepolishcalculator(formula); }
     else
    field->number=0; }
    strcpy(ttext, dtoa(field->number));
    limitsignificantnumbers(ttext, tfield->decimals);
    if (strcmp(tfield->suffix, "."))
     strcat(ttext, tfield->suffix);
   break;
   case 1:
    strcpy(ttext, field->text);
   break;
   case 2:
    strcpy(ttext, field->text);
    if (strcmp(tfield->automatic_value, "."))
     strcpy(ttext, tfield->automatic_value);
    if (tfield->formula) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, "."))
      strcpy(formula, tfield->automatic_value);
     i=stringformulacalculator(formula, currentrecord);
     if (i)
    strcpy(ttext, formula); }
   break; 
   case 3:
    strcpy(ttext, field->text);
    if (strcmp(tfield->automatic_value, "."))
     strcpy(ttext, tfield->automatic_value);
    if (tfield->formula) {
     if (strcmp(tfield->automatic_value, "."))
      strcpy(field->text, tfield->automatic_value);
     strcpy(formula, field->text);
     i=parenthesesincluderforpolishreversecalculator(formula);
     if (!i) {
      reversepolishcalculatorequalizer(formula, currentrecord);
     field->number=reversepolishcalculator(formula); }
     else
    field->number=0; }
    if (field->number) {
     strcpy(ttext, dtoa(field->number));
     limitsignificantnumbers(ttext, tfield->decimals);
     if (strcmp(tfield->suffix, "."))
    strcat(ttext, tfield->suffix); }
    if (tfield->formula && !field->number) {
     strcpy(formula, field->text);
     if (strcmp(tfield->automatic_value, "."))
      strcpy(formula, tfield->automatic_value);
     i=stringformulacalculator(formula, currentrecord);
     if (i)
    strcpy(ttext, formula); }
  break; }
  strcpy(field->text, ttext);
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
  int i1, i, fieldsreadperline=0, trecordsnumber=0, recordid=0;
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
   vector <Annotated_Field>::iterator p=trecords.end();
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
  int i, fieldshown=0, t, dummyfieldsperrecord, dummyrecordsnumber;
  char tstring[MAXSTRING];
  strcpy(tstring, dbfile);
  strcpy(dbfile, filename);
  Reccurse_File_Extension(dbfile, 2);
  if (!tryfile(dbfile)) {
   Show_Message(1, 24, 1, "external database file unreachable", 2000);
  return -1; }
  
  dummyrecord.clear();
  dummyrecords.clear();
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
  Change_Color(3);
  gotoxy(1, 24);
  printw("external .db file has %d fields per record and a total of %d records", dummyfieldsperrecord, dummyrecordsnumber);
  refresh();
  Sleep(2000);
  
   Change_Color(4);
   while (t!=ESC && t!='\n') {
    Show_Menu_Bar(1);
    Change_Color(4);
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
    chosendestinationfield=Scan_Input(1, 1, fieldsperrecord, 4);
    if (chosendestinationfield<1 || chosendestinationfield>fieldsperrecord)
     return -1;
    --chosendestinationfield;
    if (dummyrecordsnumber>recordsnumber) {
     Show_Menu_Bar(1);
     Show_Message(1, 24, 1, "external .db file has more records than current. append (y/n):", 0);
     t=tolower(sgetch());
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
