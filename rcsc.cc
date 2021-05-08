// string formula parser

const char *commands[]={ "mid$(", "left$(", "right$(", "toupper$(", "tolower$(" };

int stringformulacalculator(char formula[MAXSTRING], int record_id)
{
  int i, startpt, endpt, findinstructions=1;
  char ttext[MAXSTRING], tcommand[MAXSTRING];
  strcpy(ttext, formula);

  // replace id references
  replacepartoftextwithcorrespondingvalue(ttext, record_id);
  // locate instruction, calculate, reappend to ttext
  while (findinstructions) {
   findinstructions=0;
   for (i=0;i<5;i++)
    if ((startpt=findsimple(ttext, const_cast <char *> (commands[i]))))
     break;
    if (i<5) {
     findinstructions=1;
     --startpt;
     endpt=startpt;
     while (ttext[endpt]!=')' && endpt<strlen(ttext))
      ++endpt;
     ++endpt;
     extracttextpart(ttext, tcommand, startpt, endpt);
     // parse tcommand
     commandparser(i, tcommand);
     // now insert tcommand into startpt
   inserttextpart(ttext, tcommand, startpt); } }

  strcpy(formula, ttext);
  
 return 1;
}

//replace #id with values from corresponding records
void replacepartoftextwithcorrespondingvalue(char ttext[MAXSTRING], int record_id)
{
  int i, i1, n;
  char transformedtext[MAXSTRING], s[MAXSTRING];

  n=0;
  for (i=0;i<strlen(ttext);i++) {
   if (ttext[i]!='#')
    transformedtext[n++]=ttext[i];
   else {
    i1=0;
    while (isdigit(ttext[++i]) && i<strlen(ttext))
     s[i1++]=ttext[i];
    s[i1]='\0';
    --i;
    if (atoi(s) && atoi(s)<fieldsperrecord && atoi(s)-1!=currentfield)
     strcpy(s, records[(record_id*fieldsperrecord)+atoi(s)-1].text);
     else 
    strcpy(s, " ");
    for (i1=0;i1<strlen(s);i1++)
  transformedtext[n++]=s[i1]; } }
  transformedtext[n]='\0';
  strcpy(ttext, transformedtext);
}

// remove part of text and place in a char array
void extracttextpart(char source[MAXSTRING], char dest[MAXSTRING], int startpt, int endpt)
{
  int i, n=0;
  char ttext[MAXSTRING];
  
   for (i=startpt;i<endpt;i++)
    dest[n++]=source[i];
   dest[n]='\0';
   n=0;
   for (i=0;i<strlen(source);i++)
    if (i<startpt || i>=endpt)
     ttext[n++]=source[i];
   ttext[n]='\0';
   strcpy(source, ttext);
}

// insert text in text
void inserttextpart(char text[MAXSTRING], char part[MAXSTRING], int point)
{
  char ttext[MAXSTRING];
  int i, n=0;
  
   for (i=0;i<point;i++)
    ttext[n++]=text[i];
//    ttext[n++]=SPACE;
   for (i=0;i<strlen(part);i++)
    ttext[n++]=part[i];
//    ttext[n++]=SPACE;
   for (i=point;i<strlen(text);i++) {
    ttext[n++]=text[i];
    if (n>MAXSTRING)
   break; }
   ttext[n]='\0';
   strcpy(text, ttext);; 
}
    
// command parser
int commandparser(int reference, char tcommand[MAXSTRING])
{
  int i, n, pos, endpos, requiredparameters=0, parameters[2];
  char tparameter[10], ttext[MAXSTRING], tttext[MAXSTRING];

  // how many parameters to read  
  if (reference==1 || reference==2) // 0 toupper,tolower, 1 left$,right$
   requiredparameters=1;
  if (!reference)
   requiredparameters=2; // mid$
   
   // go back from end of tcommand
   pos=strlen(tcommand);
   n=requiredparameters;
   while (n && pos)
    if (tcommand[pos--]==',')
     --n;
   if (!pos || n)
    return 0;
   pos+=2; // now pos is at first numerical parameter or space
   if (!requiredparameters)
    pos=strlen(tcommand);
   endpos=pos-1; // use later
   // read parameters
   for (i=0;i<requiredparameters;n=0, i++) {
    while (isdigit(tcommand[pos]) || isspace(tcommand[pos]))
     tparameter[n++]=tcommand[pos++];
    tparameter[n]='\0';
    ++pos;
    parameters[i]=atoi(tparameter); 
    if (!parameters[i])
   return 0; }
   // get string to apply command on
   i=0; n=0;
   while (tcommand[i++]!='(');
   for (;i<endpos;i++)
    ttext[n++]=tcommand[i];
   ttext[n]='\0';
   // apply command on ttext
   n=0;
   switch (reference) {
    case 0: // mid$
     for (i=parameters[0]-1;i<parameters[0]+parameters[1]-1;i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case 1: // left$
     for (i=0;i<parameters[0];i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case 2: // right$
     for (i=strlen(ttext)-parameters[0];i<strlen(ttext);i++)
      tttext[n++]=ttext[i];
     tttext[n]='\0';
    break;
    case 3: // toupper
     for (i=0;i<strlen(ttext);i++)
      tttext[n++]=toupper(ttext[i]);
     tttext[n]='\0';
    break;
    case 4: // tolower
     for (i=0;i<strlen(ttext);i++)
      tttext[n++]=tolower(ttext[i]);
     tttext[n]='\0';
   break; }
   // available char string to transfer to calling routine is tcommand
   strcpy(tcommand, tttext);

 return 1;
}
