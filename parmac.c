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

#define IS_TRSN_END(P) (!P->trsnIt->fromState)

struct parmac *parmac_set(struct parmac *p,
                          PARMAC_POS pos,
                          const char *name,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *trsns) {
  p->pos=pos;
  p->prevPos=pos;
  p->name=name;
  p->state=startState;
  p->startState=startState;
  p->endState=endState;
  p->trsnIt=trsns;
  p->trsns=trsns;
  return p;
}

struct parmac *parmac_stack_push(struct parmac *stk,
                                 PARMAC_DEPTH *stkDepthPtr,
                                 parmac_machine machine) {
  struct parmac *p=&stk[*stkDepthPtr];
  struct parmac *p2=&stk[(*stkDepthPtr)+1]; //get next free mem

  machine(p2,p->pos);
  (*stkDepthPtr)++;
  return p2;
}

struct parmac *parmac_stack_pop(struct parmac *stk,PARMAC_DEPTH *stkDepthPtr) {
  assert((*stkDepthPtr)!=0);
  (*stkDepthPtr)--;
  return &stk[*stkDepthPtr];
}

void parmac_on_state_enter(struct parmac *stk,
                           PARMAC_DEPTH stkDepth,
                           PARMAC_POS fromPos,
                           PARMAC_POS toPos,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  struct parmac *p=&stk[stkDepth];

  PARMAC_DEBUG_CALLBACKS_PRINTF("(%u) enter_%s_%s (<-%s) [%u-%u]\n",
                                stkDepth,p->name,
                                toState->name,fromState?fromState->name:"_",
                                (unsigned int)fromPos,
                                (unsigned int)toPos);

  if(toState->enter) {
    toState->enter(stkDepth,p->name,
                   fromState?fromState->name:NULL,
                   toState->name,
                   fromPos,toPos,userdata);
  }
}

void parmac_on_state_leave(struct parmac *stk,
                           PARMAC_DEPTH stkDepth,
                           PARMAC_POS fromPos,
                           PARMAC_POS toPos,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  struct parmac *p=&stk[stkDepth];

  PARMAC_DEBUG_CALLBACKS_PRINTF("(%u) leave_%s_%s (->%s) [%u-%u]\n",
                                stkDepth,p->name,
                                fromState->name,toState?toState->name:"_",
                                (unsigned int)fromPos,
                                (unsigned int)toPos);

  if(fromState->leave) {
    fromState->leave(stkDepth,p->name,
                     fromState->name,
                     toState?toState->name:NULL,
                     fromPos,toPos,userdata);
  }
}

void parmac_prev_callbacks(struct parmac *stk,
                           PARMAC_DEPTH stkDepth,
                           void *userdata) {
  PARMAC_DEPTH d;

  //down
  for(d=stkDepth;d!=0 && stk[d].trsnIt->fromState==stk[d].startState;d--) {
  }

  //up
  for(;d<=stkDepth;d++) {
    if(stk[d].trsnIt->fromState==stk[d].startState) {
      parmac_on_state_enter(stk,d,stk[d].pos,stk[d].pos,userdata,
                            NULL,stk[d].trsnIt->fromState);
    }

    parmac_on_state_leave(stk,d,
                          stk[d].prevPos,stk[d].pos,
                          userdata,
                          stk[d].trsnIt->fromState,
                          stk[d].trsnIt->toState);
  }
}

void parmac_state_transition(struct parmac *stk,
                             PARMAC_DEPTH stkDepth,
                             PARMAC_POS pos,
                             void *userdata) {

  struct parmac *p=&stk[stkDepth];

  parmac_on_state_enter(stk,stkDepth,p->pos, pos,userdata,
                        p->trsnIt->fromState,p->trsnIt->toState);

  //change state
  p->state=p->trsnIt->toState;
  p->prevPos=pos;
  p->pos=pos;
  p->trsnIt=p->trsns;
}

