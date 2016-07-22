
// #define PARMAC_DEBUG_STEPS
// #define PARMAC_DEBUG_CALLBACKS

#include "parmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#ifdef PARMAC_DEBUG_STEPS
#define PARMAC_DEBUG_STEPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define PARMAC_DEBUG_STEPS_PRINTF(...)
#endif

#ifdef PARMAC_DEBUG_CALLBACKS
#define PARMAC_DEBUG_CALLBACKS_PRINTF(...) printf(__VA_ARGS__)
#else
#define PARMAC_DEBUG_CALLBACKS_PRINTF(...)
#endif

struct parmac *parmac_set(struct parmac *p,const char *name,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *startTrsn,
                          const struct parmac_transition *endTrsn) {
  p->pos=0; //p->src=src
  p->prevPos=0; //p->prevSrc=src
  p->state=startState;
  p->startState=startState;
  p->endState=endState;
  p->trsnStart=startTrsn;
  p->trsn=startTrsn;
  p->trsnEnd=endTrsn;
  p->name=name;
  return p;
}

struct parmac *parmac_stack_push(struct parmac *stk,unsigned int *pStkDepth,
                                 parmac_machine machine) {
  struct parmac *p=&stk[*pStkDepth];
  struct parmac *p2=&stk[(*pStkDepth)+1]; //get next free mem

  machine(p2); //p2,p->src
  p2->pos=p->pos;
  p2->prevPos=p->pos;

  //
  (*pStkDepth)++;
  return p2;
}

struct parmac *parmac_stack_pop(struct parmac *stk,unsigned int *pStkDepth) {
  assert((*pStkDepth)!=0);
  struct parmac *p=(&stk[*pStkDepth])-1;
  (*pStkDepth)--;
  return p;
}

void parmac_on_state_enter(const char *debug,
                           struct parmac *stk,
                           unsigned int stkDepth,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  struct parmac *p=&stk[stkDepth];

  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) enter_%s_%s (<-%s) '%.*s'\n",
         debug,stkDepth,p->name,
         toState->name,fromState?fromState->name:"_",
         (unsigned int)(srcEnd-srcStart),srcStart);
#endif

  if(toState->enter) {
    toState->enter(stkDepth,dif,srcStart,srcEnd,userdata);
  }
}

void parmac_on_state_leave(const char *debug,
                           struct parmac *stk,
                           unsigned int stkDepth,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  struct parmac *p=&stk[stkDepth];

  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) leave_%s_%s (->%s) '%.*s'\n",
         debug,stkDepth,p->name,
         fromState->name,toState?toState->name:"_",
         (unsigned int)(srcEnd-srcStart),srcStart);
#endif

  if(fromState->leave) {
    fromState->leave(stkDepth,dif,srcStart,srcEnd,userdata);
  }
}

void parmac_prev_callbacks(struct parmac *stk,
                           unsigned int stkDepth,
                           const char *src,
                           void *userdata) {
  unsigned int d;

  //down
  for(d=stkDepth;d!=0 && stk[d].trsn->fromState==stk[d].startState;d--) {
  }

  //up
  for(;d<=stkDepth;d++) {
    if(stk[d].trsn->fromState==stk[d].startState) {
      parmac_on_state_enter("a",stk,d,
                            &src[stk[d].pos], //p2->src
                            &src[stk[d].pos], //p2->src
                            userdata,
                            NULL,
                            stk[d].trsn->fromState);
    }

    parmac_on_state_leave("b",stk,d,
                          &src[stk[d].prevPos], //p2->prevSrc
                          &src[stk[d].pos], //p2->src,
                          userdata,
                          stk[d].trsn->fromState,
                          stk[d].trsn->toState);
  }
}

void parmac_state_transition(struct parmac *stk,
                             unsigned int stkDepth,
                             const char *src,
                             unsigned int pos,
                             void *userdata) {

  struct parmac *p=&stk[stkDepth];

  parmac_on_state_enter("c",stk,stkDepth,
                        &src[p->pos], //p->src
                        &src[pos], //src,
                        userdata,
                        p->trsn->fromState,p->trsn->toState);

  //change state
  p->state=p->trsn->toState;
  p->prevPos=p->pos; //p->prevSrc=p->src
  p->pos=pos; //p->src=src
  p->trsn=p->trsnStart;
}

