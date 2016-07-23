
// #define PARMAC_DEBUG_STEPS
// #define PARMAC_DEBUG_CALLBACKS

#include "old2_parmac.h"

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

struct parmac *parmac_set(struct parmac *p,const char *name,const char *src,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *startTrsn,
                          const struct parmac_transition *endTrsn) {
  p->src=src;
  p->prevSrc=src;
  p->state=startState;
  p->startState=startState;
  p->endState=endState;
  p->trsnStart=startTrsn;
  p->trsn=startTrsn;
  p->trsnEnd=endTrsn;
  p->name=name;
  return p;
}

struct parmac *parmac_stack_push(struct parmac *stk,unsigned int *pDepth,
                                 parmac_machine machine) {
  struct parmac *p=&stk[*pDepth];
  struct parmac *p2=p+1; //get next free mem
  machine(p2,p->src);
  (*pDepth)++;
  return p2;
}

struct parmac *parmac_stack_pop(struct parmac *stk,unsigned int *pDepth) {
  assert((*pDepth)!=0);
  struct parmac *p=(&stk[*pDepth])-1;
  (*pDepth)--;
  return p;
}

struct parmac *parmac_stack_next(struct parmac *p) {
  return p+1;
}

struct parmac *parmac_stack_prev(struct parmac *p) {
  return p-1;
}

void parmac_on_state_enter(const char *debug,
                           struct parmac *stk,
                           struct parmac *p,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {

  unsigned int depth=(unsigned int)(p-stk);
  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) enter_%s_%s (<-%s) '%.*s'\n",
         debug,depth,p->name,
         toState->name,fromState?fromState->name:"_",
         (unsigned int)(srcEnd-srcStart),srcStart);
#endif

  if(toState->enter) {
    toState->enter(depth,dif,srcStart,srcEnd,userdata);
  }
}

void parmac_on_state_leave(const char *debug,
                           struct parmac *stk,
                           struct parmac *p,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  unsigned int depth=(unsigned int)(p-stk);
  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) leave_%s_%s (->%s) '%.*s'\n",
         debug,depth,p->name,
         fromState->name,toState?toState->name:"_",
         (unsigned int)(srcEnd-srcStart),srcStart);
#endif

  if(fromState->leave) {
    fromState->leave(depth,dif,srcStart,srcEnd,userdata);
  }
}

void parmac_prev_callbacks(struct parmac *stk,
                           struct parmac *p,
                           void *userdata) {
  //prev states
  struct parmac *p2=p;

  //down
  while(p2!=stk && p2->trsn->fromState==p2->startState) {
    p2=parmac_stack_prev(p2);
  }

  //up
  while(p2!=p+1) {
    if(p2->trsn->fromState==p2->startState) {
      parmac_on_state_enter("a",stk,p2,p2->src,p2->src,userdata,
                            NULL,p2->trsn->fromState);
    }

    parmac_on_state_leave("b",stk,p2,p2->prevSrc,p2->src,userdata,
                          p2->trsn->fromState,p2->trsn->toState);

    p2=parmac_stack_next(p2);
  }
}

void parmac_state_transition(struct parmac *stk,
                             struct parmac *p,
                             const char *src,
                             void *userdata) {

  parmac_on_state_enter("c",stk,p,p->src,src,userdata,
                        p->trsn->fromState,p->trsn->toState);

  //change state
  p->state=p->trsn->toState;
  p->prevSrc=p->src;
  p->src=src;
  p->trsn=p->trsnStart;
}

enum parmac_status parmac_run(struct parmac *stk,
                              unsigned int *pDepth,
                              void *userdata) {

  struct parmac *p=&stk[*pDepth];

  //===
  assert(p->trsn==p->trsnEnd || p->trsn->fromState!=p->endState);
  assert(p->trsn==p->trsnEnd || p->trsn->toState!=p->startState);
  assert(p->trsn==p->trsnEnd || !p->trsn->event || !p->trsn->machine);
  assert(p->startState!=p->endState);

  //===> debug print pos
#ifdef PARMAC_DEBUG_STEPS
  {
    struct parmac *p2=stk;
    printf("\n");

    while(p2!=p+1) {
      const char *f=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->fromState->name;
      const char *t=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->toState->name;
      unsigned int d=(*pDepth)-(unsigned int)(p-p2);
      printf("/ %s : %s (%s -> %s) (d%u t%u)",
             p2->name,p2->state->name,f,t,d,
             (unsigned int)(p2->trsn-p2->trsnStart));
      p2=parmac_stack_next(p2);
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
     (*pDepth)!=0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, not root, pop machine\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave("d",stk,p,p->src,p->src,userdata,
                          p->endState,NULL);

    const char *src2=p->src;
    p=parmac_stack_pop(stk,pDepth);
    parmac_state_transition(stk,p,src2,userdata);

    return parmac_ok;
  }

  //===> at end state, root
  if(p->state==p->endState &&
     (*pDepth)==0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, root, finished\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave("e",stk,p,p->src,p->src,userdata,
                          p->endState,NULL);

    return parmac_done;
  }

  //===> trsnEnd, either not startState or at root
  if(p->trsn==p->trsnEnd &&
     p->state!=p->endState &&
     (p->state!=p->startState ||
      *pDepth==0)) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, fail\n");
    return parmac_error;
  }

  //===> trsnEnd, startState, not root
  if(p->trsn==p->trsnEnd &&
     p->state==p->startState &&
     p->state!=p->endState &&
     *pDepth != 0) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, pop machine\n");

    p=parmac_stack_pop(stk,pDepth);
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

    p=parmac_stack_push(stk,pDepth,p->trsn->machine);

    return parmac_ok;
  }

  //===> on event
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->event &&
     !p->trsn->machine) {

    const char *eventRet=p->trsn->event(p->src,userdata);

    if(eventRet==NULL) {
      PARMAC_DEBUG_STEPS_PRINTF("=event failed\n");

      p->trsn++;
    } else {
      PARMAC_DEBUG_STEPS_PRINTF("=event success\n");
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

      parmac_prev_callbacks(stk,p,userdata);
      parmac_state_transition(stk,p,eventRet,userdata);
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

    parmac_prev_callbacks(stk,p,userdata);
    parmac_state_transition(stk,p,p->src,userdata);

    return parmac_ok;
  }

  //===> shouldn't reach this point
  assert(0);
  return parmac_error;
}
