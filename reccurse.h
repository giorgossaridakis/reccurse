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
#include <algorithm>

using namespace std;
extern WINDOW *win1;

// definitions
typedef unsigned long ul;
// numericals
const int MAXSTRING=80; // characters in a regular string
const int MAXTITLE=20; // characters in a a title string
const int MAXWORDS=512; // for buttton bar menus
const int MAXNUMBERDIGITS=18; // digits in a number
const int LMAXCOMMAND=9999; /* maximum operands etc to calculate for rcpc formula*/
const int MAXSUFFIXCHARS=3; // max string for number suffixes
const int DEFAULT_DECIMALS=2; // decimal positions
const int MAXFIELDS=999; // fields per record
const int MAXRECORDS=9999; // records limit
const int MAXPAGES=25; // pages limit
const int FIELDSIZE=MAXSTRING*2+MAXNUMBERDIGITS+15;  // +7 would do
const int MAXNAME=80; // max chars in database filenames
const int MAXMENUS=10; // mouse menus
const int RELATIONSHIPSLENGTH=1024; // 1kb of external db files relationship data
const int MAXRELATIONSHIPS=25; // will fit into above 1kb, same as MAXPAGES
const int MAXSEARCHDEPTH=5;
const int INSTRUCTION='%';
const int COMMAND='@';
const int nrecordparameters=20;
enum { HORIZONTALLY=1, VERTICALLY };
const int NUMERICALLIMIT=32765;
int BLOCK=-1, UNBLOCK=1000, PRINTUNBLOCK=25, QUITUNBLOCK=2500, SCANUNBLOCK=10000; // wait times for getch()
int separatort=46, separatord=44, suffixposition=1;
enum { NOBUTTON=0, TICKBOX, BUTTONBOX, BUTTONSCREEN, BUTTONCOMMAND, AUTOMATICSCRIPT };
enum { NUMERICAL=0, CALENDAR, STRING, MIXEDTYPE, VARIABLE, PROGRAM, CLOCK };
enum { NORMAL=0, STANDOUT, UNDERLINE, REVERSE, BLINK, DIM, BOLD, PROTECT, INVISIBLE };
extern int BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE;
enum { BLACKONBLACK=1, BLACKONRED, BLACKONGREEN, BLACKONYELLOW, BLACKONBLUE, BLACKONMAGENTA,
       BLACKONCYAN, BLACKONWHITE,
       REDONBLACK, REDONRED, REDONGREEN, REDONYELLOW, REDONBLUE, REDONMAGENTA, REDONCYAN, REDONWHITE,
       GREENONBLACK, GREENONRED, GREENONGREEN, GREENONYELLOW, GREENONBLUE, GREENONMAGENTA, GREENONCYAN, GREENONWHITE,
       YELLOWONBLACK, YELLOWONRED, YELLOWONGREEN, YELLOWONYELLOW, YELLOWONBLUE, YELLOWONMAGENTA, YELLOWONCYAN, YELLOWONWHITE,
       BLUEONBLACK, BLUEONRED, BLUEONGREEN, BLUEONYELLOW, BLUEONBLUE, BLUEONMAGENTA, BLUEONCYAN, BLUEONWHITE,
       MAGENTAONBLACK, MAGENTAONRED, MAGENTAONGREEN, MAGENTAONYELLOW, MAGENTAONBLUE, MAGENTAONMAGENTA, MAGENTAONCYAN, MAGENTAONWHITE,
       CYANONBLACK, CYANONRED, CYANONGREEN, CYANONYELLOW, CYANONBLUE, CYANONMAGENTA, CYANONCYAN, CYANONWHITE,
       WHITEONBLACK, WHITEONRED, WHITEONGREEN, WHITEONYELLOW, WHITEONBLUE, WHITEONMAGENTA, WHITEONCYAN, WHITEONWHITE
      };
