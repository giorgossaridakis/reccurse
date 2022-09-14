// reccurse field&references editor

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
extern WINDOW* win1;

// constants
const int MAXSTRING=80; // characters in a regular string
const int MAXTITLE=20; // characters in a a title string
const int MAXSUFFIXCHARS=3; // max string for number suffixes
const int MAXNAME=80; // max chars in database filenames
const int MAXRELATIONSHIPS=25;
const char BOXCHAR='*';
extern const char *onoff[];
extern int fieldsperrecord;
extern int changedrecord, editoroption;
extern int currentrecord;
extern int currentfield;
extern int recordsnumber;
extern int alteredparameters;
extern int currentmenu;
extern int menubar;

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
const int ENTER=10;
const int SPACE=32;
const int DELETE=330;
const int INSERT=331;
const int HOME=262;
const int END=360;
const int MAXFIELDS=999; // fields per record
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
enum { NOBUTTON=0, TICKBOX, BUTTONBOX, BUTTONSCREEN, BUTTONCOMMAND, AUTOMATICSCRIPT };
enum { NUMERICAL=0, CALENDAR, STRING, MIXEDTYPE, VARIABLE, PROGRAM, CLOCK };
enum { NORMAL=0, STANDOUT, UNDERLINE, REVERSE, BLINK, DIM, BOLD, PROTECT, INVISIBLE };
enum { TOLEFT=1, CENTER, TORIGHT };
const char *FIELDTYPES[]= { "numerical", "calendar", "string", "mixed", "variable", "program", "clock" };
const char *BUTTONBOXES[]= { "edit field", "tick box", "button box", "button screen", "button command", "automatic script" };
enum { OFF = 0, ON };
enum { MAIN=0, OPTIONS, EDIT, EXTRA, EDITOR, CALCULATOR, EDITEXTRA, EXTERNALDB };

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

Field backuprecord;

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

// vectors
extern vector<Field> record, dummyrecord, externalrecord[MAXRELATIONSHIPS];
extern vector<Annotated_Field> records, dummyrecords, externalrecords[MAXRELATIONSHIPS];
extern vector<Relationship> relationships;
extern MEVENT mouse;

// function declarations
int References_Editor();
void Field_Editor();
void clearinputline();
int Edit_Field(int &field_id);
void Duplicate_Field(int field_id, int flag=0); // 0 only record, 1 copy records, 2 both
void Show_All_Fields_for_Editor(int field_id, int flag=0);
extern void Change_Color(int choice=WHITE);
extern void Draw_Box(int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=BLACK);
extern void Draw_Box(char t, int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=0);
extern void Draw_Box(Drawbox &tdrawbox);
extern void gotoxy(int x, int y);
extern void Change_Attributes(int attribute);
extern int sgetch(int x_pos=78, int y_pos=24, int sleeptime=250, int showflag=1);
extern int Show_Field(Annotated_Field *tfield, int flag=0);
extern int Show_Field_ID(Annotated_Field *tfield);
extern int isrecordproperlydictated(Field &tfield);
extern void Show_Menu_Bar(int mode=0);
extern int Activate_Menubar_Choice(int x);
extern int Determine_Button_Box(int field_id);
extern bool leftmousebuttondoubleclicked();
extern bool rightmousebuttonclicked();
extern int isfielddisplayable(int field_id);
extern void cleanstdin();
extern void Show_Message(int x_pos, int y_pos, int color, char *message, int sleeptime=1500);
void Show_Message(int x_pos, int y_pos, int color, const char *message, int sleeptime=1500);
extern int Scan_Input(int flag=0, int lim_a=0, int lim_b=1, int length=79);
extern char Scan_Input(char istring[MAXSTRING], int x_pos, int y_pos, int color=WHITE, int length=79, int cursor=-1, int firstlast=-1);
extern void Scan_Date(int x_pos, int y_pos, char tdate[], int flag=0);
extern int Add_Field(int type=NUMERICAL, char *name=NULL, char *textvalue=NULL);
extern void Delete_Field(int field_id);
extern int Join_Fields(int field_id, int mode);
extern void Renumber_Field_References(int startingfield);
extern void Renumber_Field_Relationships(int startingfield);
extern int Divide_Field(int field_id, int mode);
extern char *itoa(long int val, int base=10);
extern int Read_Write_db_File(int mode=0);
extern char* Reccurse_File_Extension(char *filename, int flag=0);
extern int tryfile(char *file);
extern void cropstring(char ttext[], int noofcharacters, int flag=0);

