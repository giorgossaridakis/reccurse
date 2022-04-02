// reccurse script scriptcommand parser

// definitions
enum { NOCOMMAND=0, NONEXECUTABLE, COMMAND }; // return results
const char *noparametercommands[]= { "push", "copy", "paste", "end", "compare", "greater", "smaller", "clear", "autosave", "quit", "stop", "pass" };
int noparametercommandskeys=12;
enum { PUSHSPACE=0, COPYTEXT, PASTETEXT, ENDLOOP, COMPAREWITHCLIPBOARD, COMPAREWITHCLIPBOARDGREATER, COMPAREWITHCLIPBOARDSMALLER, CLEARVARIABLES, AUTOSAVERECCURSE,  QUITPROGRAM, STOPSCRIPT, PASS };
const char *parametercommands[]= { "file", "record", "field", "enter", "append", "variable", "variabletoclipboard", "delete", "loop", "wait", "goto", "page", "attributes", "color", "sleep", "menu", "key", "keys" };
int parametercommandskeys=18;
enum { FILEOPEN=0, GOTORECORD, GOTOFIELD, ENTERTEXT, APPENDTEXT, VARIABLESET, VARIABLETOCLIPBOARD, CLEARVARIABLE, LOOPFOR, WAITSECS, GOTOLABEL, GOTOPAGE, SETATTRIBUTES, SETCOLOR, SETSLEEPTIME, CHANGEMENU, PUSHKEY, PUSHKEYS };
int scriptrunning=0, scriptsleeptime=250;
char scriptfile[MAXSTRING];


class ScriptLine {
 public:
  char textLine[MAXSTRING];
  ScriptLine(char *s) { strcpy(textLine, s); };
  ScriptLine() { };
~ScriptLine() { };
};

vector<ScriptLine> scriptlines, bscriptlines;

int runline=0;