enum { TOLEFT=1, CENTER, TORIGHT };
enum { NOSCRIPT = 0, ONENTRY, ONEXIT, ONENTRYANDEXIT };
enum { SEGMENTATIONFAULT = -4, FLOATINGPOINTEXCEPTION = -3, NOACTIVEFIELDS = -2, FILEERROR, NORMALEXIT = 0, BREAKEXIT };
enum { X = 0, Y, XY };
enum { EQUAL = 0, LESSER, LESSEROREQUAL, GREATER, GREATEROREQUAL };
const char *exittexts[]= { "SEGMENTATION FAULT", "FLOATING POINT EXCEPTION", "NO ACTIVE FIELDS", "FILE I/O ERROR", "NORMAL", "BREAK" };
const char *menukeys[]={ "eot`", "alsh`", "dckpjv+-*/.!@`", "ifru`", "yn`", "0123456789/*-+^,.()=`", "udixl`", "ir`" }; // m works in all menus
const char *buttonkeys[]={ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "/", "*", 
"-", "+", "^", ",", ".", "(", ")", "=", "sin", "cos", "tan", "cotan", "sqr", "abs", "log", "AC", "DEL", "EXEC" };
const char *programkeys="1234567890";
const char *EMPTYSTRING="~";
const int buttonkeystotal=30;
const char *alterscreenparameterskeys="/*-+.!@";
const char *menutexts[]={ "<tabshiftarrows|<>|homeend|<g>|<e>dit|<o>ptions|ex<t>ra|`|<m>enubar|<ESC>quit", "<a>utosave on/off|<l>oad database|<s>ave database|<h>elp page|`|<ESC>main menu", "e<d>it|<c>opy|<DEL>ete|<j>oin|di<v>ide|datestam<p>|<INS>more|`|<ESC>main menu", "database <i>nformation|<f>ind|so<r>t records|set<u>p database|`|<ESC>main menu", "<arrows><shift+arrows><1.2>title color<3.4>color<5.6>box color<b>ox<t>itle<*ESC>", "calculator: 0123456789/*-+^,.()= <enter> <backspace> <delete> <`>previous menu", "d<u>plicate, <d>elete record, <i>mport/e<x>port records, externa<l> .dbfiles", "<i>mport records, external <r>eferences editor" };  //main, options, edit, extra, editor, calculator, extra from edit, external db records;
const char *menubaroptions =  "0,27,31,g,0,31,38,e,0,38,48,o,0,48,56,t,0,56,58,`,0,58,68,m,0,68,78,27,1,0,18,a,1,18,34,l,1,34,50,s,1,50,62,h,1,62,64,`,1,64,79,27,2,0,7,d,2,7,14,c,2,14,23,330,2,23,30,j,2,30,39,v,2,39,51,p,2,51,61,331,2,61,63,`,2,63,78,27,3,0,23,i,3,23,30,f,3,30,45,r,3,45,62,u,3,62,64,`,3,64,79,27, 4,23,25,49, 4,25,27,50, 4,39,41,51, 4,41,43,52, 4,49,51,53,4,51,53,54,4,62,68,b, 4,67,75,t, 4,75,77,*,4,77,81,27,5,12,13,48,5,13,14,49,5,14,15,50,5,15,16,51,5,16,17,52,5,17,18,53,5,18,19,54,5,19,20,55,5,20,21,56,5,21,22,57,5,22,23,/,5,23,24,*,5,24,25,-,5,25,26,+,5,26,27,^,5,27,28,.,5,28,29,.,5,29,30,(,5,30,31,),5,31,32,=,5,33,40,10,5,41,52,263,5,53,62,330,5,62,79,`,6,0,12,u,6,13,29,d,6,30,39,i,6,39,48,x,6,57,77,l,7,0,17,i,7,18,47,r";
enum { MAIN=0, OPTIONS, EDIT, EXTRA, EDITOR, CALCULATOR, EDITEXTRA, EXTERNALDB };

// keyboard
const int DOWN=258;
const int UP=259;
const int LEFT=260;
const int RIGHT=261;
const int SHIFT_LEFT=393;
const int SHIFT_RIGHT=402;
const int SHIFT_UP=337;
const int SHIFT_DOWN=336;
const int ESC=27;
const int SPACE=32;
const int ENTER=10;
const int BACKSPACE=263; 
const int DELETE=330;
const int INSERT=331;
const int HOME=262;
const int END=360;
const int COPY=11;
const int TOGGLEMOUSE=12;
const int SCANAUTOMATICVALUE=16;
const int SCANTITLE=14;
const int IMPORTASCII=25;
const int TEXTTOAUTOMATICVALUE=21;
const int UNDO=18;
const int PASTE=22;
const int TAB=9;
const int SHIFT_TAB=353;
const int END_OF_RECORDS=336;
const int START_OF_RECORDS=337;
const int PAGES_SELECTOR_KEY=23;
const int SCRIPT_PLAYER=19;
const int QUIT=24;

