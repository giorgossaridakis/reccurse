// various library utilities

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

const int MAXSTRING=80; // characters in a regular string
const int MAXTOKENS=10;

// function declarations
int mod(double a, double b);
int ctoi(char c);
char *dtoa(long double val);
char *itoa(long int val, int base=10);
void Show_File_Error(char *filename);
int tryfile(char *file);
int Copy_File(char *source, char *dest);
void cropstring(char ttext[], int noofcharacters, int flag=0);
long int filesize(char *filename);
int numberofdigits(long int n);
char *ctos(int t);
double bringfractionpartofdivision(int param1, int param2, int scale=10);
int isspace(char t);
int iscalculationsign(char t);
int isdecimalseparator(char t);
int isparenthesis(char t);
int isprintablecharacter(int t);
int iscorruptstring(char *tstring);
int find(char text[], char token[]);
int findsimple(char text[], char token[]);
int CalcDayNumFromDate(int y, int m, int d);
int isleapyear(int year);
int daysinmonth(int year, int month);
int limitspaces(char *tstring);
int readstringentry(char *line, char *linepart);
unsigned int isseparationchar(char t);

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

// int try if file exists
int tryfile(char *file) 
{
  struct stat   buffer;   
  return (stat (file, &buffer) == 0);
    
//   FILE *f;
//    
//   f=fopen(file, "r");
//   if (!f)
//    return 0;
//   fclose (f);
//   
//  return 1;
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
   
    while ((size = fread(buf, 1, BUFSIZ, f))) 
     fwrite(buf, 1, size, w);
	
	fclose(f);
	fclose(w);	
  return 0;
}

// crop string from it's end or beginning
void cropstring(char ttext[], int noofcharacters, int flag) // 0 end, 1 beginning
{
  int i;
  char ttext2[MAXSTRING];
   
  if (flag) {
   for (i=noofcharacters;i<strlen(ttext);i++)
    ttext2[i-noofcharacters]=ttext[i];
   ttext2[i-noofcharacters]='\0'; 
  strcpy(ttext, ttext2); }
  else if (noofcharacters<=strlen(ttext))
   ttext[noofcharacters]='\0';
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

// number of digits
int numberofdigits(long int n)
{
  int i=0;
  
   while (n) {
    n/=10;
   ++i; }
  
 return i;
}  

// char to null terminated string
char *ctos(int t)
{
  static char s[3];
  
   s[0]=(char) t;
   s[1]='\0';
   
 return s;
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
    if (text[i]==token[0]) {
     hit=i+1;
     for (n=0;n<strlen(token);n++)
      ttoken[n]=text[i+n];
     ttoken[n]='\0';
     if (strcmp(ttoken, token))
      hit=0; 
     else
   break; } }
   
 return hit;
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

// is char operator
int iscalculationsign(char t)
{
   if (t=='+' || t=='-' || t=='*' || t=='/' || t=='%' || t=='^')
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

// parenthesis
int isparenthesis(char t)
{
  if (t=='(' || t==')')
   return 1;
 
 return 0;
}

// is printable character
int isprintablecharacter(int t)
{
  if (t>31 && t<127)
   return 1;
  
 return 0;
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

// return 1 if year is leap
int isleapyear(int year)
{ 
  if ((year%400==0 || year%100!=0) && (year%4==0)) // is a leap year
   return 1;

 return 0;
}

// days in month
int daysinmonth(int year, int month)
{
 return (month==2) ? (28 + isleapyear(year)) : 31 - (month - 1) % 7 % 2;
}

// trim spaces at start and end of string
int limitspaces(char *tstring)
{
  int i, n, spaces;
  char ttstring[MAXSTRING];
  
    i=n=spaces=0;
    while (isspace(tstring[i]) && i<strlen(tstring)) {
     ++i;
    ++spaces; } 
    for (;i<strlen(tstring);i++)
     ttstring[n++]=tstring[i];
    ttstring[n]='\0';
    i=strlen(ttstring)-1;
    while (isspace(ttstring[i])) {
     --i;
    ++spaces; }
    ttstring[++i]='\0';
    if (!strlen(ttstring))
     strcpy(ttstring, " ");
    strcpy(tstring, ttstring);
  
 return spaces;
}

// read entry from file
int readstringentry(char *line, char *linepart)
{
  static int i=0; // source line position
  int i1=0; // word position
  static char tline[MAXSTRING];
// 
  // reset static
  if (i==strlen(line)) {
   i=0;
   return i;
  }
  if ((i && strcmp(line, tline)) || i==0) {
   i=0;
   strcpy(tline, line);
  }

    while (line[i]) {
     if (isseparationchar(line[i++])) {
      if (i1==0)
       continue;
      break;
     }
    linepart[i1++]=line[i-1];
   }
   linepart[i1]='\0';

 return i;
}

enum { NONE=0, WORD };
// word separation characters
unsigned int isseparationchar(char t)
{
  if (t==',')
   return WORD;

 return NONE;
}