bool parmac_run(struct parmac *stk,
                PARMAC_DEPTH *stkDepthPtr,
                void *userdata) {

  struct parmac *p=&stk[*stkDepthPtr];

  //===
  assert(IS_TRSN_END(p) || p->trsnIt->fromState!=p->endState);
  assert(IS_TRSN_END(p) || p->trsnIt->toState!=p->startState);
  assert(IS_TRSN_END(p) || !p->trsnIt->toState->event || !p->trsnIt->toState->machine);
  assert(p->startState!=p->endState);

  //===> debug print pos
  PARMAC_DEBUG_STEPS_PRINTF("\n(%u) %s : %s (%s -> %s) @%u\n",
                            *stkDepthPtr,p->name,p->state->name,
                            IS_TRSN_END(p)?"X":p->trsnIt->fromState->name,
                            IS_TRSN_END(p)?"X":p->trsnIt->toState->name,
                            p->pos);



  //===> transition fromState doesn't match state
  if(!IS_TRSN_END(p) &&
     p->state!=p->trsnIt->fromState &&
     p->state!=p->endState) {
    PARMAC_DEBUG_STEPS_PRINTF("=iterating trsns\n");

    while(!IS_TRSN_END(p) && p->state!=p->trsnIt->fromState) {
      p->trsnIt++;
    }

    return true;
  }

  //===> at end state, not root
  if(p->state==p->endState &&
     (*stkDepthPtr)!=0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, not root, pop machine\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave(stk,*stkDepthPtr,p->pos,p->pos,userdata,p->endState,NULL);

    PARMAC_POS pos2=p->pos;
    p=parmac_stack_pop(stk,stkDepthPtr);

    parmac_state_transition(stk,*stkDepthPtr,pos2,userdata);

    return true;
  }

  //===> at end state, root
  if(p->state==p->endState &&
     (*stkDepthPtr)==0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, root, finished\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave(stk,*stkDepthPtr,p->pos,p->pos,userdata,p->endState,NULL);

    return false; //success
  }

  //===> trsnEnd, either not startState or at root
  if(IS_TRSN_END(p) &&
     p->state!=p->endState &&
     (p->state!=p->startState ||
      *stkDepthPtr==0)) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, fail\n");
    return false;
  }

  //===> trsnEnd, startState, not root
  if(IS_TRSN_END(p) &&
     p->state==p->startState &&
     p->state!=p->endState &&
     *stkDepthPtr != 0) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, pop machine\n");

    p=parmac_stack_pop(stk,stkDepthPtr);
    p->trsnIt++;

    return true;
  }

  //===> on machine
  if(!IS_TRSN_END(p) &&
     p->state==p->trsnIt->fromState &&
     p->state!=p->endState &&
     p->trsnIt->toState->machine &&
     !p->trsnIt->toState->event
     ) {

    PARMAC_DEBUG_STEPS_PRINTF("=pushed machine\n");

    p=parmac_stack_push(stk,stkDepthPtr,p->trsnIt->toState->machine);

    return true;
  }

  //===> on event
  if(!IS_TRSN_END(p) &&
     p->state==p->trsnIt->fromState &&
     p->state!=p->endState &&
     p->trsnIt->toState->event &&
     !p->trsnIt->toState->machine) {

    PARMAC_POS retPos=p->pos;

    if(p->trsnIt->toState->event(&retPos, userdata)) {
      PARMAC_DEBUG_STEPS_PRINTF("=event success\n");
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

      parmac_prev_callbacks(stk,*stkDepthPtr,userdata);
      parmac_state_transition(stk,*stkDepthPtr,retPos,userdata);
    } else {
      PARMAC_DEBUG_STEPS_PRINTF("=event failed\n");
      p->trsnIt++;
    }

    return true;
  }

  //===> on no machine or event
  if(!IS_TRSN_END(p) &&
     p->state==p->trsnIt->fromState &&
     p->state!=p->endState &&
     !p->trsnIt->toState->event &&
     !p->trsnIt->toState->machine) {

    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_prev_callbacks(stk,*stkDepthPtr,userdata);
    parmac_state_transition(stk,*stkDepthPtr,p->pos,userdata);

    return true;
  }

  //===> shouldn't reach this point
  assert(0);
  return false;
}

bool parmac_failed(struct parmac *stk) {
  return (stk[0].state!=stk[0].endState);
}

PARMAC_POS parmac_last_pos(struct parmac *stk,PARMAC_DEPTH stkDepth) {
  return stk[stkDepth].pos;
}