// parse by line
int commandparser(char scriptcommand[MAXSTRING])
{
  int i, i1, noparameters, commandtorun, thisfield, FAIL=0;
  double d;
  static int fileopen=0;;
  char tline[MAXSTRING], parameter[MAXSTRING];
  int returnvalue=COMMAND;
  
   if (fileopen) {
    ifstream infile(scriptfile);
    scriptlines.clear();
    while (infile) {
     infile.getline(tline, MAXSTRING);
     if (!strlen(tline))
      continue;
     ScriptLine tscriptline(tline);
    scriptlines.push_back(tscriptline); }
    bscriptlines=scriptlines;
    infile.close();
    fileopen=0;
    scriptrunning=1;
   }
   
   // stop script procedures
   if ((runline>=scriptlines.size() && scriptrunning) || !strcmp(scriptcommand, noparametercommands[STOPSCRIPT])) {
    stopscript();
   return NOCOMMAND; }
   if (scriptrunning)
    strcpy(scriptcommand, scriptlines[runline++].textLine);
   thisfield=(currentrecord*fieldsperrecord)+currentfield; // commands only work on currentfield
   
   // split line to scriptcommand and parameter
   i1=noparameters=scantextforcommand(scriptcommand, parameter, SPACE);
   
   // handle labels
   if (islinelabel(scriptcommand))
    return COMMAND;
   
    // variables replacement
    if (strcmp(scriptcommand, parametercommands[VARIABLESET]) && strcmp(scriptcommand, parametercommands[CLEARVARIABLE]))
     for (i=0;i<record.size();i++) 
      if (record[i].type==VARIABLE)
       if (!strcmp(record[i].title, parameter) && strcmp(scriptcommand, parametercommands[VARIABLESET]))
        strcpy(parameter, record[i].automatic_value);
   
   if (!noparameters) {
    for (commandtorun=0;commandtorun<noparametercommandskeys;commandtorun++)
     if (!strcmp(noparametercommands[commandtorun], scriptcommand))
      break;
    switch (commandtorun) {
     case PUSHSPACE:
      pushspaceonfield();
     break;
     case COPYTEXT:
      copytoclipboard();
     break;
     case PASTETEXT:
      pastefromclipboard();
     break;
     case ENDLOOP:
      if (!scriptrunning)
       return NOCOMMAND;
      i1=runline-2; // endloop line-1
      strcpy(scriptcommand, noparametercommands[PASS]);
      while (i1>0 && (strcmp(scriptcommand, parametercommands[LOOPFOR]))) {
       strcpy(scriptcommand, scriptlines[i1].textLine);
       scantextforcommand(scriptcommand, parameter, SPACE);
      --i1; }
      if (i1==0 && (strcmp(scriptcommand, parametercommands[LOOPFOR])))
       stopscript();
      runline=i1+1;
     break;
     case COMPAREWITHCLIPBOARD:
      if (!scriptrunning)
       return NONEXECUTABLE;
       d=atof(clipboard);
      if (record[currentfield].type==NUMERICAL && records[thisfield].number!=d) {
       ++runline; // jump next line
      break; }
      if (record[currentfield].type==MIXEDTYPE && d && d!=records[thisfield].number) {
       ++runline;
      break; }
      if (strcmp(clipboard, records[thisfield].text))
       ++runline;
     break;
     case COMPAREWITHCLIPBOARDGREATER:
      if (!scriptrunning)
       return NONEXECUTABLE;
       d=atof(clipboard);
      if ((record[currentfield].type!=NUMERICAL && record[currentfield].type!=MIXEDTYPE) || !d) {
       ++runline;
      break; }
      if (records[thisfield].number<=d)
        ++runline;
     break;
     case COMPAREWITHCLIPBOARDSMALLER:
      if (!scriptrunning)
       return NONEXECUTABLE;
       d=atof(clipboard);
      if ((record[currentfield].type!=NUMERICAL && record[currentfield].type!=MIXEDTYPE) || !d) {
       ++runline;
      break; }
       if (records[thisfield].number>=d)
        ++runline;
     break;
     case AUTOSAVERECCURSE:
      toggleautosave();
     break;
     case CLEARVARIABLES:
      for (i1=0;i1<record.size();i1++)
       if (record[i1].type==VARIABLE)
        Delete_Field(i1);
     break;
     case QUITPROGRAM:
      End_Program(0);
     break;
     case STOPSCRIPT:
      // covered above
     break;
     case PASS:
      // do nothing
     break;
     default:
      if (scriptcommand[strlen(scriptcommand)-1]==':') // label
       break;
      if (scriptrunning)
       return NONEXECUTABLE;
      return NOCOMMAND;
    break; }
   }
   else { // parameter scriptcommand
    for (commandtorun=0;commandtorun<parametercommandskeys;commandtorun++)
     if (!strcmp(parametercommands[commandtorun], scriptcommand))
      break;
    switch (commandtorun) {
     case FILEOPEN:
      if (!tryfile(parameter))
       return NONEXECUTABLE;
      stopscript();
      fileopen=1;
      strcpy(scriptfile, parameter);
     break;
     case GOTORECORD:
      i1=atoi(parameter)-1;
      if (!strcmp(parameter, "+"))
       i1=currentrecord+1;
      if (!strcmp(parameter, "-"))
       i1=currentrecord-1;
      if (i1<0 || i1>recordsnumber)
       returnvalue=FAIL;
      else
       currentrecord=i1;
     break;
     case GOTOFIELD:
      i1=atoi(parameter)-1;
      if (!strcmp(parameter, "+"))
       i1=currentfield+1;
      if (!strcmp(parameter, "-"))
       i1=currentfield-1;
      if (i1<0 || i1>fieldsperrecord-1)
       returnvalue=FAIL;
      else
      currentfield=i1;
     break;
     case ENTERTEXT:
      if (strcmp(record[currentfield].automatic_value, EMPTYSTRING) || !record[currentfield].editable)
       returnvalue=FAIL;
      else
       strcpy(records[thisfield].text, parameter);
      if (autosave)
       Read_Write_Field(records[thisfield], fieldposition(currentrecord, currentfield), 1);
     break;
     case APPENDTEXT:
      if (strcmp(record[currentfield].automatic_value, EMPTYSTRING) || !record[currentfield].editable)
       returnvalue=FAIL;
      else
       strcat(records[thisfield].text, parameter);
      if (autosave)
       Read_Write_Field(records[thisfield], fieldposition(currentrecord, currentfield), 1);
     break;
     case VARIABLESET:   
      i1=setvariablefromfield(parameter, currentfield);
      // place values in clipboard
      strcpy(clipboard, record[i1].automatic_value);
     break;
     case VARIABLETOCLIPBOARD:
      for (i1=0;i1<record.size();i1++)
       if (!strcmp(record[i1].title, parameter))
        break;
      if (i1==record.size())
       returnvalue=FAIL;
      else
      strcpy(clipboard, record[i1].automatic_value);
     break;
     case CLEARVARIABLE:
      if ((i1=isvariable(parameter))==0)
       returnvalue=FAIL;
      else
      Delete_Field(i1-1);
     break;
     case LOOPFOR:
      if (!scriptrunning)
       return NOCOMMAND;
      i1=atoi(parameter);
      if (i1<1) { // loop has reached 0
       strcpy(scriptlines[runline-1].textLine, noparametercommands[PASS]);
       while ((strcmp(scriptlines[runline].textLine, noparametercommands[ENDLOOP])) && runline<scriptlines.size())
        ++runline;
       if (runline<scriptlines.size())
        strcpy(scriptlines[runline].textLine, noparametercommands[PASS]);
      break;
      }
      sprintf(scriptlines[runline-1].textLine, "%s %d", parametercommands[LOOPFOR], i1-1);
     break;
     case WAITSECS:
      i1=atoi(parameter);
      if (i1>0)
       returnvalue=FAIL;
      else
       sleep(atoi(parameter));
     break;
     case GOTOLABEL:
      if ((i1=labelposition(parameter))==0)
       returnvalue=FAIL;
      else
       scriptlines=bscriptlines; // restore lines for loop adjustments
      runline=i1;
     break;
     case GOTOPAGE:
      i1=atoi(parameter)-1;
      if (!strcmp(parameter, "+"))
       i1=currentpage+1;
      if (!strcmp(parameter, "-"))
       i1=currentpage-1;
      if (i1<0 || i1>pagesnumber-1)
       return NOCOMMAND; // will fail in script as well
      Pages_Selector(i1);
     break;
     case SETATTRIBUTES:
      if (strlen(parameter)!=9)
       returnvalue=FAIL; {
      break; }
      for (i1=0;i1<9;i1++)
       if (parameter[i1]!='0' && parameter[i1]!='1')
        break;
      if (i1<9)
       returnvalue=FAIL;
      else
       strcpy(record[currentfield].attributes, parameter);
     break;
     case SETCOLOR:
      i1=atoi(parameter);
      if (i1<1 || i1>58)
       returnvalue=FAIL;
      else
       record[currentfield].color=i1;
     break;
     case SETSLEEPTIME:
      i1=atoi(parameter);
      if (!i1)
       returnvalue=FAIL;
      else
       scriptsleeptime=i1;
     break;
     case CHANGEMENU:
      i1=atoi(parameter);
      if (i1<0 || i1>5 || i1==4)
       returnvalue=FAIL;
      else
       currentmenu=i1;
     break;
     case PUSHKEY:
      if ((i1=decimatestringtokey(parameter))==0)
       returnvalue=FAIL;
      else
       keyonnextloop.push_back(i1);
     break;
     case PUSHKEYS:
      breaktexttokeys(parameter);
     break;
     default:
      if (scriptrunning)
       return NONEXECUTABLE;
      return NOCOMMAND;
    break; }
   }
   
   strcpy(scriptcommand, " ");

   if (returnvalue==FAIL)
    returnvalue=(scriptrunning) ? NONEXECUTABLE: NOCOMMAND;
   
 return returnvalue;
}

