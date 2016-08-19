#include "../parmac.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

struct csv_parser_data {
  const char *src,*errMsg;
  unsigned int colsCount,colsTotalCount;
};

void print_singlequote(PARMAC_DEPTH stkDepth,
                       const char *machine,
                       const char *fromState,
                       const char *toState,
                       PARMAC_POS fromPos,
                       PARMAC_POS toPos,
                       void *userdata) {
  printf("'");
}

void print_quote(PARMAC_DEPTH stkDepth,
                 const char *machine,
                 const char *fromState,
                 const char *toState,
                 PARMAC_POS fromPos,
                 PARMAC_POS toPos,
                 void *userdata) {
  printf("\"");
}

void print_char(PARMAC_DEPTH stkDepth,
                const char *machine,
                const char *fromState,
                const char *toState,
                PARMAC_POS fromPos,
                PARMAC_POS toPos,
                void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;
  printf("%.*s",(int)(toPos-fromPos),&pd->src[fromPos]);

}

void print_comma(PARMAC_DEPTH stkDepth,
                 const char *machine,
                 const char *fromState,
                 const char *toState,
                 PARMAC_POS fromPos,
                 PARMAC_POS toPos,
                 void *userdata) {
  printf(", ");
}

void print_eol(PARMAC_DEPTH stkDepth,
               const char *machine,
               const char *fromState,
               const char *toState,
               PARMAC_POS fromPos,
               PARMAC_POS toPos,
               void *userdata) {
  printf("\n");
}

void countCol(PARMAC_DEPTH stkDepth,
              const char *machine,
              const char *fromState,
              const char *toState,
              PARMAC_POS fromPos,
              PARMAC_POS toPos,
              void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  pd->colsCount++;

}

void setTotalColCount(PARMAC_DEPTH stkDepth,
                      const char *machine,
                      const char *fromState,
                      const char *toState,
                      PARMAC_POS fromPos,
                      PARMAC_POS toPos,
                      void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  pd->colsTotalCount=pd->colsCount;
}


void zeroColCount(PARMAC_DEPTH stkDepth,
                  const char *machine,
                  const char *fromState,
                  const char *toState,
                  PARMAC_POS fromPos,
                  PARMAC_POS toPos,
                  void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;
  pd->colsCount=0;
}

bool parse_schar(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]==',' ||
     pd->src[*ppos]=='\n' ||
     pd->src[*ppos]=='\0' ||
     (pd->src[*ppos]=='\r' &&
      pd->src[*ppos+1]=='\n')) {
    return false;
  }

  (*ppos)++;
  return true;
}

bool parse_twoquotes(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]=='"' &&
     pd->src[*ppos+1]=='"') {
    (*ppos)+=2;
    return true;
  }

  return false;
}

bool parse_qchar(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]=='\0' ||
     pd->src[*ppos]=='"') {
    return false;
  }

  (*ppos)++;
  return true;
}

bool parse_lquote(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]!='"') {
    return false;
  }

  (*ppos)++;
  return true;
}

bool parse_rquote(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]!='"') {
    pd->errMsg="Expected closing double quote";
    return false;
  }

  (*ppos)++;
  return true;
}

bool parse_comma(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]!=',') {
    return false;
  }

  (*ppos)++;
  return true;
}

bool parse_eol(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->src[*ppos]=='\r' && pd->src[*ppos+1]=='\n') {
    (*ppos)+=2;
    return true;
  } else if(pd->src[*ppos]=='\n') {
    (*ppos)++;
    return true;
  }

  return false;
}

bool colcheck(PARMAC_POS *ppos,void *userdata) {
  struct csv_parser_data *pd=(struct csv_parser_data*)userdata;

  if(pd->colsCount==pd->colsTotalCount) {
    return true;
  }

  pd->errMsg="Inconsistent columns count";

  return false;
}

void qstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start"},
    state_lquote={"lquote",.event=parse_lquote},
    state_rquote={"rquote",.event=parse_rquote},
    state_twoquotes={"twoquotes",.event=parse_twoquotes,.enter=print_quote},
    state_qchar={"qchar",.event=parse_qchar,.enter=print_char},
    state_end={"end"};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote},
    {&state_lquote, &state_twoquotes},
    {&state_lquote, &state_qchar},
    {&state_lquote, &state_rquote},
    {&state_qchar, &state_twoquotes},
    {&state_qchar, &state_qchar},
    {&state_qchar, &state_rquote},
    {&state_twoquotes, &state_twoquotes},
    {&state_twoquotes, &state_qchar},
    {&state_twoquotes, &state_rquote},
    {&state_rquote, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"qstr",&state_start,&state_end,trsns);
}