// global variables
int menubar;
int autosave;
int recordsdemo=0;
int printscreenmode=0;
int renewscreen=1;
int recordsdemoall=0;
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
enum { OFF = 0, ON };
const char *onoff[]= { "off", "on" };
char clipboard[MAXSTRING];
char calendarformat[MAXSTRING];
int menucolors[8]={ MAGENTA, CYAN, BLUE, YELLOW, MAGENTAONCYAN, GREEN, BLUE, BLUE };
int menulines[8]={ 24, 24, 24, 24, 24, 24, 24, 24 };
char infotext[MAXSTRING], *scriptcommand;
int screen[81][25];
FILE *out;
int fieldrepetitions[MAXFIELDS], lastfieldrepeated;
int changedrecord=1, editoroption=-1;
char input_string[MAXSTRING];
int altpressed;
int MOUSE=ON;
int savedfield=-1;
extern int scriptsleeptime;
extern int terminalhascolor;

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
  int type; // 0 number, 1 date&time, 2 string, 3 mixed type etc
  int decimals;
  char suffix[MAXSUFFIXCHARS];
  int formula;
  int fieldlist;
  int editable;
  int active;
  char automatic_value[MAXSTRING];
  int buttonbox; // 0 none, 1 tickbox, 2 button, 3 button screen, 4 command, automatic script
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

class ButtonBarMenuEntry {
 public:
  int choiceMenuId;
  int choiceStartx;
  int choiceEndx;
  int choiceKey;
  ButtonBarMenuEntry(int menuid, int startx, int endx, int key): choiceMenuId(menuid), choiceStartx(startx), choiceEndx(endx), choiceKey(key) { };
  ButtonBarMenuEntry() { };
~ButtonBarMenuEntry() { };
};

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

enum { STARTOFWORD = 0, ENDOFWORD };
class Word { 
  public:
   char formulaWord[MAXSTRING]; 
   int formulaPosition[2];
   Word(char *word, int i1, int i2): formulaPosition{i1,i2} { strcpy(formulaWord, word); };
   Word() { };
~Word() { }; };

// vectors
vector<ButtonBarMenuEntry> buttonbarmenus;
vector<Field> record, dummyrecord, externalrecord[MAXRELATIONSHIPS];
vector<Annotated_Field> records, dummyrecords, externalrecords[MAXRELATIONSHIPS], copiedrecords;
vector<Relationship> relationships;
vector <FindSchedule> findschedule;
vector<Word> separatedwords;
vector<int> multiplechoicefields, adjoiningfields;

// external variables
// rcscr.cc
const int UNDERSCORE=95;
const char BOXCHAR='*';
extern int highlightcolors[4];

// function declarations

// reccurse.cc
void Intro_Screen();
int End_Program(int code=NORMALEXIT);
int checkalteredparameters();
char* Reccurse_File_Extension(char *filename, int flag=0);
int Read_rc_File();
int Write_rc_File(char *file);
int Read_Write_db_File(int mode=0);
int Read_External_Database(int externaldatabaseid);
int Read_Write_Current_Parameters(int item, int mode=0); // 0 read, 1 write
int Read_Record_Field(ifstream &instream, Field &tfield);
int isrecordproperlydictated(Field &tfield);
void Read_Record_Field(istringstream &instream, Field &tfield);
void Write_Record_Field(ofstream &outstream, Field &tfield);
void Write_Record_Field(char *ttext, Field &tfield);
void Initialize_Record();
int Add_Field(int type=NUMERICAL, char *name=NULL, char *textvalue=NULL);
void Delete_Field(int field_id);
int Join_Fields(int field_id, int mode);
void Renumber_Field_References(int startingfield);
void Renumber_Field_Relationships(int startingfield);
void Renumber_Field_Fieldlist(int startingfield);
int Divide_Field(int field_id, int mode);
void Bring_DateTime_Stamp(char tdatetime[MAXSTRING], int field_id);
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
void Flash_Field(int field_id, int sleeptime=250);
int Show_Field_ID(Annotated_Field *tfield);
void Generate_Field_String(Annotated_Field *tfield, char *ttext);
int Generate_Dependant_Field_String(Annotated_Field *field, char *ttext);
int Export_Database(char *filename);
int Import_Database(char *filename);
int Import_External_db_File(char *filename);
int Read_Write_Relationships(int mode=0);
void Initialize_Database_Parameters(int mode=0);
void Load_Database(int pagenumber);
int Pages_Selector(int pagetochange=-1);
void Set_Mouse_Menus();
int Activate_Menubar_Choice(int x);
int fetchmousemenucommand();
int Determine_Button_Box(int field_id);
int Determine_Script_Direction(int field_id);
void Execute_Script_Command(int field_id);
char *fetchcommand(char *text, int startpt=0);
char *restructurecommand(char *command);
void Delete_Field_Entry(int field_id);
void pushspaceonfield(int field_id=-1);
void copytoclipboard();
void pastefromclipboard();
void toggleautosave();
int addorplayprogram(int programid);
void outputscreenarraytofile();