enum parmac_status parmac_run(struct parmac *stk,
                              unsigned int *pStkDepth,
                              const char *src,
                              void *userdata) {

  struct parmac *p=&stk[*pStkDepth];

  //===
  assert(p->trsn==p->trsnEnd || p->trsn->fromState!=p->endState);
  assert(p->trsn==p->trsnEnd || p->trsn->toState!=p->startState);
  assert(p->trsn==p->trsnEnd || !p->trsn->event || !p->trsn->machine);
  assert(p->startState!=p->endState);

  //===> debug print pos
#ifdef PARMAC_DEBUG_STEPS
  {
    printf("\n");

    unsigned int d;

    for(d=0;d<=*pStkDepth;d++) {
      struct parmac *p2=&stk[d];
      const char *from=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->fromState->name;
      const char *to=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->toState->name;
      unsigned int stkDepth=(*pStkDepth)-(unsigned int)(p-p2);
      unsigned int trsn=(unsigned int)(p2->trsn-p2->trsnStart);
      printf("/ %s : %s (%s -> %s) (d%u p%u t%u)",
             p2->name,p2->state->name,from,to,stkDepth,p2->pos,trsn);
    }



    printf("\n");
  }
#endif

  //===> transition fromState doesn't match state
  if(p->trsn!=p->trsnEnd &&
     p->state!=p->trsn->fromState &&
     p->state!=p->endState) {
    PARMAC_DEBUG_STEPS_PRINTF("=iterating trsns\n");

    while(p->trsn!=p->trsnEnd && p->state!=p->trsn->fromState) {
      p->trsn++;
    }

    return parmac_ok;
  }

  //===> at end state, not root
  if(p->state==p->endState &&
     (*pStkDepth)!=0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, not root, pop machine\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave("d",stk,*pStkDepth,
                          &src[p->pos],&src[p->pos], //p->src,p->src,
                          userdata,
                          p->endState,NULL);

    unsigned int pos2=p->pos; //p->src
    p=parmac_stack_pop(stk,pStkDepth);

    parmac_state_transition(stk,*pStkDepth,src,
                            pos2, //src2
                            userdata);

    return parmac_ok;
  }

  //===> at end state, root
  if(p->state==p->endState &&
     (*pStkDepth)==0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, root, finished\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave("e",stk,*pStkDepth,
                          &src[p->pos], //p->src
                          &src[p->pos], //p->src,
                          userdata,
                          p->endState,NULL);

    return parmac_done;
  }

  //===> trsnEnd, either not startState or at root
  if(p->trsn==p->trsnEnd &&
     p->state!=p->endState &&
     (p->state!=p->startState ||
      *pStkDepth==0)) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, fail\n");
    return parmac_error;
  }

  //===> trsnEnd, startState, not root
  if(p->trsn==p->trsnEnd &&
     p->state==p->startState &&
     p->state!=p->endState &&
     *pStkDepth != 0) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, pop machine\n");

    p=parmac_stack_pop(stk,pStkDepth);
    p->trsn++;

    return parmac_ok;
  }

  //===> on machine
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->machine &&
     !p->trsn->event) {

    PARMAC_DEBUG_STEPS_PRINTF("=pushed machine\n");

    p=parmac_stack_push(stk,pStkDepth,p->trsn->machine);

    return parmac_ok;
  }

  //===> on event
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->event &&
     !p->trsn->machine) {

    const char *eventRet=p->trsn->event(&src[p->pos], //p->src
                                        userdata);

    if(eventRet==NULL) {
      PARMAC_DEBUG_STEPS_PRINTF("=event failed\n");

      p->trsn++;
    } else {
      PARMAC_DEBUG_STEPS_PRINTF("=event success\n");
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

      parmac_prev_callbacks(stk,*pStkDepth,src,userdata);

      parmac_state_transition(stk,*pStkDepth,src,
                              eventRet-src,//eventRet
                              userdata);
    }

    return parmac_ok;
  }

  //===> on no machine or event
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     !p->trsn->event &&
     !p->trsn->machine) {

    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_prev_callbacks(stk,*pStkDepth,src,userdata);

    parmac_state_transition(stk,*pStkDepth,src,
                            p->pos,//p->src
                            userdata);

    return parmac_ok;
  }

  //===> shouldn't reach this point
  assert(0);
  return parmac_error;
}
