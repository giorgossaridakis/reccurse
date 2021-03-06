// reccuses headers

// included libraries
// C
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
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
#include <array>

// definitions
// numericals
const int MAXSTRING=80; // characters in a regular string
const int MAXTITLE=20; // characters in a a title string
const int MAXNUMBERDIGITS=18; // digits in a number
const int LMAXCOMMAND=9999; /* maximum operands etc to calculate for rcpc formula*/
const int MAXSUFFIXCHARS=3; // max string for number suffixes
const int DEFAULT_DECIMALS=2; // decimal positions
const int MAXFIELDS=999; // fields per record
const int MAXRECORDS=9999; // records limit
const int MAXPAGES=25; // pages limit
const int FIELDSIZE=MAXSTRING*2+MAXNUMBERDIGITS+15;  // +7 would do
const int MAXNAME=20; // max chars in database filenames
const int MAXMENUS=10; // mouse menus
const int RELATIONSHIPSLENGTH=1024; // 1kb of external db files relationship data
const int MAXRELATIONSHIPS=25; // will fit into above 1kb, same as MAXPAGES
const int MAXSEARCHDEPTH=5;
enum { HORIZONTALLY=0, VERTICALLY };
const int NUMERICALLIMIT=32765;
enum { NOBUTTON=0, TICKBOX, BUTTONBOX, BUTTONSCREEN, BUTTONCOMMAND };
enum { NUMERICAL=0, CALENDAR, STRING, MIXEDTYPE, VARIABLE, PROGRAM };
const char *menukeys[]={ "eot`", "alsh", "dcpjv+-*/.!@", "ifru", "yn", "0123456789/*-+^,.()=" }; // m works in all menus
const char *buttonkeys[]={ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "/", "*", 
"-", "+", "^", ",", ".", "(", ")", "=", "sin", "cos", "tan", "cotan", "sqr", "abs", "AC", "DEL", "EXEC" };
const char *programkeys="1234567890";
const char *EMPTYSTRING="~";
const int buttonkeystotal=29;
const char *alterscreenparameterskeys="/*-+.!@";
const char *menutexts[]={ "<tabshiftarrows|<>|homeend|<g>|<e>dit|<o>ptions|ex<t>ra|`|<m>enubar|<ESC>quit", "<a>utosave on/off|<l>oad database|<s>ave database|<h>elp page|<ESC>main menu", "e<d>it|<c>opy|<DEL>ete|<j>oin|di<v>ide|datestam<p>|<INS>more|<ESC>main menu", "database <i>nformation|<f>ind|so<r>t records|set<u>p database|<ESC>main menu", "really quit ?", "calculator mode: 0123456789/*-+^,.()= <enter> <backspace> <delete>" };  //main, options, edit, extra, quit, calculator

// keyboard
const int DOWN=258;
const int UP=259;
const int LEFT=260;
const int RIGHT=261;
const int SHIFT_LEFT=393;
const int SHIFT_RIGHT=402;
const int ESC=27;
const int SPACE=32;
const int ENTER=10;
const int BACKSPACE=263; 
const int DELETE=330;
const int INSERT=331;
const int HOME=262;
const int END=360;
const int COPY=11;
const int PASTE=22;
const int TAB=9;
const int SHIFT_TAB=353;
const int END_OF_RECORDS=336;
const int START_OF_RECORDS=337;
const int PAGES_SELECTOR_KEY=23;
const int SCRIPT_PLAYER=19;

using namespace std;

// global variables
int menubar;
int autosave;
int recordsdemo=0;
int currentpage=0;
int pagesnumber=0;
int currentmenu=0;
int currentfield;
int recordsnumber=0;
int currentrecord=0;
int startofrecords=0;
int fieldsperrecord=0;
int alteredparameters=0;
int dummyfieldsperrecord, dummyrecordsnumber;
int fieldhasdependancy=0;
int runscript=0;
vector<int> keyonnextloop;
int externalreferencedatabase;
vector<int> mousemenucommands;
MEVENT mouse;
char pages[MAXPAGES][MAXSTRING];
char rcfile[MAXSTRING], dbfile[MAXSTRING];
const char *onoff[]= { "off", "on" };
char clipboard[MAXSTRING];
int menucolors[6]={ 5, 6, 4, 3, 1, 2 };
int menulines[6]={ 24, 24, 24, 24, 24, 24 };
char infotext[MAXSTRING], *scriptcommand;

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
  int buttonbox; // 0 none, 1 tickbox, 2 button, 3 button screen
  // constructors, destructor
  Field(int i1, char s1[MAXSTRING], int i2, char s2[9], int i3, int i4, int i5, int i6, int i7, char s3[9], int i8, int i9, int i10, int i11, char s4[MAXSUFFIXCHARS], int i12, int i13, int i14, int i15, int i16, char s5[MAXSTRING]) { id=i1; strcpy(title,s1); title_position=i2; strcpy(title_attributes,s2); title_color=i3; pt.x=i4; pt.y=i5; size.x=i6; size.y=i7; strcpy(attributes, s3); color=i8; box=i9; box_color=i10; type=i11; strcpy(suffix, s4);  decimals=i12, formula=i13; fieldlist=i14; editable=i15; active=i16;  strcpy(automatic_value, s5); buttonbox=NOBUTTON; } ;
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
   Annotated_Field(int i1, double f1, char s1[MAXSTRING], const char s2[MAXSTRING]) { id=i1; number=f1; strcpy(text, s1); strcpy(formula, s2); } ;
   Annotated_Field() { } ;
