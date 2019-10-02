#define MAXOP 100 /* max size of operand or operator */
#define NUMBER '0' /* signal that a number was found */
#define MAXCOMMAND 999 /* maximum number of operands etc to calculate */
#define NUMLIMIT 32767 /* maximum limit in input */
#define MAXVAL 100 /* maximum depth of val stack */

int parenthesesincluderforpolishreversecalculator(char formula[]);
int reversepolishcalculatorequalizer(char formula[], int record_id=-1);
double reversepolishcalculator(char formula[]);
int getop(char s[], char formula[]);
void push(double f);
double pop(void);

// char formula[MAXCOMMAND];
int pos;
int sp = 0; /* next free stack position */
double val[MAXVAL]; /* value stack */

// reverse Polish calculator start

// reverse polish calculator
double reversepolishcalculator(char formula[])
{
int type;
double op2,op3;
char s[MAXOP];

    pos=0;
    while ((type = getop(s, formula)) != EOF) {
     switch (type) {
      case NUMBER:
       push(atof(s));
      break;
      case '+':       push(pop() + pop());
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
      op2 = pop();
      if (op2) {
       op3=mod(pop(), op2);
       push(op3); }
      else
     break;
     case '$':
      double op5;
      op5=rand() % 10;
      push(op5);
     break;
     case '@':
      int op4;
      op2=pop() * pop();
      op3=mod(op2,10);
      op4= (op2>9) ? (op2/10) + op3 : op2;
      op2=op4;
      push(op2);
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
   if (c == '.') { /* collect fraction part */
  while (isdigit(s[++i] = c = formula[pos++])); }
  s[i] = '\0';

return NUMBER;
}

/* push: push f onto value stack */
void push(double f)
{
  if (sp < MAXVAL)
   val[sp++] = f;
//   else
//    printf("\nerror: stack full, can't push %g", f);
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
  int tformula_pos=0, i, i1, n, tpos[2], operation=1;
  double f;
  
   strcpy(tformula, formula);
   while (operation) {
    operation=0;
    for (i=0;i<strlen(tformula);i++) {
     if (tformula[i]==')') {
      tpos[1]=i; tpos[0]=i;
      while (tformula[tpos[0]]!='(')
       --tpos[0];
      if (tpos[0]<0)
       return -1;
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
  char transformedtext[MAXCOMMAND], ttransformedtext[MAXCOMMAND];
  int type, i, n, i1, operation=1, numbers=0;
  double f;
  char tc, s[20];
  
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
   
   // is string in polish calculator syntax ?
   pos=0;
   if ((type=getop(s, formula))==NUMBER && (type=getop(s, formula))==NUMBER) {
    pos=0;
   return -1; }
   if (pos) {
    pos=0;
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
    
 return 0;
}

// end of Polish Calculator
