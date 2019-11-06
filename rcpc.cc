#define MAXOP 100 /* max size of operand or operator */
#define NUMBER '0' /* signal that a number was found */
#define MAXCOMMAND 9999 /* maximum number of operands etc to calculate */
#define NUMLIMIT 32767 /* maximum limit in input */
#define MAXVAL 100 /* maximum depth of val stack */
#define MAXFUNCTIONMAME 10 // max chars in function names
#define MAXFUNCTIONPARAMETERS 5 // max parameters in functions, separated with comma

void initiatemathematicalfunctions();
int parseformulaforfunctions(char formula[]);
int mathfunctionsparser(int function_id, char tcommand[MAXSTRING]);
int parenthesesincluderforpolishreversecalculator(char formula[]);
int reversepolishcalculatorequalizer(char formula[], int record_id=-1);
int isformulainpolishcalculatorsyntax(char formula[]);
double reversepolishcalculator(char formula[]);
int getop(char s[], char formula[]);
void push(double f);
double pop(void);

// char formula[MAXCOMMAND];
int pos;
int sp = 0; /* next free stack position */
double val[MAXVAL]; /* value stack */
int isfunction=0; // flag to take functions symbols into account
int openparentheses; // parentheses open for parser

class Function {
 public:
  char functionName[MAXFUNCTIONMAME]; // abs, sin, cos etc
  int functionParameters; // all taken as strings
  char functionSymbol; 
  Function(char s[MAXFUNCTIONMAME], int i, char c) { strcpy(functionName, s); functionParameters=i; functionSymbol=c; } ;
  Function(const char s[MAXFUNCTIONMAME], int i, char c) { strcpy(functionName, s); functionParameters=i; functionSymbol=c; } ;
  Function() { } ;
~Function() { }; 
} ;

vector<Function> functions;

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
     // new functions
     case 'a':
      pop();
      push(abs((pop())));
     break;
     case 's':
      pop();
      push(sin(pop()));
     break;
     case 'c':
      pop();
      push(cos(pop()));
     break;
     case 't':
      pop();
      push(tan(pop()));
     break;
     case 'o':
      pop();
      op2=pop();
      if (op2)
       push(1/(tan(op2)));
     break;
     case 'r':
      pop();
      push(sqrt(pop()));
     break;
     case 'l':
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

// initiate functions
void initiatemathematicalfunctions()
{
  Function tfunction1("abs", 1, 'a');
  functions.push_back(tfunction1);
  Function tfunction2("sin", 1, 's');
  functions.push_back(tfunction2);
  Function tfunction3("cos", 1, 'c');
  functions.push_back(tfunction3);
  Function tfunction4("tan", 1, 't');
  functions.push_back(tfunction4);
  Function tfunction5("cotan", 1, 'o');
  functions.push_back(tfunction5);
  Function tfunction6("sqrt", 1, 'r');
  functions.push_back(tfunction6);
  Function tfunction7("exp", 2, '^');
  functions.push_back(tfunction7);
  Function tfunction8("log", 1, 'l');
  functions.push_back(tfunction8);
}

// parse formula for functions and replace with symbols
int parseformulaforfunctions(char formula[])
{
  int i, startpt, endpt, findinstructions=1, parseresult;
  char ttext[MAXSTRING], tcommand[MAXSTRING];
  strcpy(ttext, formula);
  
   while (findinstructions) {
    findinstructions=0;
    for (i=0;i<functions.size();i++)
     if ((startpt=findsimple(ttext, functions[i].functionName)))
      break;
     if (i<functions.size()) {
      findinstructions=1;
      --startpt;
      endpt=startpt;
      while (ttext[endpt]!='(' && endpt<strlen(ttext))
       ++endpt;
      ++endpt;
      openparentheses=1;
      while (openparentheses && endpt<strlen(ttext)) {
       if (ttext[endpt]=='(')
        ++openparentheses;
       if (ttext[endpt]==')')
        --openparentheses;
      ++endpt; }
      if (openparentheses) // not all parentheses closed
       return openparentheses;
      if (endpt==strlen(ttext)-1)
       findinstructions=0;
      extracttextpart(ttext, tcommand, startpt, endpt);
      // parse tcommand
      parseresult=mathfunctionsparser(i, tcommand);
      // now insert tcommand into startpt
      if (parseresult)
       return parseresult;
   inserttextpart(ttext, tcommand, startpt); } }
   
  strcpy(formula, ttext);
      
 return 0;
}

// math functions command parser
int mathfunctionsparser(int function_id, char tcommand[MAXSTRING])
{
  int i, mpos=0, tp=0, n;
  double d;
  char tparameter[MAXFUNCTIONPARAMETERS][MAXSTRING];
  
   while (tcommand[mpos]!='(')
    ++mpos;
   ++mpos;
   tcommand[strlen(tcommand)-1]=',';
   for (i=0;i<functions[function_id].functionParameters;i++) {
    n=0;
    while (mpos<strlen(tcommand)) {
     if (tcommand[mpos]==',') {
      ++mpos;
     break; }
     if (tcommand[mpos]=='(')  // these were 0 when routine was called
      ++openparentheses;
     if (tcommand[mpos]==')')
      --openparentheses;
     tparameter[tp][n++]=tcommand[mpos];
    ++mpos; }
    tparameter[tp][n]='\0';
    if (!strlen(tparameter[tp])) { // nonexistant parameter
     tcommand[strlen(tcommand)-1]=')';
    return tp; }
    parenthesesincluderforpolishreversecalculator(tparameter[tp]);
    reversepolishcalculatorequalizer(tparameter[tp]);
    d=reversepolishcalculator(tparameter[tp]);
    strcpy(tparameter[tp], dtoa(d));
    ++tp; }
    tcommand[strlen(tcommand)-1]=')';
    if (mpos<strlen(tcommand)-1) // still more tcommand when functions have been read
     return tp;
    // reconstruct tcommand
    n=functions[function_id].functionParameters;
    if (n==1) {
     ++n;
    strcpy(tparameter[1], "1"); }
    strcpy(tcommand, " (");
    for (i=0;i<n;i++) {
      strcat(tcommand, tparameter[i]);
    strcat(tcommand, " "); }
    strcat(tcommand, ctos(functions[function_id].functionSymbol));
    strcat(tcommand, ")");
    
 return 0;
}

// parentheses clearance
int parenthesesincluderforpolishreversecalculator(char formula[])
{
  char tformula[MAXCOMMAND], tendformula[MAXCOMMAND], ts[50];
  int tformula_pos=0, i, i1, n, tpos[2], operation=1;
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
  char s[20];
  pos=0;
  
   if ((type=getop(s, formula))==NUMBER && (type=getop(s, formula))==NUMBER) {
    pos=0;
   return 1; }
   pos=0;
  
 return 0;
}

// end of Polish Calculator