// rclib.cc
void INThandler(int sig);
int filecontainsbinary(ifstream* file);
char charcoder(char d, int mode=0);
int filecodedecode(char *source, char *destination, int mode=0);
int separatewords(char formula[], vector<Word> &separatedwords, char *separator);
Word* strtok2(char *formula, char *separator);
void Show_File_Error(char *filename);
void stringcodedecode(char *source, char *destination, int mode=0);
void checkpoint(int id, int color=58);
int Scan_Input(int flag=0, int lim_a=0, int lim_b=1, int length=79);
enum { MIDDLE = -1, FIRST, LAST };
int Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color=WHITE, int length=79, int cursor=-1, int firstlast=-1);
void Scan_Date(int x_pos, int y_pos, char tdate[], int flag=0);
char *addmissingzeros(char tstring[], int zeros);
void terminatestringatcharactersend(char *ttext);
void addleadingzeros(char ttext[], Annotated_Field *tfield, int flag=0);
void addleadingspaces(char ttext[], int overallsize);
int fieldlength(char *fieldtext);
void stringquotesencloser(char *tstring, int flag=0);
void stringquotesopener(char *tstring);
int isinquotes(char tstring[]);
int sgetch(int x_pos=78, int y_pos=24, int sleeptime=250, int showflag=1);
int bgetch(int delay=BLOCK);
void cleanstdin();
void Show_Message(int x_pos, int y_pos, int color, char *message, int sleeptime=1500);
void Show_Message(int x_pos, int y_pos, int color, const char *message, int sleeptime=1500);
void replaceunderscoresandbrackets(char dataname[], int flag);
int limitsignificantnumbers(char *s, int digits);
int sortrecords(int field_id, int recordssequence[], int mode=0);
void sortfieldsbyxpt(int field_id, vector <int> &fieldxidentities);
void sortfieldsbyypt(int field_id, vector <int> &fieldyidentities);
int locatefieldbymouseclick();
bool rightmousebuttonclicked();
bool leftmousebuttondoubleclicked();
bool middlemousebuttonpressed();
int wheelmousemove();
int findfieldege(int flag=0);
void INThandler(int sig);
char* bringstringtomiddle(char *text, int width);
int scantextforcommand(char *text, char *command, char separator='@');
int kbhit(void);
int decimatestringtokey(char *text);
int breaktexttokeys(char *text);
int blockunblockgetch(int delay=BLOCK);
int isautomaticvalueformatinstruction(int field_id);
int isfieldscriptdirector(int field_id);
char *formatmonetarystring(char *text);
char *appendsuffix(char *text, int field_id);
void aligntextsingley(Annotated_Field *field, int alignment, int row=0);
char* aligntext(char text[MAXSTRING], Annotated_Field *field, int alignment);
int pagehasclockfields();
int isfieldtextlink(Annotated_Field *field, int linkparameters[]);
int isfieldmultipleselection(int field_id);
int multiplechoiceinstructions(int field_id);
int selectmultiplechoicefield(int field_id);
void moveinstructioninfieldtext(int field_id);
int assignstringvaluestoarray(char *line, char array[MAXWORDS][MAXSTRING], int entries);
int readstringentry(char *line, char *linepart);
unsigned int isseparationchar(char t);
int fieldsadjoiningfields(Annotated_Field *tfield, vector<int>& adjoiningfields, int possibilityoption=0, int sizex=0, int sizey=0, int comparisonx=EQUAL, int comparisony=EQUAL);
int referencedadjoiningfields(int field_id, vector<int>& adjoiningfields, int fieldidflag=0, int rebuildflag=0, int sizex=0, int sizey=0, int comparisonx=EQUAL, int comparisony=EQUAL);
void sortxy(vector<int>& fieldstosort, int preference=XY);
int arefieldsneighbours(int id1, int id2, int possibilityoption=0, int sizex=0, int sizey=0, int comparisonx=EQUAL, int comparisony=EQUAL);
int compareintegers(int i1, int i2, int comparison=EQUAL);
int findinintvector(int element, vector<int>& tv);
char *Generate_Calendar(int m, int y);
void leavespaces(char *calendar, int spaces);
int isfielddisplayable(int field_id);
void Sleep(ul sleepMs) { blockunblockgetch(sleepMs); getch(); blockunblockgetch(); };
void uSleep(ul sleepMs) { usleep(sleepMs*1000); };
int loadasciitofields(int field_id, char *filename);
vector<Annotated_Field>& Records_From_Adjoining_Fields(int field_id, int possibilityoption=0);
vector<Annotated_Field>& Records_From_Current_Record();
void CurrentRecord_From_Vector(vector<Annotated_Field>& tv);
void Write_Fields_Int_Vector(vector<int> tv, int recordid=-1);
void Write_Fields_AnnotatedField_Vector(vector<Annotated_Field> tv, int recordid=-1);
int isfieldreferencedinvector(int field_id, vector<int>& tv);
int togglemouse(int showflag=ON);

