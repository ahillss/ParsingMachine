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

#ifdef PARMAC_DEBUG_INSTANCES
int parmac_debug_count=0;
#endif

struct parmac *parmac_set(struct parmac *p,const char *name,const char *src,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *startTrsn,
                          const struct parmac_transition *endTrsn) {
#ifdef PARMAC_DEBUG_INSTANCES
  parmac_debug_count++;
  printf("push_inst_p=%i\n",parmac_debug_count);
#endif

  p->src=src;
  p->state=startState;
  p->startState=startState;
  p->endState=endState;

  p->trsnStart=startTrsn;
  p->trsnIt=startTrsn;
  p->trsnEnd=endTrsn;

  p->prev=false;
  p->next=false;

  p->name=name;

  return p;

}

struct parmac *parmac_push(struct parmac *stk,unsigned int *depth,parmac_machine machine) {
  struct parmac *p=&stk[*depth];
  struct parmac *p2=p+1; //get next free mem

  machine(p2,p->src);
  p2->prev=true;
  p->next=true;
  (*depth)++;

  return p2;
}

struct parmac *parmac_pop(struct parmac *stk,unsigned int *depth) {
#ifdef PARMAC_DEBUG_INSTANCES
  parmac_debug_count++;
  printf("pop_inst_p=%i\n",parmac_debug_count);
#endif

  struct parmac *p=&stk[*depth];
  struct parmac *p2=(p->prev)?p-1:NULL;

  if(p2) {
    p2->next=false;
  }

  (*depth)--;
  return p2;
}

void parmac_print_parse_pos(struct parmac *p,bool end) {
  if(p->prev) {
    parmac_print_parse_pos(p-1,false); //p->prev
  } else {
    printf("\n");
  }

  int prevCount=0,nextCount=0;
  struct parmac *tmp;

  tmp=p;

  while(tmp&&tmp->prev) {
    prevCount++;
    tmp--;//tmp=tmp->prev;
  }

  tmp=p;

  while(tmp&&tmp->next) {
    nextCount++;
    tmp++;//tmp=tmp->next;
  }

  printf("/ %s : %s (%s -> %s) (p:%i n:%i)",p->name,
         p->state->name,
         (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->fromState->name,
         (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->toState->name,
         prevCount,nextCount);

  if(end) { printf("\n"); }
}


#ifdef PARMAC_DEBUG_CALLBACKS
const char *parmac_debug_markStart;
const char *parmac_debug_markEnd;
#endif

void parmac_on_state_enter(struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           const char *srcStart,
                           const char *srcEnd,
                           void *data,
                           const char *debug) {

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
    toState->enter(fromState,toState,srcStart,srcEnd,data);
  }
}

void parmac_on_state_leave(struct parmac *p,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState,
                           void *data,
                           const char *debug) {

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
    fromState->leave(fromState,toState,data);
  }
}


void parmac_on_event_success(struct parmac *p,
                             const char *srcStart,
                             const char *srcEnd,
                             void *data) {

#ifndef PARMAC_DEBUG_STEPS
  PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

  //section B040 : prev machines start states
  if(p->trsnIt->fromState==p->startState) {
    struct parmac *p2=p;

    //to bottom
    while(p2->prev && p2->trsnIt->fromState==p2->startState) {
      p2--;//p2=p2->prev;
    }

    //to top
    while(p2 && p2!=p) {
      //prev machines start state, on enter
      if(p2->trsnIt->fromState==p2->startState) {
        parmac_on_state_enter(p2,NULL,p2->trsnIt->fromState,p2->src,p2->src,data,"e");
      }

      //prev machines, on leave
      parmac_on_state_leave(p2,p2->trsnIt->fromState,p2->trsnIt->toState,data,"d");

      //
      p2++;//p2=p2->next;
    }
  }

  //section B060 : last start state, on enter
  if(p->trsnIt->fromState==p->startState) {
    parmac_on_state_enter(p,NULL,p->trsnIt->fromState,p->src,p->src,data,"c");
  }

  //section B100 : last state, on leave
  parmac_on_state_leave(p,p->trsnIt->fromState,p->trsnIt->toState,data,"b");

  //section B120 : cur state transition, on enter
  parmac_on_state_enter(p,p->trsnIt->fromState,p->trsnIt->toState,srcStart,srcEnd,data,"a");

  //change state
  p->state=p->trsnIt->toState;
  p->src=srcEnd;
  p->trsnIt=p->trsnStart;


  PARMAC_DEBUG_STEPS_PRINTF("==aaa\n");


#ifdef PARMAC_DEBUG_CALLBACKS
  printf("ok\n");
#endif
}

bool parmac_run(struct parmac *stk,unsigned int *depth,void *data,bool *err) {
  *err=false;
  struct parmac *p=&stk[*depth];//*pp;

  //===> on empty stack, stop and do nothing
  if(!p) {
    PARMAC_DEBUG_STEPS_PRINTF("=stk empty\n");
    return false;
  }

  //===> transition fromState doesn't match state
  if(p->trsnIt!=p->trsnEnd &&
     p->state!=p->trsnIt->fromState &&
     p->state!=p->endState
     ) {
    // PARMAC_DEBUG_STEPS_PRINTF("=incr trsn7\n");

    p->trsnIt++;
    return true;
  }

  //
#ifdef PARMAC_DEBUG_STEPS
  parmac_print_parse_pos(p,true);
#endif

  //===> successfully transitioned to end state at root, without excur
  if(//p->trsnIt!=p->trsnEnd &&
     p->state==p->endState &&
     !p->prev) {
    PARMAC_DEBUG_STEPS_PRINTF("=at end a\n");

#ifndef PARMAC_DEBUG_STEPS
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

    //end state, on leave
    parmac_on_state_leave(p,p->endState,NULL,data,"x");

    //pop machine
    p=parmac_pop(stk,depth);//p=parmac_pop(p);

    //
    return false;
  }

   //===> successfully transitioned to end state (only non excur reaches here?)
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->endState &&
     p->prev) {
    PARMAC_DEBUG_STEPS_PRINTF("=at end b\n");

#ifndef PARMAC_DEBUG_STEPS
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

    //end state, on leave
    parmac_on_state_leave(p,p->endState,NULL,data,"X");

    //
    const char *src2=p->src;

    //pop machine
    p=parmac_pop(stk,depth);

    //last state, on enter
    parmac_on_state_enter(p,p->trsnIt->fromState,p->trsnIt->toState,p->src,src2,data,"J");

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

    p=parmac_pop(stk,depth);
    *err=true;
    return false;
  }

  //===>trsnEnd, startState
  if(p->trsnIt==p->trsnEnd &&
     p->state==p->startState) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns found\n");

    //pop stack
    p=parmac_pop(stk,depth);

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

    p=parmac_push(stk,depth,p->trsnIt->machine);

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

      parmac_on_event_success(p,p->src,eventRet,data);
    }

    return true;
  }

  //======> no machine or event specified
  if(p->trsnIt!=p->trsnEnd &&
     p->state==p->trsnIt->fromState &&
     !p->trsnIt->event &&
     !p->trsnIt->machine) {

    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");

    parmac_on_event_success(p,p->src,p->src,data);

    return true;
  }


  //===> shouldn't reach this point
  assert(0);
  return false;
}
