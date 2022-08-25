// string formula parser
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

// numericals
const int MAXSTRING=80; // characters in a regular string
const int MAXPARAMETER=25;
const int MAXRELATIONSHIPS=25; // will fit into above 1kb, same as MAXPAGES
const int INSTRUCTION='%';
enum { HORIZONTALLY=1, VERTICALLY };
enum { NOBUTTON=0, TICKBOX, BUTTONBOX, BUTTONSCREEN, BUTTONCOMMAND, AUTOMATICSCRIPT };
enum { NUMERICAL=0, CALENDAR, STRING, MIXEDTYPE, VARIABLE, PROGRAM, CLOCK };
enum { NORMAL=0, STANDOUT, UNDERLINE, REVERSE, BLINK, DIM, BOLD, PROTECT, INVISIBLE };
enum { TOLEFT=1, CENTER, TORIGHT };
enum { SAME=0, LOWER, UPPER };

int nextletterssize;
const char *commands[]={ "mid$(", "left$(", "right$(", "toupper$(", "tolower$(" };
enum { MID = 0, LEFT, RIGHT, TOUPPER, TOLOWER };
const int commandssize=5;
extern int fieldsperrecord;
extern int changedrecord, editoroption;
extern int currentrecord;
extern int recordsnumber;
extern int currentfield;
extern int alteredparameters;
extern int separatort, separatord, suffixposition;

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

extern vector<Annotated_Field> records, dummyrecords, externalrecords[MAXRELATIONSHIPS];

// function declarations
int stringformulacalculator(char formula[MAXSTRING], int record_id);
int replacepartoftextwithcorrespondingvalue(char ttext[MAXSTRING], int record_id);
int extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt);
void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point);
int commandparser(int reference, char tcommand[MAXSTRING]);
char* formatreplacer(char source[MAXSTRING], int field_id);
char *performinstruction(char instruction[MAXSTRING], int field_id);
extern int findsimple(char text[], char token[]);

int stringformulacalculator(char formula[MAXSTRING], int record_id)
{
  int i, startpt, endpt, findinstructions=1, operations;
  char ttext[MAXSTRING], tcommand[MAXSTRING];
  strcpy(ttext, formula);

  // replace id references
  operations=replacepartoftextwithcorrespondingvalue(ttext, record_id);
  // locate instruction, calculate, reappend to ttext
  while (findinstructions) {
   findinstructions=0;
   for (i=0;i<commandssize;i++) {
    if ((startpt=findsimple(ttext, const_cast <char *> (commands[i])))) {
     break;
    }
   }
   if (i<commandssize) {
    findinstructions=1;
    ++operations;
    --startpt;
    endpt=startpt;
    while (ttext[endpt]!=')' && endpt<(int) strlen(ttext))
     ++endpt;
    if ( endpt == (int)strlen(ttext) )
     return 0;
    ++endpt;
    extracttextpart(ttext, tcommand, startpt, endpt);
    // parse tcommand
    if ( (commandparser(i, tcommand)) == 0 )
     return 0;
    // now insert tcommand into startpt
   inserttextpart(ttext, tcommand, startpt);
   }
  }

  strcpy(formula, ttext);
  
 return operations;
}

//replace #id with values from corresponding records
int replacepartoftextwithcorrespondingvalue(char ttext[MAXSTRING], int record_id)
{
  int i, i1, n, operations=0;
  char transformedtext[MAXSTRING], s[MAXSTRING];

  n=0;
  for (i=0;i<(int)strlen(ttext);i++) {
   if (ttext[i]!='#')
    transformedtext[n++]=ttext[i];
   else {
    i1=0;
    while ( i<(int)strlen(ttext) && isdigit(ttext[++i]) )
     s[i1++]=ttext[i];
    s[i1]='\0';
    --i;
    if ( atoi(s) && atoi(s)<fieldsperrecord && atoi(s)-1!=currentfield && strlen(records[(record_id*fieldsperrecord)+atoi(s)-1].text) ) {
     strcpy(s, records[(record_id*fieldsperrecord)+atoi(s)-1].text);
     ++operations;
    }
    else 
     strcpy(s, " ");
    for (i1=0;n < MAXSTRING - 1 && i1<(int)strlen(s);i1++)
     transformedtext[n++]=s[i1];
   }
  }
  transformedtext[n]='\0';
  strcpy(ttext, transformedtext);
  
 return operations;
}

// remove part of text and place in a char array
int extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt)
{
  int i, n=0;
  char ttext[MAXSTRING];
  
   for (i=startpt;i<endpt;i++)
    dest[n++]=source[i];
   dest[n]='\0';
   n=0;
   for (i=0;i<(int) strlen(source);i++)
    if (i<startpt || i>=endpt)
     ttext[n++]=source[i];
   ttext[n]='\0';
   strcpy(source, ttext);
   
 return (int) strlen(source);
}

// insert text in text
void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point)
{
  char ttext[MAXSTRING];
  int i, n=0;
  
   for (i=0;i<point;i++)
    ttext[n++]=text[i];
   for (i=0;i<(int) strlen(part) && n<MAXSTRING;i++)
    ttext[n++]=part[i];
   for (i=point;i<(int) strlen(text) && n<MAXSTRING;i++)
    ttext[n++]=text[i];
   ttext[n]='\0';
   strcpy(text, ttext);; 
}
    
