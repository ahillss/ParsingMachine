#include "parmac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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

  p->prev=NULL;
  p->next=NULL;

  p->name=name;

  p->excurUp=NULL;
  p->excurDown=NULL;

  return p;

}


struct parmac *parmac_pop(struct parmac *p) {
#ifdef PARMAC_DEBUG_INSTANCES
  parmac_debug_count++;
  printf("pop_inst_p=%i\n",parmac_debug_count);
#endif

  if(p->prev) {
    p->prev->next=NULL;
  }

  return p->prev;
}

void parmac_print_parse_pos(struct parmac *p,bool end) {
  if(p->prev) {
    parmac_print_parse_pos(p->prev,false);
  } else {
    PARMAC_DEBUG_STEPS_PRINTF("\n");
  }

  int excurDownCount=0,excurUpCount=0,prevCount=0,nextCount=0;
  struct parmac *tmp;

  tmp=p;

  while(tmp&&tmp->excurUp) {
    excurUpCount++;
    tmp=tmp->excurUp;
  }

  tmp=p;

  while(tmp&&tmp->excurDown) {
    excurDownCount++;
    tmp=tmp->excurDown;
  }

  tmp=p;

  while(tmp&&tmp->prev) {
    prevCount++;
    tmp=tmp->prev;
  }
  
  tmp=p;

  while(tmp&&tmp->next) {
    nextCount++;
    tmp=tmp->next;
  }
 
  PARMAC_DEBUG_STEPS_PRINTF("/ %s : %s (%s -> %s) (u:%i d:%i p:%i n:%i)",p->name,
                            p->state->name,
                            (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->fromState->name,
                            (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->toState->name,
                            excurUpCount,excurDownCount,prevCount,nextCount);

  if(end) { PARMAC_DEBUG_STEPS_PRINTF("\n"); }
}

struct parmac *parmac_excur_up(struct parmac *p) {
  if(p->prev) {
    p->prev->next=p->excurUp->prev;
  }

  p=p->excurUp;
  return p;
}

struct parmac *parmac_excur_down(struct parmac *p) {
  p=p->excurDown;

  if(p->prev) {
    p->prev->next=p;
  }

  return p;
}

struct parmac *parmac_push_excur(struct parmac *p) {
#ifdef PARMAC_DEBUG_INSTANCES
  parmac_debug_count++;
  printf("push_inst_e=%i\n",parmac_debug_count);
#endif

  struct parmac *cur,*prev;

  cur=p;
  prev=p->prev;

  p++;

  cur->excurDown=p;

  p->src=cur->src;
  p->trsnStart=prev->trsnStart;
  p->trsnEnd=prev->trsnEnd;
  p->trsnIt=prev->trsnStart;
  p->state=prev->trsnIt->toState;
  p->startState=prev->startState;
  p->endState=prev->endState;
  p->prev=prev->prev;

  if(prev->prev) {
    prev->prev->next=p;
  }

  p->next=NULL;
  p->excurUp=cur;
  p->excurDown=NULL;
  p->name=prev->name;

  return p;
}

struct parmac *parmac_pop_excur(struct parmac *p) {

  if(p->prev) {
    p->prev->next=p->excurUp->prev;
  }

  p=p->excurUp;
  p->excurDown=NULL;
  p->trsnIt++;

  return p;
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
  printf("%s : %s_%s_enter(%i) '%.*s'\n",
         debug,p->name,toState->name,
         fromState!=toState,
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
  printf("%s : %s_%s_leave(%i) '%.*s'\n",
         debug,p->name,fromState->name,
         fromState!=toState,
         parmac_debug_markEnd-parmac_debug_markStart,
         parmac_debug_markStart);
#endif

  if(fromState->leave) {
    fromState->leave(fromState,toState,data);
  }
}

void parmac_on_event_success_excurs(struct parmac *p,void *data) {

  //on excur downwards

  struct parmac *p2=p;

  //to top
  while(p2->excurUp) {
    p2=p2->excurUp;
  }

  //to bottom
  while(p2 && p2!=p) {

    //excur prev state, on enter
    if(p2->excurUp) {
      struct parmac *p3=p2->excurUp->prev;
      //p2,p2->trsnIt->state,p3->src,p2->src,p2->trsnIt->state!=p2->trsnIt->toState (wrong?)
      parmac_on_state_enter(p2,p2->trsnIt->fromState,p2->trsnIt->toState,p3->src,p2->src,data,"i");
    }

    //excur prev state, on leave
    //p2,p2->trsnIt->state,true (is end state?)
    parmac_on_state_leave(p2,p2->trsnIt->fromState,NULL,data,"h");

    // excur end state, on enter
    //p2,p2->endState,p2->src,p2->src,true
    parmac_on_state_enter(p2,p2->trsnIt->fromState,p2->endState,p2->src,p2->src,data,"g");

    // excur end state, on leave
    //p2,p2->endState,true
    parmac_on_state_leave(p2,p2->endState,NULL,data,"f");

    //
    //struct parmac *p3=p2;
    p2=p2->excurDown;
  }

  PARMAC_DEBUG_CALLBACKS_PRINTF("--\n");

}

void parmac_on_event_success(struct parmac *p,
                             const char *srcStart,
                             const char *srcEnd,
                             bool excurs,void *data) {

#ifndef PARMAC_DEBUG_STEPS
  PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

  //
  if(excurs) {
    //section B000
    if(p->excurUp) {
      parmac_on_event_success_excurs(p,data);
    }

    //section B020
    if(!p->excurUp) {
      struct parmac *p2=p;

      //
      while(p2->prev && !p2->excurUp) {
        p2=p2->prev;
      }

      //
      if(p2->excurUp) {
        parmac_on_event_success_excurs(p2,data);

        //
        struct parmac *p3=p2->excurUp->prev;
        //p2,p2->state,p3->src,p2->src,p3->trsnIt->state!=p3->trsnIt->toState (hmm)
        parmac_on_state_enter(p2,p3->trsnIt->fromState,p2->state,p3->src,p2->src,data,"k");

      }
    }
  }

  //section B040 : prev machines start states
  if(p->trsnIt->fromState==p->startState) {
    struct parmac *p2=p;

    //to bottom
    while(p2->prev && p2->trsnIt->fromState==p2->startState) {
      p2=p2->prev;
    }

    //to top
    while(p2 && p2!=p) {
      //prev machines start state, on enter
      if(p2->trsnIt->fromState==p2->startState) {
        //p2,p2->startState,p2->src,p2->src,true
		parmac_on_state_enter(p2,NULL,p2->startState,p2->src,p2->src,data,"e");
      }

      //prev machines, start state, on leave
      //p2,p2->trsnIt->state,p2->trsnIt->state!=p2->trsnIt->toState
      parmac_on_state_leave(p2,p2->trsnIt->fromState,p2->trsnIt->toState,data,"d");

      //
      p2=p2->next;
    }
  }

  //section B060 : last start state, on enter
  if(p->trsnIt->fromState==p->startState) {
    //p,p->startState,p->src,p->src,true
    parmac_on_state_enter(p,NULL,p->startState,p->src,p->src,data,"c");
  }

  if(excurs) {
    //section B080 : last state, on enter
    if(p->excurUp) {
      struct parmac *p2=p->excurUp->prev;
      //p,p->state,p2->src,p->src,p2->trsnIt->state!=p2->trsnIt->toState (hmm)
      parmac_on_state_enter(p,p2->trsnIt->fromState,p->state,p2->src,p->src,data,"j");
    }
  }

  //section B100 : last state, on leave
  //p, p->trsnIt->state,p->trsnIt->state!=p->trsnIt->toState
  parmac_on_state_leave(p,p->trsnIt->fromState,p->trsnIt->toState,data,"b");

  //section B120 : cur state transition, on enter
  //p,p->trsnIt->toState,srcStart,srcEnd,p->trsnIt->state!=p->trsnIt->toState
  parmac_on_state_enter(p,p->trsnIt->fromState,p->trsnIt->toState,srcStart,srcEnd,data,"a");

  //change state
  p->state=p->trsnIt->toState;
  p->src=srcEnd;
  p->trsnIt=p->trsnStart;

  if(excurs) {
    p->excurUp=NULL;
    p->excurDown=NULL;
  }

  PARMAC_DEBUG_STEPS_PRINTF("==aaa\n");


#ifdef PARMAC_DEBUG_CALLBACKS
  printf("ok\n");
#endif
}

bool parmac_run(struct parmac **pp,void *data,bool *err,bool excurs) {
  *err=false;
  struct parmac *p=*pp;

  //===> on empty stack, stop and do nothing
  if(!p) {
    PARMAC_DEBUG_STEPS_PRINTF("=stk empty\n");
    return false;//NULL;
  }

  //
#ifdef PARMAC_DEBUG_STEPS
  parmac_print_parse_pos(p,true);
#endif

#if defined(PARMAC_DEBUG_CALLBACKS) || defined(PARMAC_DEBUG_STEPS)
  // printf("'%s'\n",p->src);
#endif

  if(excurs) {
    //===>excur down, toEnd, not root, push excur
    if(!p->excurDown && p->prev &&
       p->trsnIt!=p->trsnEnd &&
       p->trsnIt->fromState==p->state &&
       p->trsnIt->toState==p->endState) {
      PARMAC_DEBUG_STEPS_PRINTF("=push excur\n");
      p=parmac_push_excur(p);

      *pp=p; return true;//p;
    }

    //===>excur down, trsnEnd, pop excur
    if(p->excurUp && !p->excurDown && p->trsnIt==p->trsnEnd) {
      PARMAC_DEBUG_STEPS_PRINTF("=no trsns, pop excur\n");

      p=parmac_pop_excur(p);
      *pp=p; return true;//p;

    }

    //===>excur down, toEnd, root, go up
    if(p->excurUp && !p->prev &&
       p->trsnIt!=p->trsnEnd &&
       p->trsnIt->fromState==p->state &&
       p->trsnIt->toState==p->endState) {
      p=parmac_excur_up(p);

      PARMAC_DEBUG_STEPS_PRINTF("=bottom at end, go up excur\n");

      *pp=p; return true;//p;
    }

    //===>excur up, toEnd, iterate trsn
    if(p->excurDown &&
       p->trsnIt!=p->trsnEnd &&
       p->trsnIt->fromState==p->state &&
       p->trsnIt->toState==p->endState) {
      p->trsnIt++;

      PARMAC_DEBUG_STEPS_PRINTF("=to end, iterate trsn\n");

      *pp=p; return true;//p;
    }

    //===>excur up, trsnEnd, go up
    if(p->excurUp &&
       p->excurDown &&
       p->trsnIt==p->trsnEnd) {

      p=parmac_excur_up(p);
      PARMAC_DEBUG_STEPS_PRINTF("=trsnEnd, go up excur\n");

      *pp=p; return true;//p;
    }

    //===>excur up, top, trsnEnd, go root exit
    if(!p->excurUp &&
       p->excurDown &&
       p->trsnIt==p->trsnEnd) {

      PARMAC_DEBUG_STEPS_PRINTF("=bla to end\n");


#ifndef PARMAC_DEBUG_STEPS
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

      //
      //p,(p->trsnIt-1)->state,true (hmm)
      parmac_on_state_leave(p,(p->trsnIt-1)->fromState,NULL,data,"w"); //wrong toState

      while(p) {
        //p,p->endState,p->src,p->src,true (hmm)
        parmac_on_state_enter(p,NULL,p->endState,p->src,p->src,data,"m"); //wrong fromState

        //p, p->endState,true,data;
        parmac_on_state_leave(p,p->endState,NULL,data,"n");

        //
        if(p->prev) {
          struct parmac *p2=p->prev;

          //p2,p2->trsnIt->toState,p2->src,p->src,p2->trsnIt->state!=p2->trsnIt->toState
          parmac_on_state_enter(p2,p2->trsnIt->fromState,p2->trsnIt->toState,p2->src,p->src,data,"o");

          //p2, p2->trsnIt->toState,true
          parmac_on_state_leave(p2,p2->trsnIt->toState,NULL,data,"p"); //wrong toState?
        }

        //
        p=p->excurDown;
      }

      *pp=p; return true;//false;//NULL;
    }

    //===> successfully transitioned to end state, without excur (is this for excurs code only?)
    if(p->state==p->endState) {
      PARMAC_DEBUG_STEPS_PRINTF("=at end\n");

#ifndef PARMAC_DEBUG_STEPS
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

      //p,p->state,true
      parmac_on_state_leave(p,p->state,NULL,data,"x"); //wrong toState?

      //pop machine
      p=parmac_pop(p);

      if(p) {
        //temporary (for what?)
        p->trsnIt++;
      }

      //
      *pp=p; return true;//p;
    }
  }

  if(!excurs) {
    //===>
    if(p->state==p->endState) {
      PARMAC_DEBUG_STEPS_PRINTF("=at end2\n");

#ifndef PARMAC_DEBUG_STEPS
      PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

      //p,p->state,true,data
      parmac_on_state_leave(p,p->state,NULL,data,"X");

      //
      const char *src2=p->src;

      //pop machine
      p=parmac_pop(p);

      if(p) {
        //last state, on enter
        //p,p->trsnIt->toState,p->src,src2,p->trsnIt->state!=p->trsnIt->toState
        parmac_on_state_enter(p,p->trsnIt->fromState,p->trsnIt->toState,p->src,src2,data,"J");

        //change state
        p->state=p->trsnIt->toState;
        p->src=src2;
        p->trsnIt=p->trsnStart;
      }

      //
      *pp=p; return true;//p;
    }

  }

  //
  if(excurs) {
    //===>root, trsnEnd, not startState
    if(p->trsnIt==p->trsnEnd &&
       p->state!=p->startState &&
       !p->prev) {

      const struct parmac_transition *t;
      t=p->trsnEnd-1;

      while(t!=p->trsnStart && t->toState!=p->endState) {
        t--;
      }
 
      if(t->toState==p->endState) {
        PARMAC_DEBUG_STEPS_PRINTF("=root toEnd\n");
        p->trsnIt=t;
        parmac_on_event_success(p,p->src,p->src,excurs,data);

        *pp=p; return true;//NULL;
      }
    }

  }

  //===>trsnEnd, not startState
  if(p->trsnIt==p->trsnEnd &&
     p->state!=p->startState) {

    PARMAC_DEBUG_STEPS_PRINTF("=err no trsns\n");

    //
    p=parmac_pop(p);

    *err=true;
    //*pp=p;
    return false;//NULL;
  }


  //===>trsnEnd, startState
  if(p->trsnIt==p->trsnEnd &&
     p->state==p->startState) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns found\n");

    //pop stack
    p=parmac_pop(p);

    //increment transition iterator
    if(p) {
      p->trsnIt++;
      PARMAC_DEBUG_STEPS_PRINTF("==it trsns\n");
      *pp=p; return true;//p;
    }

    //
    PARMAC_DEBUG_STEPS_PRINTF("==end\n");

    *err=true;
    return false;
  }

  if(excurs) {
    //======> root, toEndState
    if(p->trsnIt->fromState==p->state &&
       p->trsnIt->toState==p->endState &&
       !p->prev) {
      PARMAC_DEBUG_STEPS_PRINTF("=root toEnd skip\n");
      p->trsnIt++;

      // PARMAC_DEBUG_STEPS_PRINTF("=root toEnd\n");
      // parmac_on_event_success(p,p->src,p->src);

      *pp=p; return true;//p;
    }
  }

  //======> is a machine
  if(p->trsnIt->fromState==p->state &&
     p->trsnIt->machine) {
// printf("aaa\n");
// #ifdef PARMAC_DEBUG_STEPS
//   parmac_print_parse_pos(p,true);
// #endif
// printf("bbb\n");
    struct parmac *p2=p;

    //goto next free bit of memory

    if(excurs) {
      while(p2->excurDown) {
        p2=p2->excurDown;
      }
    }

    p2++;

    //
    p->trsnIt->machine(p2,p->src);
    p2->prev=p;
    p->next=p2;
    // *stk=p2;

    PARMAC_DEBUG_STEPS_PRINTF("=trying machine '%s'\n",p2->name);

    *pp=p2; return true;//p2;
  }

  //======> is an event
  if(p->trsnIt->fromState==p->state && p->trsnIt->event) {
    const char *eventName;
    const char *eventRet=p->trsnIt->event(p->src,&eventName,data);
    const char *startSrc=p->src;

    //
    PARMAC_DEBUG_STEPS_PRINTF("=trying event '%s'\n",eventName);

    //=========> event failed
    if(eventRet==NULL) {

      //event failed, try next transition
      p->trsnIt++;

      //

      *pp=p; return true;//p;
    }

    //=========> event successful

    //
    PARMAC_DEBUG_STEPS_PRINTF("=event success\n");

    //
    parmac_on_event_success(p,p->src,eventRet,excurs,data);

    //
    *pp=p; return true;//p;

  }

  //======> no machine or event specified
  if(p->trsnIt->fromState==p->state) {

    //
    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");

    //
    parmac_on_event_success(p,p->src,p->src,excurs,data);

    //

    *pp=p; return true;//p;
  }

  //===> transition fromState doesn't match state
  {
    PARMAC_DEBUG_STEPS_PRINTF("=incr trsn\n");
    //increment transition iterator
    p->trsnIt++;

    *pp=p; return true;//p;
  }
}