// field editor and setup routine
void Field_Editor()
{
  int i, selection, fieldshown=currentfield, previousrecord=-1, t=0, x, y;
  char ttext[MAXSTRING];
  editoroption=alteredparameters=1;; // reserved to point to fieldshown, if needed
  mousemask(ALL_MOUSE_EVENTS, NULL);
  
  while ( t != ESC ) {
   
   if ( previousrecord != fieldshown || previousrecord == -1 ) {
    backuprecord=record[fieldshown];
    previousrecord=fieldshown;
   }
      
   Determine_Button_Box(fieldshown);
   Draw_Box(BOXCHAR, 6, 17, 33, 5, 17, 25);
   Show_Menu_Bar(1);
   clearinputline();
   Change_Color(BLUE);
   gotoxy(25, 6);
   printw("Reccurse Database");
   Change_Color(YELLOW);
   for (i=8;i<18;i++) {
    gotoxy(34, i);
   printw("|"); }
   attron(A_BOLD);
   sprintf(ttext, "field %d/%d", fieldshown+1, fieldsperrecord);
   gotoxy(18 + (32 - (int) strlen(ttext))/2, 7);
   printw("%s", ttext);
   attroff(A_BOLD);
   Change_Color(YELLOW);
   gotoxy(18, 8);
   printw("1.title:");
   Change_Color(RED);
   printw("%.8s", record[fieldshown].title);
   Change_Color(YELLOW);
   gotoxy(18, 9);
   printw("2.title pos:");
   Change_Color(RED);
   printw("%d", record[fieldshown].title_position);
   Change_Color(YELLOW);
   gotoxy(18, 10);
   printw("3.title attrs:");
   Change_Color(RED);
   printw("*");
   Change_Color(YELLOW);
   gotoxy(18, 11);
   printw("4.title color:");
   Change_Color(RED);
   printw("%2d", record[fieldshown].title_color);
   Change_Color(YELLOW);
   gotoxy(18,12);
   printw("5.pos.x:");
   Change_Color(RED);
   printw("%d", record[fieldshown].pt.x);
   Change_Color(YELLOW);
   gotoxy(18,13);
   printw("6.pos.y:");
   Change_Color(RED);
   printw("%d", record[fieldshown].pt.y);
   Change_Color(YELLOW);
   gotoxy(18,14);
   printw("7.size.x:");
   Change_Color(RED);
   printw("%d", record[fieldshown].size.x);
   Change_Color(YELLOW);
   gotoxy(18,15);
   printw("8.size.y:");
   Change_Color(RED);
   printw("%d", record[fieldshown].size.y);
   Change_Color(YELLOW);
   gotoxy(18,16);
   printw("9.attributes:");
   Change_Color(RED);
   printw("*");
   Change_Color(YELLOW);
   gotoxy(18,17);
   printw("10.color:");
   Change_Color(RED);
   printw("%2d", record[fieldshown].color);
   Change_Color(YELLOW);
   gotoxy(35,8);
   printw("11.box:");
   Change_Color(RED);
   printw("%s", onoff[record[fieldshown].box]);
   Change_Color(YELLOW);
   gotoxy(35,9);
   printw("12.box color:");
   Change_Color(RED);
   printw("%2d", record[fieldshown].box_color);
   Change_Color(YELLOW);
   gotoxy(35,10);
   printw("13.field type:");
   Change_Color(RED);
   printw("%d", record[fieldshown].type);
   Change_Color(YELLOW);
   gotoxy(35,11);
   printw("14.decimals:");
   Change_Color(RED);
   printw("%d", record[fieldshown].decimals);
   Change_Color(YELLOW);
   gotoxy(35,12);
   printw("15.suffix:");
   Change_Color(RED);
   printw("*");
   Change_Color(YELLOW);
   gotoxy(35,13);
   printw("16.formula:");
   Change_Color(RED);
   printw("%s", onoff[record[fieldshown].formula]);
   Change_Color(YELLOW);
   gotoxy(35,14);
   printw("17.list:");
   Change_Color(RED);
   printw("%d", record[fieldshown].fieldlist);
   Change_Color(YELLOW);
   gotoxy(35,15);
   printw("18.editable:");
   Change_Color(RED);
   printw("%s", onoff[record[fieldshown].editable]);
   Change_Color(YELLOW);
   gotoxy(35,16);
   printw("19.active:");
   Change_Color(RED);
   printw("%s", onoff[record[fieldshown].active]);
   Change_Color(YELLOW);
   gotoxy(35,17);
   printw("20.autovalue:");
   Change_Color(RED);
   printw("*");
   Change_Color(MAGENTA);
   gotoxy(18, 18);
   printw("<arrows><enter><space><esc><$ &>");
   gotoxy(18, 19);
   printw("<j>ump <u>ndo <f>ront <ins><del>");
   sprintf(ttext, "[%s]&[%s]", FIELDTYPES[record[fieldshown].type], BUTTONBOXES[record[fieldshown].buttonbox]);
   Change_Color(CYAN);
   gotoxy(18 + (32 - (int) strlen(ttext))/2, 21);
   printw("%s", ttext);
   Change_Color(YELLOW);
   gotoxy(18,20);
   t=sgetch(18,20);
   cleanstdin();
   if ( t!=SPACE && t!=LEFT && t!=RIGHT && t!=ESC && t!=INSERT && t!=DELETE && t!='j' && t!='$' && t!='&' && t!='u' && t != 'f' )
    t='\n';
   switch (t) {
    case 'u':
     record[fieldshown]=backuprecord;
    break;
    case 'f':
     if ( isfielddisplayable(fieldshown) == 0 )
      break;
     Duplicate_Field(fieldshown, 2);
     Delete_Field(fieldshown);
     fieldshown=record.size()-1;
    break;
    case SPACE:
     if ( isfielddisplayable(fieldshown) == 0 )
      break;
     Edit_Field(fieldshown);
    break;
    case '$':
     clear();
     Show_All_Fields_for_Editor( fieldshown, 1 );
     Show_Field_ID( &records[(currentrecord*fieldsperrecord)+fieldshown] );
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
      Show_Message(18, 20, RED, ttext, 1500);
     break;
     case DELETE:
      if (fieldsperrecord<2)
       break;
      printw("delete (y/n):");
      t=sgetch();
      if (tolower(t)=='y') {
       Delete_Field(fieldshown);
       clearinputline();
       Show_Message(18, 20, RED, "field deleted", 1500);
       if ( fieldshown )
        --fieldshown; 
      }
     break;
     case '&':
      printw("duplicate (y/n):");
      t=sgetch();
      if (t!='y')
       break;
      clearinputline();
      gotoxy(18,20);
      Change_Color(GREEN);
      printw("copy records (y/n):");
      t=sgetch();
      t = (t=='y') ? 2 : 0;
      Duplicate_Field(fieldshown, t);
      strcpy(ttext, "duplicate to field ");
      strcat(ttext, itoa(fieldsperrecord));
      Show_Message(18, 20, RED, ttext, 1500);
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
      getyx(win1, y, x);
      gotoxy(x+2, y+1);
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
        i=Scan_Input(1, 1, 64, 2);
        if (i && i<64)
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
        i=Scan_Input(1, 0, 65, 2);
        if (i>-1 && i<66) // 0 or 65 is handled from fieldrepetitions
         record[fieldshown].color=i;
       break;
       case 11:
        i=Scan_Input(1, 0, 1, 1);
        if (!i || i==1)
         record[fieldshown].box=i;
       break;
       case 12:
        i=Scan_Input(1, 1, 64, 2);
        if (i && i<65)
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
        if (i>-1 && i<fieldsperrecord+1)
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
       break;
      }
      if ( isrecordproperlydictated(record[fieldshown]) == 0 )
       record[fieldshown]=backuprecord;
      break;
   }
  }
  clearinputline();
  Change_Color(RED);
  gotoxy(18,20);
  cleanstdin();
  printw("save changes (y/n):");
  t=sgetch();
  if (tolower(t)=='y') {
   alteredparameters=0;
   Read_Write_db_File(3);
   Read_Write_db_File(1); 
  }
  editoroption=-1;
  
}

