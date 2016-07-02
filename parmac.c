
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

struct parmac *parmac_set(struct parmac *p,const char *name,const char *src,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *startTrsn,
                          const struct parmac_transition *endTrsn) {
  p->src=src;
  p->state=startState;
  p->startState=startState;
  p->endState=endState;
  p->trsnStart=startTrsn;
  p->trsnIt=startTrsn;
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
  struct parmac *p=&stk[*pDepth];
  struct parmac *p2=((*pDepth)!=0)?p-1:NULL;
  (*pDepth)=((*pDepth)==0)?0:(*pDepth)-1; //since unsigned
  return p2;
}

struct parmac *parmac_stack_incr(struct parmac *p) {
  return p+1;
}

struct parmac *parmac_stack_decr(struct parmac *p) {
  return p-1;
}

#ifdef PARMAC_DEBUG_CALLBACKS
const char *parmac_debug_markStart=NULL;
const char *parmac_debug_markEnd=NULL;
#endif

void parmac_on_state_enter(struct parmac *stk,
                           struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *data,
                           const char *debug) {

  unsigned int depth=(unsigned int)(p-stk);

#ifdef PARMAC_DEBUG_CALLBACKS
  parmac_debug_markStart=srcStart;
  parmac_debug_markEnd=srcEnd;
  printf("%s : enter_%s_%s%s%s '%.*s'\n",
         debug,p->name,
         toState->name,
         fromState?" <=":"",
         fromState?fromState->name:"",
         srcEnd-srcStart,srcStart);
#endif

  if(toState->enter) {
    toState->enter(depth,fromState,toState,srcStart,srcEnd,data);
  }
}

void parmac_on_state_leave(struct parmac *stk,
                           struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           void *data,
                           const char *debug) {
  unsigned int depth=(unsigned int)(p-stk);

#ifdef PARMAC_DEBUG_CALLBACKS
  printf("%s : leave_%s_%s%s%s '%.*s'\n",
         debug,p->name,
         fromState->name,
         toState?" =>":"",
         toState?toState->name:"",
         parmac_debug_markEnd-parmac_debug_markStart,
         parmac_debug_markStart);
#endif

  if(fromState->leave) {
    fromState->leave(depth,fromState,toState,data);
  }
}

void parmac_on_event_success(struct parmac *stk,
                             struct parmac *p,
                             unsigned int depth,
                             const char *srcStart,
                             const char *srcEnd,
                             void *data) {

  PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

  //prev machines start states
  if(p->trsnIt->fromState==p->startState) {
    struct parmac *p2=p;

    //to bottom
    while(p2!=stk && p2->trsnIt->fromState==p2->startState) {//p2->prev p-depth
      p2=parmac_stack_decr(p2); //p2--;
    }

    //to top
    while(p2 && p2!=p) {
      //prev machines start state, on enter
      if(p2->trsnIt->fromState==p2->startState) {
        parmac_on_state_enter(stk,p2,NULL,p2->trsnIt->fromState,
                              p2->src,p2->src,data,"e");
      }

      //prev machines, on leave
      parmac_on_state_leave(stk,p2,p2->trsnIt->fromState,
                            p2->trsnIt->toState,data,"d");

      //
      p2=parmac_stack_incr(p2); //p2++;
    }
  }

  //last start state, on enter
  if(p->trsnIt->fromState==p->startState) {
    parmac_on_state_enter(stk,p,NULL,p->trsnIt->fromState,p->src,p->src,data,"c");
  }

  //last state, on leave
  parmac_on_state_leave(stk,p,p->trsnIt->fromState,p->trsnIt->toState,data,"b");

  //cur state transition, on enter
  parmac_on_state_enter(stk,p,p->trsnIt->fromState,p->trsnIt->toState,
                        srcStart,srcEnd,data,"a");

  //change state
  p->state=p->trsnIt->toState;
  p->src=srcEnd;
  p->trsnIt=p->trsnStart;
}

