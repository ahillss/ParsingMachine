#define PARMAC_DEBUG_STEPS
#define PARMAC_DEBUG_CALLBACKS

#include "parmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#ifdef PARMAC_DEBUG_STEPS
#define PARMAC_DEBUG_STEPS_PRINTF(...) fprintf(stderr,__VA_ARGS__)
#else
#define PARMAC_DEBUG_STEPS_PRINTF(...)
#endif

#ifdef PARMAC_DEBUG_CALLBACKS
#define PARMAC_DEBUG_CALLBACKS_PRINTF(...) fprintf(stderr,__VA_ARGS__)
#else
#define PARMAC_DEBUG_CALLBACKS_PRINTF(...)
#endif

struct parmac *parmac_set(struct parmac *p,
                          const char *name,
                          PARMAC_POS pos,
                          const struct parmac_state *startState,
                          const struct parmac_state *endState,
                          const struct parmac_transition *trsnStart,
                          const struct parmac_transition *trsnEnd) {
  p->pos=pos;
  p->state=startState;
  p->startState=startState;
  p->endState=endState;
  p->trsnStart=trsnStart;
  p->trsn=trsnStart;
  p->trsnEnd=trsnEnd;
  p->name=name;
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

  PARMAC_DEBUG_CALLBACKS_PRINTF("(%u) enter_%s_%s (<-%s) (%u+%u)\n",
                                stkDepth,p->name,
                                toState->name,fromState?fromState->name:"_",
                                (unsigned int)fromPos,
                                (unsigned int)(toPos-fromPos));

  if(toState->enter) {
    toState->enter(stkDepth,p->name,
                   fromState?fromState->name:NULL,
                   toState->name,
                   fromPos,toPos,userdata);
  }
}

void parmac_on_state_leave(struct parmac *stk,
                           PARMAC_DEPTH stkDepth,
                           void *userdata,
                           const struct parmac_state *fromState,
                           const struct parmac_state *toState) {
  struct parmac *p=&stk[stkDepth];

  PARMAC_DEBUG_CALLBACKS_PRINTF("(%u) leave_%s_%s (->%s)\n",
                                stkDepth,p->name,
                                fromState->name,toState?toState->name:"_");

  if(fromState->leave) {
    fromState->leave(stkDepth,p->name,
                     fromState->name,
                     toState?toState->name:NULL,
                     userdata);
  }
}

void parmac_prev_callbacks(struct parmac *stk,
                           PARMAC_DEPTH stkDepth,
                           void *userdata) {
  PARMAC_DEPTH d;

  //down
  for(d=stkDepth;d!=0 && stk[d].trsn->fromState==stk[d].startState;d--) {
  }

  //up
  for(;d<=stkDepth;d++) {
    if(stk[d].trsn->fromState==stk[d].startState) {
      parmac_on_state_enter(stk,d,stk[d].pos,stk[d].pos,userdata,
                            NULL,stk[d].trsn->fromState);
    }

    parmac_on_state_leave(stk,d,userdata,
                          stk[d].trsn->fromState,
                          stk[d].trsn->toState);
  }
}

void parmac_state_transition(struct parmac *stk,
                             PARMAC_DEPTH stkDepth,
                             PARMAC_POS pos,
                             void *userdata) {

  struct parmac *p=&stk[stkDepth];

  parmac_on_state_enter(stk,stkDepth,p->pos, pos,userdata,
                        p->trsn->fromState,p->trsn->toState);

  //change state
  p->state=p->trsn->toState;
  p->pos=pos;
  p->trsn=p->trsnStart;
}

bool parmac_run(struct parmac *stk,
                PARMAC_DEPTH *stkDepthPtr,
                void *userdata) {

  struct parmac *p=&stk[*stkDepthPtr];

  //===
  assert(p->trsn==p->trsnEnd || p->trsn->fromState!=p->endState);
  assert(p->trsn==p->trsnEnd || p->trsn->toState!=p->startState);
  assert(p->trsn==p->trsnEnd || !p->trsn->event || !p->trsn->machine);
  assert(p->startState!=p->endState);

  //===> debug print pos
#ifdef PARMAC_DEBUG_STEPS
  {
    PARMAC_DEBUG_STEPS_PRINTF("\n");

    PARMAC_DEPTH d;

    for(d=0;d<=*stkDepthPtr;d++) {
      struct parmac *p2=&stk[d];
      const char *from=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->fromState->name;
      const char *to=(p2->trsn==p2->trsnEnd)?"X":p2->trsn->toState->name;
      unsigned int stkDepth=(unsigned int)(*stkDepthPtr)-(unsigned int)(p-p2);
      unsigned int trsn=(unsigned int)(p2->trsn-p2->trsnStart);
      PARMAC_DEBUG_STEPS_PRINTF("/ %s : %s (%s -> %s) (d%u p%u t%u)",
                                p2->name,p2->state->name,from,to,
                                stkDepth,p2->pos,trsn);
    }

    PARMAC_DEBUG_STEPS_PRINTF("\n");
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

    return true;
  }

  //===> at end state, not root
  if(p->state==p->endState &&
     (*stkDepthPtr)!=0) {

    PARMAC_DEBUG_STEPS_PRINTF("=end, not root, pop machine\n");
    PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

    parmac_on_state_leave(stk,*stkDepthPtr,userdata,p->endState,NULL);

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

    parmac_on_state_leave(stk,*stkDepthPtr,userdata,p->endState,NULL);

    return false; //success
  }

  //===> trsnEnd, either not startState or at root
  if(p->trsn==p->trsnEnd &&
     p->state!=p->endState &&
     (p->state!=p->startState ||
      *stkDepthPtr==0)) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, fail\n");
    return false;
  }

  //===> trsnEnd, startState, not root
  if(p->trsn==p->trsnEnd &&
     p->state==p->startState &&
     p->state!=p->endState &&
     *stkDepthPtr != 0) {

    PARMAC_DEBUG_STEPS_PRINTF("=no trsns left, pop machine\n");

    p=parmac_stack_pop(stk,stkDepthPtr);
    p->trsn++;

    return true;
  }

  //===> on machine
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->machine &&
     !p->trsn->event) {

    PARMAC_DEBUG_STEPS_PRINTF("=pushed machine\n");

    p=parmac_stack_push(stk,stkDepthPtr,p->trsn->machine);

    return true;
  }

  //===> on event
  if(p->trsn!=p->trsnEnd &&
     p->state==p->trsn->fromState &&
     p->state!=p->endState &&
     p->trsn->event &&
     !p->trsn->machine) {

    PARMAC_POS retPos=p->pos;

    if(p->trsn->event(&retPos, userdata)) {
      PARMAC_DEBUG_STEPS_PRINTF("=event success\n");
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");

      parmac_prev_callbacks(stk,*stkDepthPtr,userdata);
      parmac_state_transition(stk,*stkDepthPtr,retPos,userdata);
    } else {
      PARMAC_DEBUG_STEPS_PRINTF("=event failed\n");
      p->trsn++;
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