// rcutil.cc
extern int mod(double a, double b);
extern int ctoi(char c);
extern char *dtoa(long double val);
extern char *itoa(long int val, int base=10);
extern int tryfile(char *file);
extern int Copy_File(char *source, char *dest);
extern void cropstring(char ttext[], int noofcharacters, int flag=0);
extern long int filesize(char *filename);
extern int numberofdigits(long int n);
extern char *ctos(int t);
extern double bringfractionpartofdivision(int param1, int param2, int scale=10);
extern int isspace(char t);
extern int iscalculationsign(char t);
extern int isdecimalseparator(char t);
extern int isparenthesis(char t);
extern int isprintablecharacter(int t);
extern int find(char text[], char token[]);
extern int findsimple(char text[], char token[]);
extern int CalcDayNumFromDate(int y, int m, int d);
extern int isleapyear(int year);
extern int daysinmonth(int year, int month);
extern int limitspaces(char *tstring);
extern int readstringentry(char *line, char *linepart);
extern unsigned int isseparationchar(char t);

// rcfre.cc
extern int References_Editor();
extern void Field_Editor();
extern void clearinputline();
extern int Edit_Field(int &field_id);
extern void Show_All_Fields_for_Editor(int field_id, int flag=0);
extern void Duplicate_Field(int field_id, int flag=0);

// rcpc.cc
extern int parenthesesincluderforpolishreversecalculator(char formula[]);
extern int reversepolishcalculatorequalizer(char formula[], int record_id=-1);
extern int isformulainpolishcalculatorsyntax(char formula[]);
extern double reversepolishcalculator(char formula[]);
extern int getop(char s[], char formula[]);
extern void push(double f);
extern double pop(void);

// rcpclib.cc
extern void initiatemathematicalfunctions();
extern int parseformulaforfunctions(char formula[]);
extern int mathfunctionsparser(int function_id, char tcommand[MAXSTRING]);
extern int parseformulaforerrors(char formula[]);

// rcsc.cc
extern int stringformulacalculator(char formula[MAXSTRING], int record_id);
extern int replacepartoftextwithcorrespondingvalue(char ttext[MAXSTRING], int record_id);
extern int extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt);
extern void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point);
extern int commandparser(int reference, char tcommand[MAXSTRING]);
extern char* formatreplacer(char source[MAXSTRING], int field_id);
extern char *performinstruction(char instruction[MAXSTRING], int field_id);

// rccompar.cc
extern int commandparser(char scriptcommand[MAXSTRING]);
extern int labelposition(char *label);
extern int islinelabel(char *scriptcommand);
extern int setvariablefromfield(char *parameter, int field_id);
extern void stopscript();
extern int isvariable(char *parameter);

// rcscr.cc
extern int Init_Screen();
extern void End_Screen();
extern void Change_Color(int choice=WHITE);
extern void Draw_Box(int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=BLACK);
extern void Draw_Box(char t, int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=BLACK);
extern void Draw_Box(Drawbox &tdrawbox);
extern void gotoxy(int x, int y);
extern void Change_Attributes(int attribute);

// to be compiled together
#include "rclib.cc"