~Annotated_Field() { } ; } ;

class Relationship {
 public:
  char extDbname[MAXNAME];
  // will be declared equal, local will reflect external if ext[0]=local[0]. if local field is fieldreference active, relationship will occur anyway
  int extFields[2]; 
  int localFields[2];
  Relationship(char name[MAXNAME], int e1, int e2, int l1, int l2) { strcpy(extDbname, name); extFields[0]=e1; extFields[1]=e2; localFields[0]=l1; localFields[1]=l2; } ;
  Relationship() { };
~Relationship() { } ; } ;

class MenuEntry {
 public:
  char entryText[MAXSTRING];
  vector<int> returnCommands;
  int menuDirectionId;
  MenuEntry(char *text, vector<int> commands, int directionid) { strcpy(entryText, text); returnCommands=commands; menuDirectionId=directionid; };
  MenuEntry(const char *text, vector<int> commands, int directionid) { strcpy(entryText, text); returnCommands=commands; menuDirectionId=directionid; };
  MenuEntry() { };
~MenuEntry() { }; };
 
class Menu {
 public:
  int menuId;
  int menuReference;
  int fatherMenuId;
  int fatherMenuPosition;
  char menuTitle[MAXNAME];
  vector<MenuEntry> menuEntries;
  Menu(int id, int reference, int fatherid, int fatherposition, char *title) { menuId=id; menuReference=reference; fatherMenuId=fatherid; fatherMenuPosition=fatherposition; strcpy(menuTitle, title); };
  Menu(int id, int reference, int fatherid, int fatherposition, const char *title) { menuId=id; menuReference=reference; fatherMenuId=fatherid;  fatherMenuPosition=fatherposition; strcpy(menuTitle, title); };
  Menu() { };
~Menu() { }; };

class Drawbox {
 public:
  int menuId;
  int drawcolor;
  int paintcolor;
  struct Points pt;
  struct Points size;
  Drawbox(int id, int color1, int color2, int ptx, int pty, int sizex, int sizey) { menuId=id; drawcolor=color1; paintcolor=color2; pt.x=ptx; pt.y=pty; size.x=sizex; size.y=sizey; };
  Drawbox() { };
~Drawbox() { }; };

struct FindSchedule {
   int field_id;
char texttolookfor[MAXSTRING]; } ;
 
vector<Menu> mousemenus;
vector<Drawbox> mousemenuboxes;
vector<Field> record, dummyrecord, externalrecord[MAXRELATIONSHIPS];
vector<Annotated_Field> records, dummyrecords, externalrecords[MAXRELATIONSHIPS];
vector<Relationship> relationships;
vector <FindSchedule> findschedule;

// function declarations