void sstr_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start"},
    state_schar={"schar",.event=parse_schar,.enter=print_char},
    state_end={"end"};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_schar},
    {&state_start, &state_end},
    {&state_schar, &state_schar},
    {&state_schar, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"sstr",&state_start,&state_end,trsns);
}

void field_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",.enter=print_singlequote},
    state_qstr={"qstr",.machine=qstr_machine},
    state_sstr={"sstr",.machine=sstr_machine},
    state_end={"end",.enter=print_singlequote};

   static const struct parmac_transition trsns[]={
     {&state_start, &state_qstr},
     {&state_start, &state_sstr},
     {&state_qstr, &state_end},
     {&state_sstr, &state_end},
     PARMAC_TRANSITION_END};

   parmac_set(p,pos,"field",&state_start,&state_end,trsns);
}

void record_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start",.leave=zeroColCount},
    state_field={"field",.machine=field_machine,.leave=countCol},
    state_comma={"comma",.event=parse_comma,.enter=print_comma},
    state_end={"end",.enter=print_eol};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_field},
    {&state_field, &state_comma},
    {&state_field, &state_end},
    {&state_comma, &state_field},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"record",&state_start,&state_end,trsns);
}

void main_machine(struct parmac *p,PARMAC_POS pos) {
  static const struct parmac_state
    state_start={"start"},
    state_record1={"record1",.machine=record_machine,.leave=setTotalColCount},
    state_recordn={"recordn",.machine=record_machine},
    state_eol={"eol",.event=parse_eol},
    state_colcheck={"colcheck",.event=colcheck},
    state_end={"end"};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_record1},
    {&state_record1, &state_eol},
    {&state_record1, &state_end},
    {&state_eol, &state_recordn},
    {&state_recordn, &state_colcheck},
    {&state_colcheck, &state_eol},
    {&state_colcheck, &state_end},
    PARMAC_TRANSITION_END};

  parmac_set(p,pos,"main",&state_start,&state_end,trsns);
}

void countRowCol(const char *src,const char*srcTo,int *row,int *col,
                 int *rowStart,int *rowLen) {
  const char *it=src;

  *row=0;
  *col=0;
  *rowStart=0;
  *rowLen=0;

  while(it!=srcTo) {
    if(it[0]=='\n') {
      (*row)++;
      (*col)=0;
      (*rowStart)=(int)(it-src)+1;
    } else if(it[0]!='\r') {
      (*col)++;
    }

    it++;
  }

  *rowLen=*col;

  while(it[0]!='\0' && it[0]!='\n') {
    if(it[0]!='\r') {
      (*rowLen)++;
    }

    it++;
  }
}

void printSyntaxError(const char *src,const char *srcPos,const char *errMsg) {
  int row,col,rowStart,rowLen;
  countRowCol(src,srcPos,&row,&col,&rowStart,&rowLen);

  //
  printf("\n");

  //
  printf("'%.*s'\n",rowLen,&src[rowStart]);

   int i;

  for(i=0;i<col+1;i++) {
    printf(" ");
  }

  printf("^\n");

  //
  printf("Error: %s at pos %i, line %i, col %i.\n",
         errMsg?errMsg:"Unexpected syntax",
         (int)(srcPos-src),row+1,col+1);

}

void parse_csv(const char *src) {
  struct csv_parser_data parserData;

  PARMAC_DEPTH stkDepth;
  struct parmac stk[4];

  parserData.src=src;
  parserData.errMsg=NULL;
  parserData.colsCount=0;
  parserData.colsTotalCount=0;

  stkDepth=0;
  main_machine(stk,0);

  while(parmac_run(stk,&stkDepth,&parserData)) {

  }

  if(parmac_failed(stk) || src[parmac_last_pos(stk,stkDepth)]!='\0') {
    printSyntaxError(src,&src[parmac_last_pos(stk,stkDepth)],parserData.errMsg);
  } else {
    printf("parse success!\n");
  }
}

char *string_from_file(const char *fn) {
  FILE *file;
  unsigned int dataSize;
  char *str;

  if(!(file = fopen(fn, "rb"))) {
    return 0;
  }

  fseek(file,0L,SEEK_END);
  dataSize = ftell(file);
  fseek(file,0L,SEEK_SET);
  str=(char*)malloc(dataSize+1);
  str[dataSize]='\0';
  fread(str,1,dataSize,file);
  fclose(file);

  return str;
}

int main() {
  char *src;

  if(!(src=string_from_file("test.csv"))) {
    return 1;
  }

  parse_csv(src);

  free(src);
  return 0;
}
