// Polish Calculator library

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

// constants
const int MAXSTRING=80; // characters in a regular string
const int MAXTITLE=20; // characters in a a title string
const int MAXSUFFIXCHARS=3; // max string for number suffixes
const int MAXOP=1000; /* max size of operand or operator */
const char NUMBER='0'; /* signal that a number was found */
const int MAXCOMMAND=9999; /* maximum number of operands etc to calculate */
const int MAXVAL=1000; /* maximum depth of val stack */
const int MAXFUNCTIONPARAMETERS=5; // max parameters in functions, separated with comma
enum { NOBUTTON=0, TICKBOX, BUTTONBOX, BUTTONSCREEN, BUTTONCOMMAND, AUTOMATICSCRIPT };
extern int fieldsperrecord;
const int MAXRELATIONSHIPS=25;

int pos, sp; /* next free stack position */
double val[MAXVAL]; /* value stack */
int isfunction=0; // flag to take functions symbols into account
int openparentheses; // parentheses open for parser

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

// vectors
extern vector<Field> record, dummyrecord, externalrecord[MAXRELATIONSHIPS];
extern vector<Annotated_Field> records, dummyrecords, externalrecords[MAXRELATIONSHIPS];

// function declarations
int parenthesesincluderforpolishreversecalculator(char formula[]);
int reversepolishcalculatorequalizer(char formula[], int record_id=-1);
int isformulainpolishcalculatorsyntax(char formula[]);
double reversepolishcalculator(char formula[]);
int getop(char s[], char formula[]);
void push(double f);
double pop(void);
extern int isdecimalseparator(char t);
extern char *ctos(int t);
extern char *dtoa(long double val);
extern int mod(double a, double b);

#include "rcpclib.cc"

// reverse Polish calculator start

// reverse polish calculator
double reversepolishcalculator(char formula[])
{
int type;
double op2,op3;
char s[MAXOP];

    pos=sp=0;
    while ((type = getop(s, formula)) != EOF) {
     switch (type) {
      case NUMBER:
       push(atof(s));
      break;
      case '+':       
       push(pop() + pop());
      break;
      case '*':
       push(pop() * pop());
      break;
      case '^':
       push (pow(pop(),pop()));
      break;
      case '-':
       op2 = pop();
       push(pop() - op2);
      break;
     case '/':
      op2 = pop();
      if (op2)
       push(pop() / op2);
     break;
     case '%':
      int iop4, iop5;
      iop4=(int) pop();
      iop5=(int) pop();
      if (iop4) {
       op3=iop5 % iop4;;
      push(op3); }
      else
       push(0);
     break;
     case '$':
      int op5;
      pop();
      op2=pop();
      if (op2)
       push((op5=rand() % (int) op2));
     break;
     case '@':
      int op4;
      op2=pop() * pop();
      op3=mod(op2,10);
      op4= (op2>9) ? (op2/10) + op3 : op2;
      op2=op4;
      push(op2);
     break;
     // new functions
     case '!':
      pop();
      push(abs((pop())));
     break;
     case '[':
      pop();
      push(sin(pop()));
     break;
     case ']':
      pop();
      push(cos(pop()));
     break;
     case ';':
      pop();
      push(tan(pop()));
     break;
     case '"':
      pop();
      op2=pop();
      if (op2)
       push(1/(tan(op2)));
      else
       push(0);
     break;
     case '?':
      pop();
      push(sqrt(pop()));
     break;
     case ':':
      pop();
      push(log(pop()));
     break;
     case '\0':
      return pop();
     default:
   break; } }
   
 return 0;
}

/* getop: get next character or numeric operand */
int getop(char s[], char formula[])
{
int i, c;

  if (pos>strlen(formula))
   return EOF;
  while ((s[0] = c = formula[pos++]) == ' ' || c == '\t');
  s[1] = '\0';
  if (c=='-' && isdigit(formula[pos]))
   c=formula[pos];
  if (!isdigit(c) && !isdecimalseparator(c))
   return c; /* not a number */
  i = 0;
  if (isdigit(c)) /* collect integer part */
  while (isdigit(s[++i] = c = formula[pos++]));
  if (isdecimalseparator(c)) { /* collect fraction part */
   s[i]='.';
  while (isdigit(s[++i] = c = formula[pos++])); }
  s[i] = '\0';

return NUMBER;
}

/* push: push f onto value stack */
void push(double f)
{
  if (sp < MAXVAL)
   val[sp++] = f;
}

/* pop: pop and return top value from stack */
double pop(void)
{
  if (sp > 0)
   return val[--sp];
  else
  return 0.0;
}