bool parmac_run(struct parmac *stk,unsigned int *pDepth,
                void *data,bool *err) {
  *err=false;
  struct parmac *p=&stk[*pDepth];

  //===> on empty stack, stop and do nothing
  if(!p) {
    PARMAC_DEBUG_STEPS_PRINTF("=stk empty\n");
    return false;
  }

  //===> transition fromState doesn't match state
  if(p->trsnIt!=p->trsnEnd &&
     p->state!=p->trsnIt->fromState &&
     p->state!=p->endState) {
    p->trsnIt++;
    return true;
  }

  //debug print pos
#ifdef PARMAC_DEBUG_STEPS
  {
    struct parmac *p2=stk;
    printf("\n");

    while(p2!=p+1) {
      const char *f=(p2->trsnIt==p2->trsnEnd)?"X":p2->trsnIt->fromState->name;
      const char *t=(p2->trsnIt==p2->trsnEnd)?"X":p2->trsnIt->toState->name;
      unsigned int d=(*pDepth)-(unsigned int)(p-p2);
      printf("/(%i) %s : %s (%s -> %s)",d,p2->name,p2->state->name,f,t);
      p2=parmac_stack_incr(p2);
    }

    printf("\n");
  }
#endif

  //===> successfully transitioned to end state at root
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->endState &&
     (*pDepth)==0) {

    PARMAC_DEBUG_STEPS_PRINTF("=at end a\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    //end state, on leave
    parmac_on_state_leave(stk,p,p->endState,NULL,data,"x");

    //pop machine
    p=parmac_stack_pop(stk,pDepth);

    //
    return false;
  }

  //===> successfully transitioned to end state
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->endState &&
     (*pDepth)!=0) {
    PARMAC_DEBUG_STEPS_PRINTF("=at end b\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    //end state, on leave
    parmac_on_state_leave(stk,p,p->endState,NULL,data,"X");

    //
    const char *src2=p->src;

    //pop machine
    p=parmac_stack_pop(stk,pDepth);

    //last state, on enter
    parmac_on_state_enter(stk,p,p->trsnIt->fromState,p->trsnIt->toState,
                          p->src,src2,data,"J");

    //change state
    p->state=p->trsnIt->toState;
    p->src=src2;
    p->trsnIt=p->trsnStart;

    //
    return true;
  }

  //===>trsnEnd, not startState
  if(p->trsnIt==p->trsnEnd &&
     p->state!=p->startState) {
    PARMAC_DEBUG_STEPS_PRINTF("=err no trsns\n");

    p=parmac_stack_pop(stk,pDepth);
    *err=true;
    return false;
  }

  //===>trsnEnd, startState
  if(p->trsnIt==p->trsnEnd &&
     p->state==p->startState) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns found\n");

    //pop stack
    p=parmac_stack_pop(stk,pDepth);

    //increment transition iterator
    if(p) {
      p->trsnIt++;
      PARMAC_DEBUG_STEPS_PRINTF("==it trsns\n");
      return true;
    }

    //
    PARMAC_DEBUG_STEPS_PRINTF("==end\n");

    *err=true;
    return false;
  }

  //======> is a machine
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->trsnIt->fromState &&
     p->trsnIt->machine) {

    p=parmac_stack_push(stk,pDepth,p->trsnIt->machine);

    PARMAC_DEBUG_STEPS_PRINTF("=trying machine '%s'\n",p->name);

    return true;
  }

  //======> is an event
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->trsnIt->fromState &&
     p->trsnIt->event) {
    const char *eventName;
    const char *eventRet=p->trsnIt->event(p->src,&eventName,data);
    const char *startSrc=p->src;

    PARMAC_DEBUG_STEPS_PRINTF("=trying event '%s'\n",eventName);

    if(eventRet==NULL) {
      //event failed, try next transition
      p->trsnIt++;
    } else {
      //event success
      PARMAC_DEBUG_STEPS_PRINTF("=event success\n");

      parmac_on_event_success(stk,p,*pDepth,p->src,eventRet,data);
    }

    return true;
  }

  //======> no machine or event specified
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->trsnIt->fromState &&
     !p->trsnIt->event &&
     !p->trsnIt->machine) {

    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");

    parmac_on_event_success(stk,p,*pDepth,p->src,p->src,data);

    return true;
  }

  //===> shouldn't reach this point
  assert(0);
  return false;
}