// command parser
int commandparser(int reference, char tcommand[MAXSTRING])
{
  int i, n, pos, endpos, requiredparameters=0, parameters[2];
  char tparameter[MAXPARAMETER], ttext[MAXSTRING], tttext[MAXSTRING];

  // how many parameters to read  
  if ( reference == LEFT || reference == RIGHT ) { // 0 for toupper,tolower, 1 for left$,right$
   requiredparameters=1;
  }
  if ( reference == MID ) {
   requiredparameters=2; // 2 parameters for mid$
  }
   
   // go back from end of tcommand
   pos=(int)strlen(tcommand);
   n=requiredparameters;
   while (n && pos)
    if (tcommand[pos--]==',')
     --n;
   if (!pos || n)
    return 0;
   pos+=2; // now pos is at first numerical parameter or space
   if (!requiredparameters)
    pos=(int)strlen(tcommand);
   endpos=pos-1; // use later
   // read parameters
   for (i=0;i<requiredparameters;n=0, i++) {
    while ( pos < (int)strlen(tcommand) && ((isdigit(tcommand[pos]) || isspace(tcommand[pos]))) )
     tparameter[n++]=tcommand[pos++];
    tparameter[n]='\0';
    ++pos;
    parameters[i]=atoi(tparameter); 
    if ( parameters[i] == 0 )
     return 0;
   }
   // get string to apply command on
   i=0; n=0;
   while ( i < (int)strlen(tcommand) && tcommand[i++]!='(');
   for (;i<endpos;i++)
    ttext[n++]=tcommand[i];
   ttext[n]='\0';
   // apply command on ttext
   n=0;
   switch (reference) {
    case MID: // mid$
     for (i=parameters[0]-1;i < (int)strlen(ttext) && i<parameters[0]+parameters[1]-1;i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case LEFT: // left$
     for (i=0;i < (int)strlen(ttext) && i<parameters[0];i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case RIGHT: // right$
     for (i=(int)strlen(ttext)-parameters[0];i<(int)strlen(ttext);i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case TOUPPER: // toupper
     for (i=0;i<(int)strlen(ttext);i++)
      tttext[n++]=toupper(ttext[i]);
     tttext[n]='\0';
    break;
    case TOLOWER: // tolower
     for (i=0;i<(int)strlen(ttext);i++)
      tttext[n++]=tolower(ttext[i]);
     tttext[n]='\0';
   break; }
   // available char string to transfer to calling routine is tcommand
   strcpy(tcommand, tttext);

 return 1;
}

// execute commands for format string
char* formatreplacer(char source[MAXSTRING], int field_id)
{
  int i, i1, operation=1;
  static char formattedtext[MAXSTRING];
  char instruction[MAXSTRING], newsource[MAXSTRING];

   strcpy(newsource, source);
   stringformulacalculator(newsource, currentrecord); // treat as a formula record
   i=i1=nextletterssize=0;
   
   while (operation) {
    operation=0;
       
    for (;i<(int) strlen(newsource);i++) {
     if (newsource[i]==INSTRUCTION) {
      extracttextpart(newsource, instruction, i, i+3);
      inserttextpart(newsource, performinstruction(instruction, field_id), i);
      operation=1;
     break; }
     switch (nextletterssize) {
      case UPPER:
       formattedtext[i1++]=toupper(newsource[i]);
      break;
      case LOWER:
       formattedtext[i1++]=tolower(newsource[i]);
      break;
      case SAME:
       formattedtext[i1++]=newsource[i];
     break; }
    }
   }
   formattedtext[i1]='\0'; 
    
  return &formattedtext[0];   
}

// instructions in automatic values
char *performinstruction(char instruction[MAXSTRING], int field_id)
{

  int i;
  char number[3];
  if (isdigit(instruction[1]) || isdigit(instruction[2])) { // characters from field string to bring
   number[0]=instruction[1];
   number[1]=(int) strlen(instruction)==3 ? instruction[2] : ' ';
  instruction[1]='t'; }
  
   switch(instruction[1]) {
    case 'f': // capital or not for the next characters
     switch (instruction[2]) {
      case 'c':
       nextletterssize=UPPER;
      break;
      case 's':
       nextletterssize=LOWER;
      break;
      default:
       nextletterssize=SAME;
     break; }
     break;
     case 't': // insert current fields text, if number follows, cut after n chars
      i=atoi(number);
      strcpy(instruction, records[(currentrecord*fieldsperrecord)+field_id].text);
      if (i)
       instruction[i]='\0';
      return &instruction[0];
     break;
     case 'm':
      switch (instruction[2]) {
       case 'i':
        separatort=44;
       break;
       case 'm':
        separatort=46;
       break;
       case 'f':
        suffixposition=0;
       break;
       case 'b':
        suffixposition=1;
      break; }
     break;
     default:
      // add more
    break; }
    strcpy(instruction, "");
    
 return &instruction[0];   
}
