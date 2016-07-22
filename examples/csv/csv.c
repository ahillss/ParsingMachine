#include "../../parmac.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define endof(x) (x+sizeof(x)/sizeof(*x))

struct csv_parser_data {
  const char *errMsg;
  unsigned int colsCount,colsTotalCount;
};

void print_singlequote(unsigned int stkDepth,bool dif,
                 const char *start,const char *end,
                 void *userdata) {
  printf("'");
}

void print_quote(unsigned int stkDepth,bool dif,
                    const char *start,const char *end,
                    void *userdata) {
  printf("\"");
}

void print_char(unsigned int stkDepth,bool dif,
                const char *start,const char *end,
                void *userdata) {
  printf("%.*s",(int)(end-start),start);

}

void print_comma(unsigned int stkDepth,bool dif,
              const char *start,const char *end,
              void *userdata) {
  printf(", ");
}

void print_eol(unsigned int stkDepth,bool dif,
               const char *start,const char *end,
               void *userdata) {
  printf("\n");
}

void on_field(unsigned int stkDepth,bool dif,
              const char *start,const char *end,
              void *userdata) {
  struct csv_parser_data *parserData=(struct csv_parser_data*)userdata;
  parserData->colsCount++;
}

void on_record1(unsigned int stkDepth,bool dif,
               const char *start,const char *end,
               void *userdata) {
  struct csv_parser_data *parserData=(struct csv_parser_data*)userdata;
  parserData->colsTotalCount=parserData->colsCount;
  parserData->colsCount=0;
}

void on_recordn(unsigned int stkDepth,bool dif,
               const char *start,const char *end,
               void *userdata) {
  struct csv_parser_data *parserData=(struct csv_parser_data*)userdata;
  parserData->colsCount=0;
}

const char *parse_schar(const char *src,void *userdata) {
  if(src[0]==',' || src[0]=='\n' || src[0]=='\0' ||
     (src[0]=='\r' && src[1]=='\n')) {
    return NULL;
  }

  return src+1;
}

const char *parse_twoquotes(const char *src,void *userdata) {
  if(src[0]=='"' && src[1]=='"') {
    return src+2;
  }

  return NULL;
}

const char *parse_qchar(const char *src,void *userdata) {
  if(src[0]=='\0' || src[0]=='"') {
    return NULL;
  }

  return src+1;
}

const char *parse_lquote(const char *src,void *userdata) {
  if(src[0]!='"') {
    return NULL;
  }

  return src+1;
}

const char *parse_rquote(const char *src,void *userdata) {
  struct csv_parser_data *parserData=(struct csv_parser_data*)userdata;

  if(src[0]!='"') {
    parserData->errMsg="Expected closing double quote";
    return NULL;
  }

  return src+1;
}

const char *parse_comma(const char *src,void *userdata) {
  if(src[0]!=',') {
    return NULL;
  }

  return src+1;
}

const char *parse_eol(const char *src,void *userdata) {
  if(src[0]=='\r' && src[1]=='\n') {
    return src+2;
  } else if(src[0]=='\n') {
    return src+1;
  }

  return NULL;
}

const char *colcheck(const char *src,void *userdata) {
  struct csv_parser_data *parserData=(struct csv_parser_data*)userdata;

  if(parserData->colsCount==parserData->colsTotalCount) {
    return src;
  }

  parserData->errMsg="Inconsistent columns count";

  return NULL;
}

void qstr_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_lquote={"lquote",NULL,NULL},
    state_rquote={"rquote",NULL,NULL},
    state_twoquotes={"twoquotes",NULL,print_quote},
    state_qchar={"qchar",NULL,print_char},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_lquote, parse_lquote, NULL},

    {&state_lquote, &state_twoquotes, parse_twoquotes, NULL},
    {&state_lquote, &state_qchar, parse_qchar, NULL},
    {&state_lquote, &state_rquote, parse_rquote, NULL},

    {&state_qchar, &state_twoquotes, parse_twoquotes, NULL},
    {&state_qchar, &state_qchar, parse_qchar, NULL},
    {&state_qchar, &state_rquote, parse_rquote, NULL},

    {&state_twoquotes, &state_twoquotes, parse_twoquotes, NULL},
    {&state_twoquotes, &state_qchar, parse_qchar, NULL},
    {&state_twoquotes, &state_rquote, parse_rquote, NULL},

    {&state_rquote, &state_end, NULL, NULL},
  };

  parmac_set(p,"qstr",&state_start,&state_end,trsns,endof(trsns));
}

void sstr_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_schar={"schar",NULL,print_char},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_schar, parse_schar, NULL},
    {&state_start, &state_end, NULL, NULL},

    {&state_schar, &state_schar, parse_schar, NULL},
    {&state_schar, &state_end, NULL, NULL},
  };

  parmac_set(p,"sstr",&state_start,&state_end,trsns,endof(trsns));
}

void field_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,print_singlequote},
    state_qstr={"qstr",NULL,NULL},
    state_sstr={"sstr",NULL,NULL},
    state_end={"end",NULL,print_singlequote};

   static const struct parmac_transition trsns[]={
     {&state_start, &state_qstr, NULL, qstr_machine},
     {&state_start, &state_sstr, NULL, sstr_machine},

     {&state_qstr, &state_end, NULL, NULL},
     {&state_sstr, &state_end, NULL, NULL},
   };

   parmac_set(p,"field",&state_start,&state_end,trsns,endof(trsns));
}

void record_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_field={"field",NULL,on_field},
    state_comma={"comma",NULL,print_comma},
    state_end={"end",NULL,print_eol};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_field, NULL, field_machine},

    {&state_field, &state_comma, parse_comma, NULL},
    {&state_field, &state_end,   NULL,        NULL},

    {&state_comma, &state_field, NULL, field_machine}
  };

  parmac_set(p,"record",&state_start,&state_end,trsns,endof(trsns));
}

void main_machine(struct parmac *p) {
  static const struct parmac_state
    state_start={"start",NULL,NULL},
    state_record1={"record1",NULL,on_record1},
    state_recordn={"recordn",NULL,on_recordn},
    state_eol={"eol",NULL,NULL},
    state_colcheck={"colcheck",NULL,NULL},
    state_end={"end",NULL,NULL};

  static const struct parmac_transition trsns[]={
    {&state_start, &state_record1, NULL, record_machine},

    {&state_record1, &state_eol, parse_eol, NULL},
    {&state_record1, &state_end, NULL,      NULL},

    {&state_eol, &state_recordn, NULL, record_machine},

    {&state_recordn, &state_colcheck, colcheck, NULL},

    {&state_colcheck, &state_eol, parse_eol, NULL},
    {&state_colcheck, &state_end, NULL,      NULL},
  };

  parmac_set(p,"main",&state_start,&state_end,trsns,endof(trsns));
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

  unsigned int stkDepth;
  struct parmac stk[4];
  enum parmac_status status;

  parserData.errMsg=NULL;
  parserData.colsCount=0;
  parserData.colsTotalCount=0;

  stkDepth=0;
  main_machine(stk);

  while((status=parmac_run(stk,&stkDepth,src,&parserData))==parmac_ok) {
  }

  if(status==parmac_error || src[stk[stkDepth].pos]!='\0') {
    printSyntaxError(src,&src[stk[stkDepth].pos],parserData.errMsg);
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
