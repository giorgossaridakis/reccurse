// reverse Polish calculator library

// function declarations
void initiatemathematicalfunctions();
int parseformulaforfunctions(char formula[]);
int mathfunctionsparser(int function_id, char tcommand[MAXSTRING]);
int parseformulaforerrors(char formula[]);
extern int findsimple(char text[], char token[]);
extern int extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt);
extern void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point);

class Function {
 public:
  char functionName[MAXSTRING];
  char functionPrototype[MAXCOMMAND]; 
  int functionParameters;
  int internalflag;
  Function(char s1[MAXSTRING], char s2[MAXCOMMAND], int i, int i1=0) { strcpy(functionName, s1); strcpy(functionPrototype, s2); functionParameters=i; internalflag=i1; };
  Function(const char s1[MAXSTRING], const char s2[MAXCOMMAND], int i, int i1=0) { strcpy(functionName, s1); strcpy(functionPrototype, s2); functionParameters=i; internalflag=i1; };
  Function() { } ;
~Function() { }; };

vector<Function> functions;

// initiate functions
void initiatemathematicalfunctions()
{
  Function tfunction1("abs", "(#a 1 !)", 1, 1);
  functions.push_back(tfunction1);
  Function tfunction2("sin", "(#a 1 [)", 1, 1);
  functions.push_back(tfunction2);
  Function tfunction3("cos", "(#a 1 ])", 1, 1);
  functions.push_back(tfunction3);
  Function tfunction4("tan", "(#a 1 ;)", 1, 1);
  functions.push_back(tfunction4);
  Function tfunction5("cotan", "(#a 1 \")", 1, 1);
  functions.push_back(tfunction5);
  Function tfunction6("sqr", "(#a 1 ?)", 1, 1);
  functions.push_back(tfunction6);
  Function tfunction7("log", "(#a 1 :)", 1, 1);
  functions.push_back(tfunction7);
  Function tfunction8("rnd", "(#a 1 $)", 1, 1);
  functions.push_back(tfunction8);
  Function tfunction9("exp", "(#a^#b)", 2, 1);
  functions.push_back(tfunction9);
  Function tfunction10("mod", "(#a%#b)", 2, 1);
  functions.push_back(tfunction10);
}

// parse formula for user functions and call identify
int parseformulaforfunctions(char formula[])
{
  int i, n, startpt, endpt, findinstructions=1, parseresult;
  char ttext[strlen(formula)+1], tcommand[MAXCOMMAND];
  strcpy(ttext, formula);
  
   while (findinstructions) {
    findinstructions=0;
    for (i=0;i<functions.size();i++) {
     if ((startpt=findsimple(ttext, functions[i].functionName))) {
      break;
     }
    }
    if (i<functions.size()) {
     findinstructions=1;
     --startpt;
     n=startpt;
     // empty function callname so it will not be parsed again
     while (ttext[n]!='(' && n<strlen(ttext))
      ttext[n++]=' ';
     endpt=n+1;
     openparentheses=1;
     while (openparentheses && endpt<strlen(ttext)) {
      if (ttext[endpt]=='(')
       ++openparentheses;
      if (ttext[endpt]==')')
       --openparentheses;
     ++endpt; }
     if (openparentheses) // not all parentheses closed
      return openparentheses;
     extracttextpart(ttext, tcommand, startpt, endpt);
     // call routine recursively
     n=parseformulaforfunctions(tcommand);
     if (n)
      return n;
     // parse tcommand
     parseresult=mathfunctionsparser(i, tcommand);
     // now insert tcommand into startpt
     if (parseresult)
      return parseresult;
     inserttextpart(ttext, tcommand, startpt); 
    }
   }
   
  strcpy(formula, ttext);
      
 return 0;
}

// extract user function parameters and replace in functionPrototype
int mathfunctionsparser(int function_id, char tcommand[MAXSTRING])
{
  int i, mpos=0, tp=0, n, operation=1;
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
    return tp*-1; } // negative return to separate from openparentheses
    parenthesesincluderforpolishreversecalculator(tparameter[tp]);
    reversepolishcalculatorequalizer(tparameter[tp]);
    d=reversepolishcalculator(tparameter[tp]);
    strcpy(tparameter[tp], dtoa(d));
    ++tp; }
    tcommand[strlen(tcommand)-1]=')';
    if (mpos<strlen(tcommand)-1) // still more tcommand when functions have been read
     return tp*-1;
    // reconstruct tcommand
    strcpy(tcommand, functions[function_id].functionPrototype);
    while (operation) {
     operation=0;
     for (i=0;i<functions[function_id].functionParameters;i++) {
      for (n=0;n<strlen(tcommand);n++) {
       if (tcommand[n]==i+97 && tcommand[n-1]=='#') { // find a,b,c,d,e ..MAXFUNCTIONPARAMETERS
        break;
       }
      }
      if (n<strlen(tcommand)) {
       operation=1;
       tcommand[n-1]=' ';
       tcommand[n]=' ';
       inserttextpart(tcommand, tparameter[i], n-1);
      }
     }
    }
    
 return 0;
}

// parse formula for nondefined functions and too long numbers
int parseformulaforerrors(char formula[])
{
  int i,n;

   for (i=0;i<strlen(formula);i++) {
    if (isalpha(formula[i]) && (formula[i+1]=='(' || formula[i+1]=='('))
     return -1; // nondefined function
    if (isdigit(formula[i])/* && !digitflag*/) {
     n=0;
     while ((isdigit(formula[i]) || isdecimalseparator(formula[i])) && i<strlen(formula)) {
      ++n;
     ++i; }
     if (n>MAXOP-1)
   return -2; } } // too long number
    
 return 0;
}