// reccurse.cc
void Intro_Screen();
int End_Program(int code=0);
void Reccurse_File_Extension(char *filename, int flag=0);
int Read_rc_File();
int Write_rc_File(char *file);
int Read_Write_db_File(int mode=0);
int Read_External_Database(int externaldatabaseid);
int Read_Write_Current_Parameters(int item, int mode=0); // 0 read, 1 write
void Read_Record_Field(ifstream &instream, Field &tfield);
void Read_Record_Field(istringstream &instream, Field &tfield);
void Write_Record_Field(ofstream &outstream, Field &tfield);
void Write_Record_Field(char *ttext, Field &tfield);
void Initialize_Record();
int Add_Field(int type=NUMERICAL, char *name=NULL, char *textvalue=NULL);
void Delete_Field(int field_id);
int Join_Fields(int field_id, int mode);
void Renumber_Field_References(int startingfield);
void Renumber_Field_Relationships(int startingfield);
int Divide_Field(int field_id, int mode);
void Bring_DateTime_Stamp(char tdatetime[MAXSTRING]);
int Read_Write_Field(Annotated_Field &tfield, long int field_position, int mode=0);
long int fieldposition(int record_id, int field_id);
long int fieldserialtofieldposition(int position);
// void fieldserialtoparameters(int position, int &record_id, int &field_id);
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
int Generate_Dependant_Field_String(Annotated_Field *field, char *ttext);
int Export_Database(char *filename);
int Import_Database(char *filename);
int Import_External_db_File(char *filename);
int Read_Write_Relationships(int mode=0);
void Initialize_Database_Parameters();
void Load_Database(int pagenumber);
int Pages_Selector(int pagetochange=-1);
int Clean_Database(char *filename);
void Set_Mouse_Menus();
int fetchmousemenucommand();
int Determine_Button_Box(int field_id);
char *fetchcommand(char *text);
void Delete_Field_Entry(int field_id);
void pushspaceonfield(int field_id=-1);
void copytoclipboard();
void pastefromclipboard();
void toggleautosave();
int addorplayprogram(int programid);

// rccompar.cc
int labelposition(char *label);
int islinelabel(char *scriptcommand);
int setvariablefromfield(char *parameter, int field_id);
void stopscript();
int isvariable(char *parameter);

// rclib.cc
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
int Scan_Input(int flag=0, int lim_a=0, int lim_b=1, int length=0);
char Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color=58);
void Scan_Date(int x_pos, int y_pos, char tdate[]);
char *addmissingzeros(char tstring[], int zeros);
void terminatestringatcharactersend(char *ttext);
void addleadingzeros(char ttext[], Annotated_Field *tfield, int flag=0);
void addleadingspaces(char ttext[], int overallsize);
void cropstring(char ttext[], int noofcharacters, int flag=0);
int fieldlength(char *fieldtext);
long int filesize(char *filename);
void stringquotesencloser(char *tstring, int flag=0);
void stringquotesopener(char *tstring);
int isinquotes(char tstring[]);
void limitspaces(char *tstring);
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
int iscalculationsign(char t);
int isdecimalseparator(char t);
int isparenthesis(char t);
int isprintablecharacter(int t);
int iscorruptstring(char *tstring);
int limitsignificantnumbers(char *s, int digits);
int find(char text[], char token[]);
int findsimple(char text[], char token[]);
int sortrecords(int field_id, int recordssequence[], int mode=0);
void sortfieldsbyxpt(int field_id, vector <int> &fieldxidentities);
void sortfieldsbyypt(int field_id, vector <int> &fieldyidentities);
int locatefieldbymouseclick();
bool rightmousebuttonclicked();
int findfieldege(int flag=0);
int CalcDayNumFromDate(int y, int m, int d);
int isleapyear(int year);
int daysinmonth(int year, int month);
void INThandler(int sig);
char* bringstringtomiddle(char *text, int width);
int scantextforcommand(char *text, char *command, char separator='@');
int kbhit(void);
int decimatestringtokey(char *text);
int breaktexttokeys(char *text);

// rcfre.cc
int References_Editor();
void Field_Editor();

// rcmenusel.cc
int Menu_Selector();
int Draw_Mouse_Box(int menuid);

// rcpc.cc
int parenthesesincluderforpolishreversecalculator(char formula[]);
int reversepolishcalculatorequalizer(char formula[], int record_id=-1);
int isformulainpolishcalculatorsyntax(char formula[]);
double reversepolishcalculator(char formula[]);
int getop(char s[], char formula[]);
void push(double f);
double pop(void);

// rcpclib.cc
void initiatemathematicalfunctions();
int parseformulaforfunctions(char formula[]);
int mathfunctionsparser(int function_id, char tcommand[MAXSTRING]);
int parseformulaforerrors(char formula[]);

// rcsc.cc
int stringformulacalculator(char formula[MAXSTRING], int record_id);
void replacepartoftextwithcorrespondingvalue(char ttext[MAXSTRING], int record_id);
void extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt);
void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point);
int commandparser(int reference, char tcommand[MAXSTRING]);

// rcscr.cc
int Init_Screen();
void End_Screen();
void Change_Color(int choice);
void Draw_Box(int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=0);
void Draw_Box(char t, int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=0);
void Draw_Box(Drawbox &tdrawbox);
void gotoxy(int x, int y);

// to be compiled together
#include "rcscr.cc"
#include "rclib.cc"
#include "rcsc.cc"
#include "rcpc.cc"
#include "rcpclib.cc"
#include "rcfre.cc"
#include "rcmenusel.cc"
#include "rccompar.cc"

