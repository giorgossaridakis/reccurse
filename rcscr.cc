// reccurse ncurses screen modules
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

// global constants
enum { NORMAL=0, STANDOUT, UNDERLINE, REVERSE, BLINK, DIM, BOLD, PROTECT, INVISIBLE };
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
int BLACK=BLACKONBLACK, RED=REDONBLACK, GREEN=GREENONBLACK, YELLOW=YELLOWONBLACK, BLUE=BLUEONBLACK, MAGENTA=MAGENTAONBLACK, CYAN=CYANONBLACK, WHITE=WHITEONBLACK;

struct Points {
 int x;
int y; } ;

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

// local variables
WINDOW *win1=newwin(80, 24, 1, 1);
const char BOXCHAR='*';
int highlightcolors[2]={ MAGENTAONYELLOW, YELLOWONMAGENTA };
int terminalhascolor=1;

// external
extern int screen[81][25];
extern int printscreenmode;
const int SPACE=32;

// function declarations
int Init_Screen();
void End_Screen();
void Change_Color(int choice=WHITE);
void Draw_Box(int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=BLACK);
void Draw_Box(char t, int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor=0);
void Draw_Box(Drawbox &tdrawbox);
void gotoxy(int x, int y);
void Change_Attributes(int attribute);

// Initialize screen
int Init_Screen()
{
  win1=initscr();
  noecho();
  cbreak();
  keypad(win1, TRUE);
  mousemask(ALL_MOUSE_EVENTS, NULL);
  if (has_colors() == FALSE) 
   terminalhascolor=0;
  curs_set(1);
  start_color();
  
  // black on others
  init_pair(BLACKONBLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(BLACKONRED, COLOR_BLACK, COLOR_RED);
  init_pair(BLACKONGREEN, COLOR_BLACK, COLOR_GREEN);
  init_pair(BLACKONYELLOW, COLOR_BLACK, COLOR_YELLOW);
  init_pair(BLACKONBLUE, COLOR_BLACK, COLOR_BLUE);
  init_pair(BLACKONMAGENTA, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(BLACKONCYAN, COLOR_BLACK, COLOR_CYAN);
  init_pair(BLACKONWHITE, COLOR_BLACK, COLOR_WHITE);
  // red on others
  init_pair(REDONBLACK, COLOR_RED, COLOR_BLACK);
  init_pair(REDONRED, COLOR_RED, COLOR_RED);
  init_pair(REDONGREEN, COLOR_RED, COLOR_GREEN);
  init_pair(REDONYELLOW, COLOR_RED, COLOR_YELLOW);
  init_pair(REDONBLUE, COLOR_RED, COLOR_BLUE);
  init_pair(REDONMAGENTA, COLOR_RED, COLOR_MAGENTA);
  init_pair(REDONCYAN, COLOR_RED, COLOR_CYAN);
  init_pair(REDONWHITE, COLOR_RED, COLOR_WHITE);
  // green on others
  init_pair(GREENONBLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(GREENONRED, COLOR_GREEN, COLOR_RED);
  init_pair(GREENONGREEN, COLOR_GREEN, COLOR_GREEN);
  init_pair(GREENONYELLOW, COLOR_GREEN, COLOR_YELLOW);
  init_pair(GREENONBLUE, COLOR_GREEN, COLOR_BLUE);
  init_pair(GREENONMAGENTA, COLOR_GREEN, COLOR_MAGENTA);
  init_pair(GREENONCYAN, COLOR_GREEN, COLOR_CYAN);
  init_pair(GREENONWHITE, COLOR_GREEN, COLOR_WHITE);
  // yellow on others
  init_pair(YELLOWONBLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(YELLOWONRED, COLOR_YELLOW, COLOR_RED);
  init_pair(YELLOWONGREEN, COLOR_YELLOW, COLOR_GREEN);
  init_pair(YELLOWONYELLOW, COLOR_YELLOW, COLOR_YELLOW);
  init_pair(YELLOWONBLUE, COLOR_YELLOW, COLOR_BLUE);
  init_pair(YELLOWONMAGENTA, COLOR_YELLOW, COLOR_MAGENTA);
  init_pair(YELLOWONCYAN, COLOR_YELLOW, COLOR_CYAN);
  init_pair(YELLOWONWHITE, COLOR_YELLOW, COLOR_WHITE);
  // blue on others
  init_pair(BLUEONBLACK, COLOR_BLUE, COLOR_BLACK);
  init_pair(BLUEONRED, COLOR_BLUE, COLOR_RED);
  init_pair(BLUEONGREEN, COLOR_BLUE, COLOR_GREEN);
  init_pair(BLUEONYELLOW, COLOR_BLUE, COLOR_YELLOW);
  init_pair(BLUEONBLUE, COLOR_BLUE, COLOR_BLUE);
  init_pair(BLUEONMAGENTA, COLOR_BLUE, COLOR_MAGENTA);
  init_pair(BLUEONCYAN, COLOR_BLUE, COLOR_CYAN);
  init_pair(BLUEONWHITE, COLOR_BLUE, COLOR_WHITE);
  // magenta on others
  init_pair(MAGENTAONBLACK, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(MAGENTAONRED, COLOR_MAGENTA, COLOR_RED);
  init_pair(MAGENTAONGREEN, COLOR_MAGENTA, COLOR_GREEN);
  init_pair(MAGENTAONYELLOW, COLOR_MAGENTA, COLOR_YELLOW);
  init_pair(MAGENTAONBLUE, COLOR_MAGENTA, COLOR_BLUE);
  init_pair(MAGENTAONMAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
  init_pair(MAGENTAONCYAN, COLOR_MAGENTA, COLOR_CYAN);
  init_pair(MAGENTAONWHITE, COLOR_MAGENTA, COLOR_WHITE);
  // cyan on others
  init_pair(CYANONBLACK, COLOR_CYAN, COLOR_BLACK);
  init_pair(CYANONRED, COLOR_CYAN, COLOR_RED);
  init_pair(CYANONGREEN, COLOR_CYAN, COLOR_GREEN);
  init_pair(CYANONYELLOW, COLOR_CYAN, COLOR_YELLOW);
  init_pair(CYANONBLUE, COLOR_CYAN, COLOR_BLUE);
  init_pair(CYANONMAGENTA, COLOR_CYAN, COLOR_MAGENTA);
  init_pair(CYANONCYAN, COLOR_CYAN, COLOR_CYAN);
  init_pair(CYANONWHITE, COLOR_CYAN, COLOR_WHITE);
  // white on others
  init_pair(WHITEONBLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(WHITEONRED, COLOR_WHITE, COLOR_RED);
  init_pair(WHITEONGREEN, COLOR_WHITE, COLOR_GREEN);
  init_pair(WHITEONYELLOW, COLOR_WHITE, COLOR_YELLOW);
  init_pair(WHITEONBLUE, COLOR_WHITE, COLOR_BLUE);
  init_pair(WHITEONMAGENTA, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(WHITEONCYAN, COLOR_WHITE, COLOR_CYAN);
  init_pair(WHITEONWHITE, COLOR_WHITE, COLOR_WHITE);

  attron(COLOR_PAIR(7));  
 return 0;
}

// close screen
void End_Screen()
{
  endwin();
}

// change color
void Change_Color(int choice)
{
  
  if ( terminalhascolor == 0 )
   return;
  
   attron(COLOR_PAIR(choice));
}

// draw a usual box
void Draw_Box(int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor)
{
  int i, boxchar=BOXCHAR;
  
   if (!printscreenmode) {
    attron(A_ALTCHARSET);
   Change_Color(color); }
   else
    boxchar='*';
  
   gotoxy(x_pos, y_pos);
   screen[x_pos][y_pos]=boxchar;
   addch(108);
   gotoxy(x_pos+x_size, y_pos);
   screen[x_pos+x_size][y_pos]=boxchar;
   addch(107);
   gotoxy(x_pos, y_pos+y_size);
   screen[x_pos][y_pos+y_size]=boxchar;
   addch(109);
   gotoxy(x_pos+x_size, y_pos+y_size);
   screen[x_pos+x_size][y_pos+y_size]=boxchar;
   addch(106);
   for (i=x_pos+1;i<x_pos+x_size;i++) {
    gotoxy(i, y_pos);
    screen[i][y_pos]=boxchar;
    addch(113);
    gotoxy(i, y_pos+y_size);
    screen[i][y_pos+y_size]=boxchar;
   addch(113); }
   for (i=y_pos+1;i<y_pos+y_size;i++) {
    gotoxy(x_pos, i);
    screen[x_pos][i]=boxchar;
    addch(120);
    gotoxy(x_pos+x_size, i);
    screen[x_pos+x_size][i]=boxchar;
   addch(120); }
   attroff(A_ALTCHARSET);
   if (paintcolor) {
    Change_Color(paintcolor);
    int n;
    for (i=x_pos+1;i<x_pos+x_size;i++) {
     for (n=y_pos+1;n<y_pos+y_size;n++) {
      gotoxy(i, n);
    addch(SPACE); } }
    Change_Color(WHITE); }

  refresh();
}

// draw a box with custom char
void Draw_Box(char t, int color, int x_pos, int x_size, int y_pos, int y_size, int paintcolor)
{
  int i;  
   
   Change_Color(color);
   for (i=x_pos;i<x_pos+x_size+1;i++) {
    gotoxy(i, y_pos);
    addch(t);
    gotoxy(i, y_pos+y_size);
   addch(t); }
   for (i=y_pos;i<y_pos+y_size;i++) {
    gotoxy(x_pos, i);
    addch(t);
    gotoxy(x_pos+x_size, i);
   addch(t); }
   
  if (paintcolor) {
   Change_Color(paintcolor);
   int n;
   for (i=x_pos+1;i<x_pos+x_size;i++) {
    for (n=y_pos+1;n<y_pos+y_size;n++) {
     gotoxy(i, n);
   addch(SPACE); } }
  Change_Color(WHITE); }
  refresh();
}

// drawbox overloaded for class
void Draw_Box(Drawbox &tdrawbox)
{
  int i, x_pos, y_pos, x_size, y_size, paintcolor, color;

  color=tdrawbox.drawcolor; 
  paintcolor=tdrawbox.paintcolor;
  x_pos=tdrawbox.pt.x;
  y_pos=tdrawbox.pt.y;
  x_size=tdrawbox.size.x;
  y_size=tdrawbox.size.y;
  attron(A_ALTCHARSET); 
  
  Change_Color(color);
  gotoxy(x_pos, y_pos);
  addch(108);
  gotoxy(x_pos+x_size, y_pos);
  addch(107);
  gotoxy(x_pos, y_pos+y_size);
  addch(109);
  gotoxy(x_pos+x_size, y_pos+y_size);
  addch(106);
  for (i=x_pos+1;i<x_pos+x_size;i++) {
   gotoxy(i, y_pos);
   addch(113);
   gotoxy(i, y_pos+y_size);
  addch(113); }
  for (i=y_pos+1;i<y_pos+y_size;i++) {
   gotoxy(x_pos, i);
   addch(120);
   gotoxy(x_pos+x_size, i);
  addch(120); }
  attroff(A_ALTCHARSET);
  if (paintcolor) {
   Change_Color(paintcolor);
   int n;
   for (i=x_pos+1;i<x_pos+x_size;i++) {
    for (n=y_pos+1;n<y_pos+y_size;n++) {
     gotoxy(i, n);
   addch(SPACE); } }
  Change_Color(WHITE); }
 refresh();
}

// gotoxy
void gotoxy(int x, int y)
{
  move(y-1,x-1);
}

// change attributes
void Change_Attributes(int attribute)
{
  switch (attribute) {
   case NORMAL:
    attrset(0);
   break;
   case STANDOUT:
    attron(A_STANDOUT);
   break;
   case UNDERLINE:
    attron(A_UNDERLINE);
   break;
   case REVERSE:
    attron(A_REVERSE);
   break;
   case BLINK:
    attron(A_BLINK);
   break;
   case DIM:
    attron(A_DIM);
   break;
   case BOLD:
    attron(A_BOLD);
   break;
   case PROTECT:
    attron(A_PROTECT);
   break;
   case INVISIBLE:
    attron(A_INVIS);
  break; }

}
