
// #define PARMAC_DEBUG_STEPS
#define PARMAC_DEBUG_CALLBACKS

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

void parmac_on_state_enter(struct parmac *stk,
                           struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const char *debug) {

  unsigned int depth=(unsigned int)(p-stk);
  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) enter_%s_%s (<-%s) '%.*s'\n",
         debug,depth,p->name,
         toState->name,fromState?fromState->name:"_",
         srcEnd-srcStart,srcStart);
#endif

  if(toState->enter) {
    toState->enter(depth,dif,srcStart,srcEnd,userdata);
  }
}

void parmac_on_state_leave(struct parmac *stk,
                           struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *userdata,
                           const char *debug) {
  unsigned int depth=(unsigned int)(p-stk);
  bool dif=fromState!=toState;

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : (%u) leave_%s_%s (->%s) '%.*s'\n",
         debug,depth,p->name,
         fromState->name,toState?toState->name:"_",
         srcEnd-srcStart,srcStart);
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

  //down chain of 'from start states'
  while(p2!=stk && p2->trsn->fromState==p2->startState) {//p2->prev/p->depth
    p2=parmac_stack_prev(p2);
  }

  //from bottom to top
  while(p2!=p+1) {
    //start state, on enter
    if(p2->trsn->fromState==p2->startState) {
      parmac_on_state_enter(stk,p2,NULL,p2->trsn->fromState,
                            p2->src,p2->src,userdata,"a");
    }

    //on leave
    parmac_on_state_leave(stk,p2,p2->trsn->fromState,p2->trsn->toState,
                          p2->prevSrc,p2->src,userdata,"b");

    //
    p2=parmac_stack_next(p2);
  }
}

void parmac_state_transition(struct parmac *stk,
                             struct parmac *p,
                             const char *src,
                             void *userdata) {

  //cur state, on enter
  parmac_on_state_enter(stk,p,p->trsn->fromState,p->trsn->toState,
                        p->src,src,userdata,"c");

  //change state
  p->state=p->trsn->toState;
  p->prevSrc=p->src;
  p->src=src;
  p->trsn=p->trsnStart;
}

bool parmac_run(struct parmac *stk,unsigned int *pDepth,
                void *userdata,bool *err) {
  *err=false;
  struct parmac *p=&stk[*pDepth];

  //===
  assert(p->trsn==p->trsnEnd || p->trsn->fromState!=p->endState);
  assert(p->trsn==p->trsnEnd || p->trsn->toState!=p->startState);
  assert(p->trsn==p->trsnEnd || !p->trsn->event || !p->trsn->machine);
  assert(p->startState!=p->endState);

  //===> transition fromState doesn't match state
  if(p->trsn!=p->trsnEnd &&
     p->state!=p->trsn->fromState &&
     p->state!=p->endState) {
    p->trsn++;
    return true;
  }

  //debug print pos
#ifdef PARMAC_DEBUG_STEPS
  {
    struct parmac *p2=stk;
    printf("\n");

    while(p2!=p+1) {
      const char *f=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->fromState->name;
      const char *t=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->toState->name;
      unsigned int d=(*pDepth)-(unsigned int)(p-p2);
      printf("/(%i) %s : %s (%s -> %s)",d,p2->name,p2->state->name,f,t);
      p2=parmac_stack_next(p2);
    }

    printf("\n");
  }
#endif

  //===> at end state, not root
  if(p->state==p->endState &&
     (*pDepth)!=0) {
    PARMAC_DEBUG_STEPS_PRINTF("=at end, not root\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    //end state, on leave
    parmac_on_state_leave(stk,p,p->endState,NULL,
                          p->src,p->src,userdata,"d");

    //
    const char *src2=p->src;

    p=parmac_stack_pop(stk,pDepth);

    parmac_state_transition(stk,p,src2,userdata);

    //
    return true;
  }

  //===> at end state, root
  if(p->state==p->endState &&
     (*pDepth)==0) {

    PARMAC_DEBUG_STEPS_PRINTF("=at end, root\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    //end state, on leave
    parmac_on_state_leave(stk,p,p->endState,NULL,
                          p->src,p->src,userdata,"e");

    //
    return false;
  }

  //===>trsnEnd, either not startState or at root
  if(p->trsn==p->trsnEnd &&
     p->state!=p->endState &&
     (p->state!=p->startState || *pDepth==0)) {
    PARMAC_DEBUG_STEPS_PRINTF("=err no trsns\n");

    *err=true;
    return false;
  }

  //===>trsnEnd, startState, not root
  if(p->trsn==p->trsnEnd &&
     p->state==p->startState &&
     p->state!=p->endState &&
     *pDepth != 0) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns found\n");

    p=parmac_stack_pop(stk,pDepth);
    p->trsn++;

    return true;
  }

  //===> on machine
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->machine &&
     !p->trsn->event) {

    p=parmac_stack_push(stk,pDepth,p->trsn->machine);

    PARMAC_DEBUG_STEPS_PRINTF("=trying machine '%s'\n",p->name);

    return true;
  }

  //===> on event
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->event &&
     !p->trsn->machine) {
    const char *eventName;
    const char *eventRet=p->trsn->event(p->src,&eventName,userdata);

    if(eventRet==NULL) {
      PARMAC_DEBUG_STEPS_PRINTF("=event '%s' failed\n",eventName);
      p->trsn++;
    } else {
      PARMAC_DEBUG_STEPS_PRINTF("=event '%s' success\n",eventName);
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

      parmac_prev_callbacks(stk,p,userdata);
      parmac_state_transition(stk,p,eventRet,userdata);
    }

    return true;
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

    return true;
  }

  //===> shouldn't reach this point
  assert(0);
  *err=true;
  return false;
}