// parentheses clearance
int parenthesesincluderforpolishreversecalculator(char formula[])
{
  char tformula[MAXCOMMAND], tendformula[MAXCOMMAND], ts[50];
  int i, i1, n, tpos[2], operation=1;
  double f;
  
   // check for equal number of parentheses
   openparentheses=0;
   for (i=0;i<strlen(formula);i++) {
    if (formula[i]=='(')
     ++openparentheses;
    if (formula[i]==')')
   --openparentheses; }
   if (openparentheses) // not all parentheses closed
    return openparentheses;
   strcpy(tformula, formula);
   while (operation) {
    operation=0;
    for (i=0;i<strlen(tformula);i++) {
     if (tformula[i]==')') {
      tpos[1]=i; tpos[0]=i;
      while (tformula[tpos[0]]!='(')
       --tpos[0];
      operation=1;
      // copy parenthesis content into formula
      for (n=0;n<tpos[1]-tpos[0]-1;n++)
       formula[n]=tformula[tpos[0]+n+1];
      formula[n]='\0';
      // run reversepolishcalculator routines
      reversepolishcalculatorequalizer(formula);
      f=reversepolishcalculator(formula);
      // fill tformula parenthesis content with spaces
      for (n=tpos[0];n<tpos[1]+1;n++)
       tformula[n]=' ';
      // copy rest of of tformula to tendformula
      i1=0;
      for (n=tpos[1];n<strlen(tformula);n++)
       tendformula[i1++]=tformula[n];
      tendformula[i1]='\0';
      // append f to end of tformula
      strcpy(ts, dtoa(f));
      for (n=0;n<strlen(ts);n++)
       tformula[tpos[0]+n+1]=ts[n];
      // append tendformula to formula
      for (i1=0;i1<strlen(tendformula);i1++)
       tformula[tpos[0]+n+i1]=tendformula[i1];
      tformula[tpos[0]+n+i1]='\0';
    break; } } }
   strcpy(formula, tformula);
   
  return 0;
}

// equalize text for reversepolishcalculator use
int reversepolishcalculatorequalizer(char formula[], int record_id)
{
  char transformedtext[MAXCOMMAND], tc, s[MAXOP];
  int type, i, n, i1, numbers=0;
  
   // replace #id with values from corresponding records
   n=0;
   for (i=0;i<strlen(formula);i++) {
    if (formula[i]!='#')
     transformedtext[n++]=formula[i];
    else {
     i1=0;
     while (isdigit(formula[++i]) && i<strlen(formula))
      s[i1++]=formula[i];
     s[i1]='\0';
     --i;
     if (atoi(s) && atoi(s)<fieldsperrecord+1 &&  atof(records[(record_id*fieldsperrecord)+atoi(s)-1].text))
      strcpy(s, records[(record_id*fieldsperrecord)+atoi(s)-1].text);
     else 
      strcpy(s, "0");
     for (i1=0;i1<strlen(s);i1++)
   transformedtext[n++]=s[i1]; } }
   transformedtext[n]='\0';
   strcpy(formula, transformedtext);
   
   // lets see if formula has any digits
   n=0;
   for (i=0;i<strlen(formula);i++)
    if (isdigit(formula[i]))
     ++n;
   if (!n) // no numbers, return 0 (this was done done for mixed type formula fields)
    return 0;
  
   // repair space positions where necessary 
   n=0;
   for (i=0;i<strlen(formula);i++) {
    while (isspace(formula[i]) && isspace(formula[i+1]))
     ++i;
    transformedtext[n]=formula[i];
    if (isdigit(transformedtext[n]) && !isdigit(formula[i+1]) && !isdecimalseparator(formula[i+1]))
     transformedtext[++n]=' ';
    if (!isdigit(transformedtext[n]) && isdigit(formula[i+1]) && !isdecimalseparator(transformedtext[n]) &&transformedtext[n]!='-' && !isspace(transformedtext[n]))
     transformedtext[++n]=' '; 
    ++n; }
   transformedtext[n]='\0';
   strcpy(formula, transformedtext);

   if (!isformulainpolishcalculatorsyntax(formula)) {
    type=getop(s, formula); // read first number
    strcpy(transformedtext, s);
    while ((type = getop(s, formula)) != EOF) {
     if (type==NUMBER) {
      strcat(transformedtext, s);
     ++numbers; }
     strcat(transformedtext, " ");
     if (type!=NUMBER)
      tc=type;
     if (numbers) {
      strcat(transformedtext, ctos(tc));
     numbers=0; } } 
    strcpy(formula, transformedtext); }
    
 return 1;
}

// is string in polish calculator syntax ?
int isformulainpolishcalculatorsyntax(char formula[])
{
  int type;
  char s[MAXOP];
  pos=0;
  
   if ((type=getop(s, formula))==NUMBER && (type=getop(s, formula))==NUMBER) {
    pos=0;
   return 1; }
   pos=0;
  
 return 0;
}

// end of Polish Calculator