// locate label
int labelposition(char *label)
{
  int i, i1;
  char tlabel[MAXSTRING];
  strcpy(tlabel, label);
  strcat(tlabel, ":");
  
   // find label
   for (i=0;i<scriptlines.size();i++)
    if (!strcmp(tlabel, scriptlines[i].textLine))
     break;
   // duplicate ?
   for (i1=i+1;i1<scriptlines.size();i1++)
    if (!strcmp(tlabel, scriptlines[i1].textLine))
    break;
    
   if (i==scriptlines.size() || i1<scriptlines.size())
    return 0;
  
 return i;
}

// is scriptcommand a label
int islinelabel(char *scriptcommand)
{
  if (scriptcommand[strlen(scriptcommand)-1]==':')
   return 1;
  
 return 0;
}

// set variable from field
int setvariablefromfield(char *parameter, int field_id)
{
  int i;
  int thisfield=(currentrecord*fieldsperrecord)+field_id;
  
   i=isvariable(parameter);
   
   if (i) // variable exists
    strcpy(record[i-1].automatic_value, records[thisfield].text);
   else
    Add_Field(VARIABLE, parameter, records[thisfield].text);
   
 return i;
}

// stop script
void stopscript()
{
    scriptlines.clear();
    runline=scriptrunning=0;
}

// return variable field id
int isvariable(char *parameter)
{
  int i;
  
   for (i=0;i<record.size();i++)
    if (record[i].type==VARIABLE)
     if (!strcmp(record[i].title, parameter))
      break;
    
 return (i==record.size()) ? 0 : i+1;
}