// references editor
int References_Editor()
{
  int i, t=0, relationshipshown=0;
  char ttext[MAXSTRING];
  vector<Relationship> trelationships=relationships; // create a copy, copy back afterwards if wanted
  Relationship trelationship(const_cast <char *> ("dummydatabase"), 0, 1, 2, 3);
  vector<Relationship>::iterator p;
  alteredparameters=1;
  
  if ( trelationships.size() == 0 )
   trelationships.push_back(trelationship);
  
  while ( t != ESC && trelationships.size() ) {
   Draw_Box(BOXCHAR, 6, 17, 33, 5, 16, 25);
   Show_Menu_Bar(1);
   for (i=18;i<40;i++) {
    gotoxy(i, 20);
   addch(SPACE); }
   Change_Color(GREEN);
   gotoxy(20, 6);
   printw("reccurse relationship tables");
   Change_Color(MAGENTA);
   gotoxy(18,8);
   printw("external database name");
   Change_Color(YELLOW);
   gotoxy(18,9);
   printw("1.filename:%.22s", trelationships[relationshipshown].extDbname);
   Change_Color(MAGENTA);
   gotoxy(18,11);
   printw("equalized fields");
   Change_Color(YELLOW);
   gotoxy(18,12);
   printw("2.external:%d", trelationships[relationshipshown].extFields[0]);
   gotoxy(35,12);
   printw("3.local:%d", trelationships[relationshipshown].localFields[0]);
   gotoxy(18,14);
   Change_Color(MAGENTA);
   printw("references fields");
   Change_Color(YELLOW);
   gotoxy(18,15);
   printw("4.external:%d", trelationships[relationshipshown].extFields[1]);
   gotoxy(35,15); 
   printw("5.local:%d", trelationships[relationshipshown].localFields[1]);
   Change_Color(MAGENTA);
   gotoxy(18,17);
   printw("relatioship id:");
   Change_Color(YELLOW);
   printw("%d", relationshipshown+1);
   Change_Color(GREEN);
   gotoxy(22,18);
   printw("<arrow keys|HOME|END>");
   gotoxy(25,19);
   printw("<INS|DEL|ESC>");
   Change_Color(YELLOW);
   gotoxy(18,20);
   t=tolower(sgetch(18,20));
   cleanstdin(); 
   switch (t) {
    case LEFT:
     if (relationshipshown)
      --relationshipshown;
    break;
    case RIGHT:
     if (relationshipshown<(int) trelationships.size()-1)
      ++relationshipshown;
    break;
    case HOME:
     relationshipshown=0;
    break;
    case END:
     relationshipshown=(int) trelationships.size()-1;
    break;
    case INSERT:
     if ( (int) trelationships.size()<MAXRELATIONSHIPS) {
      trelationships.push_back(trelationship);
     Show_Message(22, 20, RED, "relationship inserted", 1500); }
     else
      Show_Message(20,20, RED, "relationships storage full", 1500);
    break;
    case DELETE:
     p=trelationships.begin();
     p+=relationshipshown;
     trelationships.erase(p);
     relationshipshown+=(!relationshipshown) ? 1 : -1;
     Show_Message(22, 20, RED, "relationship deleted", 1500);
    break;
    case '1':
     Show_Menu_Bar(1);
     strcpy(ttext, trelationships[relationshipshown].extDbname);
     t=Scan_Input(ttext, 1, 24, 49);
     if (t==ESC)
      break;
     Reccurse_File_Extension(ttext, 0);
     cropstring(ttext, MAXNAME-3);
     Reccurse_File_Extension(ttext, 2);
     if (!tryfile(ttext)) {
      Show_Message(22, 20, RED, "nonexistant database", 1500);
     break; }
     strcpy(trelationships[relationshipshown].extDbname, ttext);
    break;
    case '2':
     Change_Color(CYAN);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].extFields[0]=i;
    break;
    case '3':
     Change_Color(CYAN);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].localFields[0]=i;
    break;
    case '4':
     Change_Color(CYAN);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].extFields[1]=i;
    break;
    case '5':
     Change_Color(CYAN);
     gotoxy(18, 20);
     printw("field->");
     i=Scan_Input(1, 1, MAXFIELDS, 3);
     if (i && i<MAXFIELDS+1)
      trelationships[relationshipshown].localFields[1]=i;
     break;
   }
  }
  clearinputline();
  gotoxy(18,20);
  Change_Color(RED);
  cleanstdin();
  printw("save changes (y/n):");
  t=sgetch();
  if (tolower(t)=='y') {
   alteredparameters=0;
   relationships.clear();
   for (i=0;i<(int) trelationships.size();i++)
    if (strcmp(trelationships[i].extDbname, "dummydatabase"))
     relationships.push_back(trelationships[i]);
   Read_Write_db_File(3);
  Read_Write_db_File(1); }
    
 return alteredparameters;
}

// clear input line
void clearinputline()
{
  int i;
  
   Change_Color(WHITE);
   for (i=18;i<40;i++) {
    gotoxy(i, 20);
   addch(SPACE); }
   refresh();
}

int Edit_Field(int &field_id)
{
 int c=0, bc=0, backupmenu=currentmenu, backupbar=menubar, showallrecords=0;
 Field trecord=record[field_id], ttrecord=record[field_id];
 currentmenu=EDITOR; menubar=1;
 
  while ( c != ESC && c != ENTER ) {
    
   clear();
   Show_Menu_Bar();
   if ( menubar == 2 ) { // add new menu
    Show_Menu_Bar(1);
    Change_Color(MAGENTAONYELLOW);
    gotoxy(1, 24);
    printw("<m> toggle bar|<f>ield in front|$*/*-+.|<ENTER>store|<SPACE>revert|<ESC>discard");
    refresh();
   }
   if ( isrecordproperlydictated(record[field_id]) == 0 )
    record[field_id]=ttrecord;
   if ( (record[field_id].size.x * record[field_id].size.y) > MAXSTRING )
    record[field_id].editable=OFF;
   ttrecord=record[field_id];
   Determine_Button_Box(field_id);
   if ( showallrecords ) 
    Show_All_Fields_for_Editor( field_id );
   else
    Show_Field( &records[(currentrecord*fieldsperrecord)+field_id], 1 );
   gotoxy(80, 24);
   if ( bc == 0 )
    c=sgetch();
   else
    c=bc;
   bc=0;
   switch ( c ) {
    case KEY_MOUSE:   
     if (leftmousebuttondoubleclicked())
      if ( mouse.y+1 == 24  && menubar==1 )
       bc=Activate_Menubar_Choice(mouse.x);
     if (rightmousebuttonclicked())
      bc=ENTER;
    break;
    case UP:
     --record[field_id].pt.y;
    break;
    case DOWN:
     ++record[field_id].pt.y;
    break;
    case LEFT:
     --record[field_id].pt.x;
    break;
    case RIGHT:
     ++record[field_id].pt.x;
    break;
    case SHIFT_UP:
     if ( record[field_id].size.y > 1 ) 
      --record[field_id].size.y;
    break;
    case SHIFT_DOWN:
      ++record[field_id].size.y;
    break;
    case SHIFT_LEFT:
     if ( record[field_id].size.x > 1 )
      --record[field_id].size.x;
    break;
    case SHIFT_RIGHT:
     ++record[field_id].size.x;
    break;
    case '1':
     if ( record[field_id].title_color )
      --record[field_id].title_color;
     else
      record[field_id].title_color=64;
    break;
    case '2':
     if ( record[field_id].title_color < 65 )
      ++record[field_id].title_color;
     else
      record[field_id].title_color=0;
    break;
    case '3':
     if ( record[field_id].color )
      --record[field_id].color;
     else
      record[field_id].color=64;
    break;
    case '4':
     if ( record[field_id].color < 65 )
      ++record[field_id].color;
     else
      record[field_id].color=0;
    break;
    case 'b': // box
     record[field_id].box = ( record[field_id].box == 1 ) ? 0 : 1;
    break;
    case '5':
     if ( record[field_id].box_color )
      --record[field_id].box_color;
     else
      record[field_id].box_color=64;
    break;
    case '6':
     if ( record[field_id].box_color < 65 )
      ++record[field_id].box_color;
     else
      record[field_id].box_color=0;
    break;
    case 't':
     if ( record[field_id].title_position < 4 )
      ++record[field_id].title_position;
     else
      record[field_id].title_position=0;
    break;
    case '$':
     showallrecords = ( showallrecords == 1 ) ? 0 : 1;
    break;
    case '*':
     record[field_id].attributes[1]=(record[field_id].attributes[1]=='1') ? '0' : '1';
    break;
    case '-':
     record[field_id].attributes[2]=(record[field_id].attributes[2]=='1') ? '0' : '1';
    break;
    case '/':
     record[field_id].attributes[4]=(record[field_id].attributes[4]=='1') ? '0' : '1';      
    break;
    case '.':
     record[field_id].attributes[5]=(record[field_id].attributes[5]=='1') ? '0' : '1';      
    break;
    case '+':
     record[field_id].attributes[6]=(record[field_id].attributes[6]=='1') ? '0' : '1';
    break;
    case 'm':
    ++menubar;
     menubar=(menubar==3) ? 0 : menubar;
    break;
    case 'f':
     if ( isfielddisplayable(field_id) == 0 )
      break;
     Duplicate_Field(field_id, 2);
     Delete_Field(field_id);
//      record[field_id].active=0;
     field_id=record.size()-1;
     trecord=ttrecord=record[field_id];
    break;
    case SPACE:
     record[field_id]=trecord;
    break;
   }
  }    
  currentmenu=backupmenu;
  menubar=backupbar;
  addch(SPACE); // remove char printed at 80,24
  
  if ( c == ESC )
   record[field_id]=trecord;
 
 return ( c == ESC ) ? 1 : 0;
 
}

// display all fields dimmed, except field_id
void Show_All_Fields_for_Editor(int field_id, int flag) // 0 only fields, 1 field ids
{
  int i;
  
    for (i=0;i<fieldsperrecord;i++) {
     Show_Field( &records[(currentrecord*fieldsperrecord)+i], ( i == field_id ) ? 1 : 0 );
     if ( i != field_id && flag )
      Show_Field_ID( &records[(currentrecord*fieldsperrecord)+i] );
    }
    
}
  
// copy field
void Duplicate_Field(int field_id, int flag)
{
 int i;   
    
  if ( flag == 0 ) {
   Add_Field();
   record[record.size()-1]=record[field_id];
   record[record.size()-1].id=record.size()-1;
   if ( record[field_id].fieldlist-1 == field_id )
    record[record.size()-1].fieldlist=(int) record.size();
  }
  if ( flag == 1 ) {
   for (i=0;i<recordsnumber;i++)
    strcpy(records[(i*fieldsperrecord)+record.size()-1].text, records[(i*fieldsperrecord)+field_id].text);
  }
  if ( flag == 2 ) {
   Duplicate_Field(field_id);
   Duplicate_Field(field_id, 1);
  }
    
}
