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
  p->markStart=src;//(*stk)?(*stk)->markStart:src;
  p->markEnd=src;//(*stk)?(*stk)->markEnd:src;

  p->expecting=NULL;
  

#ifndef PARMAC_NO_EXCUR
  p->excurUp=NULL;
  p->excurDown=NULL;
#endif

  return p;

}


struct parmac *parmac_pop(struct parmac *p) {
#ifdef PARMAC_DEBUG_INSTANCES
  parmac_debug_count++;
  printf("pop_inst_p=%i\n",parmac_debug_count);
#endif
  // printf("X\n");
   // struct parmac *tmp=(*stk)->prev;

   // if(tmp) {
  // tmp->next=NULL;
  // }
  if(p->prev) {
    p->prev->next=NULL;
  }
  // free(*stk);
  // *stk=tmp;

  return p->prev;
}


// void print_parse_depth(struct parmac *p,int i) {
//   while(i>0) {
//     PARMAC_DEBUG_STEPS_PRINTF("  ");
//     i--;
//   }

//   while(p) {
//     PARMAC_DEBUG_STEPS_PRINTF("  ");
//     p=p->prev;
//   }
// }

void parmac_print_parse_pos(struct parmac *p,bool end) {
  if(p->prev) {
    parmac_print_parse_pos(p->prev,false);
  } else {
    PARMAC_DEBUG_STEPS_PRINTF("\n");
  }


     PARMAC_DEBUG_STEPS_PRINTF("/ %s : %s (%s -> %s) (%i %i)",p->name,
                               p->state->name,
                               (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->state->name,
                               (p->trsnIt==p->trsnEnd)?"X":p->trsnIt->toState->name,
							   
#ifndef PARMAC_NO_EXCUR
                               p->excurUp?1:0,
                               p->excurDown?1:0
#else
                                0,0
#endif
                               );

     if(end) {
     PARMAC_DEBUG_STEPS_PRINTF("\n");
     }
}

void parmac_free_expectings(struct parmac_expecting *e) {
  while(e) {
    struct parmac_expecting *tmp=e->next;
    free(e);
    e=tmp;
  }
}

#ifndef PARMAC_NO_EXCUR
struct parmac *parmac_excur_up(struct parmac *p) {
  // struct parmac *p;

  // p=*stk;

   if(p->prev) {
     p->prev->next=p->excurUp->prev;
   }

  p=p->excurUp;
  // *stk=p;
  return p;
}

struct parmac *parmac_excur_down(struct parmac *p) {
  // struct parmac *p=*stk;
  p=p->excurDown;
  // *stk=p;

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

  // struct parmac *p;
  // p=(struct parmac*)malloc(sizeof(struct parmac));


  struct parmac *cur;//=*stk;
  struct parmac *prev;//=cur->prev;

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

  p->markStart=prev->markStart;
  p->markEnd=prev->markEnd;
  p->expecting=NULL;

  // *stk=p;
  return p;
}
 
 #endif

void parmac_run_state_enter(struct parmac *p,
                                 const struct parmac_state *state,
                                 const char *srcStart,
                                 const char *srcEnd,
                                 bool first) {
#ifdef PARMAC_DEBUG_CALLBACKS
  if(first) {
    p->markStart=srcStart;
  }

  p->markEnd=srcEnd;

  printf("- %s_%s_enter(%i) '%.*s'\n",p->name,
         state->name,first,srcEnd-srcStart,srcStart);
#else
  state->enter(srcStart,srcEnd,first,&p->markStart,&p->markEnd);
#endif
}

void parmac_run_state_leave(struct parmac *p,
                                 const struct parmac_state *state,
                                 bool dif) {
#ifdef PARMAC_DEBUG_CALLBACKS
 printf("- %s_%s_leave(%i) '%.*s'\n",p->name,state->name,dif,
         p->markEnd-p->markStart,p->markStart);
#else
  state->leave(p->markStart,p->markEnd,dif);
#endif
}

#ifndef PARMAC_NO_EXCUR
void parmac_on_event_success_excurs(struct parmac *p) {

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
      bool dif=p2->trsnIt->state!=p2->trsnIt->toState;

      PARMAC_DEBUG_CALLBACKS_PRINTF("i");
      parmac_run_state_enter(p2,p2->trsnIt->state,p3->src,p2->src,dif);
    }

    //excur prev state, on leave
    PARMAC_DEBUG_CALLBACKS_PRINTF("h");
    parmac_run_state_leave(p2,p2->trsnIt->state,true);


    // excur end state, on enter
    PARMAC_DEBUG_CALLBACKS_PRINTF("g");
    parmac_run_state_enter(p2,p2->endState,p2->src,p2->src,true);

    // excur end state, on leave
    PARMAC_DEBUG_CALLBACKS_PRINTF("f");
    parmac_run_state_leave(p2,p2->endState,true);

    //
    struct parmac *p3=p2;
    p2=p2->excurDown;
  }

  PARMAC_DEBUG_CALLBACKS_PRINTF("--\n");
}
#endif

void parmac_on_event_success(struct parmac *p,
                             const char *srcStart,
                             const char *srcEnd) {

#ifndef PARMAC_DEBUG_STEPS
  PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

  //
#ifndef PARMAC_NO_EXCUR
  if(p->excurUp) {
    parmac_on_event_success_excurs(p);


  } else {
    struct parmac *p2=p;

    //
    while(p2->prev && !p2->excurUp) {
      p2=p2->prev;
    }

    //
    if(p2->excurUp) {
      parmac_on_event_success_excurs(p2);

      //
      struct parmac *p3=p2->excurUp->prev;
      bool dif=p3->trsnIt->state!=p3->trsnIt->toState;

      PARMAC_DEBUG_CALLBACKS_PRINTF("k");
      parmac_run_state_enter(p2,p2->state,p3->src,p2->src,dif);


    }
  }
#endif

  //prev start states
  if(p->trsnIt->state==p->startState) {
    struct parmac *p2=p;



    //to bottom
    while(p2->prev && p2->trsnIt->state==p2->startState) {
      p2=p2->prev;
    }

    //to top
    while(p2 && p2!=p) {
      //prev machines start state, on enter
      if(p2->trsnIt->state==p2->startState) {
        PARMAC_DEBUG_CALLBACKS_PRINTF("e");
        parmac_run_state_enter(p2,p2->startState,p2->src,p2->src,true);
      }

      //prev machines, start state, on leave
      {
        bool dif=p2->trsnIt->state!=p2->trsnIt->toState;

        PARMAC_DEBUG_CALLBACKS_PRINTF("d");
        parmac_run_state_leave(p2,p2->trsnIt->state,dif);
      }


      p2=p2->next;
    }
  }

  //prev start state, on enter
  if(p->trsnIt->state==p->startState) {
    PARMAC_DEBUG_CALLBACKS_PRINTF("c");
    parmac_run_state_enter(p,p->startState,p->src,p->src,true);
  }

#ifndef PARMAC_NO_EXCUR
  //prev state, on enter
  if(p->excurUp) {
    struct parmac *p2=p->excurUp->prev;
    bool dif=p2->trsnIt->state!=p2->trsnIt->toState;

    PARMAC_DEBUG_CALLBACKS_PRINTF("j");
    parmac_run_state_enter(p,p->state,p2->src,p->src,dif);
  }
#endif

  //prev state, on leave
  {
    bool dif=p->trsnIt->state!=p->trsnIt->toState;

    PARMAC_DEBUG_CALLBACKS_PRINTF("b");
    parmac_run_state_leave(p, p->trsnIt->state,dif);
  }

  //cur state transition, on enter
  {
    bool dif=p->trsnIt->state!=p->trsnIt->toState;

    PARMAC_DEBUG_CALLBACKS_PRINTF("a");
    parmac_run_state_enter(p,p->trsnIt->toState,srcStart,srcEnd,dif);
  }

  //change state
  p->state=p->trsnIt->toState;
  p->src=srcEnd;
  p->trsnIt=p->trsnStart;
  
#ifndef PARMAC_NO_EXCUR
  p->excurUp=NULL;
  p->excurDown=NULL;
#endif

  PARMAC_DEBUG_STEPS_PRINTF("==aaa\n");

  //pop error messages
  parmac_free_expectings(p->expecting);
  p->expecting=0;


}



struct parmac *parmac_run(struct parmac *p,void *data,bool *err,char *errMsg) {
  // struct parmac *p=*stk;

  //===> on empty stack, stop and do nothing
  if(!p) {
    return 0;
  }

  //
#ifdef PARMAC_DEBUG_STEPS
  parmac_print_parse_pos(p,true);
#endif

#ifndef PARMAC_NO_EXCUR
  //===>excur down, toEnd, not root, push excur
  if(!p->excurDown && p->prev &&
     p->trsnIt!=p->trsnEnd &&
     p->trsnIt->state==p->state &&
     p->trsnIt->toState==p->endState) {
    PARMAC_DEBUG_STEPS_PRINTF("=push excur\n");
    p=parmac_push_excur(p);
    return p;
  }

  //===>excur down, trsnEnd, pop excur
  if(p->excurUp && !p->excurDown && p->trsnIt==p->trsnEnd) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns, pop excur\n");
 
    if(p->prev) {
       p->prev->next=p->excurUp->prev;
     }

    p=p->excurUp;
    p->trsnIt++;
    return p;
  }

 //===>excur down, toEnd, root, go up
  if(p->excurUp && !p->prev &&
     p->trsnIt!=p->trsnEnd &&
     p->trsnIt->state==p->state &&
     p->trsnIt->toState==p->endState) {
    p=parmac_excur_up(p);

    PARMAC_DEBUG_STEPS_PRINTF("=bottom at end, go up excur\n");
    return p;
  }

  //===>excur up, toEnd, iterate trsn
  if(p->excurDown &&
     p->trsnIt!=p->trsnEnd &&
     p->trsnIt->state==p->state &&
     p->trsnIt->toState==p->endState) {
    p->trsnIt++;

    PARMAC_DEBUG_STEPS_PRINTF("=to end, iterate trsn\n");
    return p;
  }

  //===>excur up, trsnEnd, go up
  if(p->excurUp &&
     p->excurDown &&
     p->trsnIt==p->trsnEnd) {

    p=parmac_excur_up(p);
    PARMAC_DEBUG_STEPS_PRINTF("=trsnEnd, go up excur\n");

    return p;
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
    PARMAC_DEBUG_CALLBACKS_PRINTF("w");
    parmac_run_state_leave(p,(p->trsnIt-1)->state,true);

    while(p) {
      PARMAC_DEBUG_CALLBACKS_PRINTF("m");
      parmac_run_state_enter(p,p->endState,p->src,p->src,true);

      PARMAC_DEBUG_CALLBACKS_PRINTF("n");
      parmac_run_state_leave(p, p->endState,true);

      //
      if(p->prev) {
        struct parmac *p2=p->prev;
        {
          bool dif=p2->trsnIt->state!=p2->trsnIt->toState;
          PARMAC_DEBUG_CALLBACKS_PRINTF("o");
          parmac_run_state_enter(p2,p2->trsnIt->toState,p2->src,p->src,dif);
        }


        PARMAC_DEBUG_CALLBACKS_PRINTF("p");
        parmac_run_state_leave(p2, p2->trsnIt->toState,true);
      }

      //
      p=p->excurDown;
    }

    return NULL;
  }
#endif

  //===> successfully transitioned to end state, without excur
  if(p->state==p->endState) {
    PARMAC_DEBUG_STEPS_PRINTF("=at end\n");

#ifndef PARMAC_DEBUG_STEPS
  PARMAC_DEBUG_CALLBACKS_PRINTF("\n");
#endif

    PARMAC_DEBUG_CALLBACKS_PRINTF("x");
    parmac_run_state_leave(p,p->state,true);

    //pop error messages
    parmac_free_expectings(p->expecting);
    p->expecting=NULL;

    //pop machine
    p=parmac_pop(p);

    //temporary
    if(p) {
      p->trsnIt++;
    }
    //
    return p;
  }

  //===>root, trsnEnd, not startState
  if(p->trsnIt==p->trsnEnd &&
     p->state!=p->startState &&
     !p->prev) {

    const struct parmac_transition *t;
    t=p->trsnEnd-1;

    while(t!=p->trsnStart && t->toState!=p->endState) {
      t--;
    }

    if(t->toState!=p->endState) {
      PARMAC_DEBUG_STEPS_PRINTF("=root toEnd\n");
      parmac_on_event_success(p,p->src,p->src);
      return NULL;
    }
  }

  //===>trsnEnd, not startState
  if(p->trsnIt==p->trsnEnd &&
     p->state!=p->startState) {

    //
    struct parmac_expecting *e=p->expecting;

    while(e) {
      PARMAC_DEBUG_STEPS_PRINTF("\n%s\n",e->msg);
      e=e->next;
    }

    //
    parmac_free_expectings(p->expecting);

    //
    p=parmac_pop(p);

    //
    return NULL;
  }

  //===>trsnEnd, startState
  if(p->trsnIt==p->trsnEnd) {
    PARMAC_DEBUG_STEPS_PRINTF("=no trsns found\n");

    //pop stack
    p=parmac_pop(p);

    //increment transition iterator
    if(p) {
      p->trsnIt++;
    }

    //
    return p;
  }


  //======> root, toEndState
  if(p->trsnIt->state==p->state &&
     p->trsnIt->toState==p->endState &&
     !p->prev) {
    PARMAC_DEBUG_STEPS_PRINTF("=root toEnd skip\n");
    p->trsnIt++;

    // PARMAC_DEBUG_STEPS_PRINTF("=root toEnd\n");
    // parmac_on_event_success(p,p->src,p->src);
    return p;
  }

  //======> is a machine
  if(p->trsnIt->state==p->state && p->trsnIt->machine) {

      struct parmac *p2=p;

    //goto next free bit of memory

#ifndef PARMAC_NO_EXCUR
    while(p2->excurDown) {
      p2=p2->excurDown;
    }
#endif

    p2++;

    //
    p->trsnIt->machine(p2,p->src);
    p2->prev=p;
    p->next=p2;
    // *stk=p2;

    PARMAC_DEBUG_STEPS_PRINTF("=trying machine '%s'\n",p2->name);
    return p2;
  }

  //======> is an event
  if(p->trsnIt->state==p->state && p->trsnIt->event) {
    bool eventErr=false;
    const char *eventName;
    const char *eventRet=p->trsnIt->event(p->src,&eventErr,&eventName);
    const char *startSrc=p->src;

    //
    PARMAC_DEBUG_STEPS_PRINTF("=trying event '%s'\n",eventName);

    //=========> event failed
    if(eventErr) {
      //on error message
      if(eventRet) {
        struct parmac_expecting *expecting;
        size_t s=sizeof(struct parmac_expecting);
        expecting=(struct parmac_expecting*)malloc(s);
        expecting->next=p->expecting;
        expecting->msg=eventRet;
        p->expecting=expecting;
      }

      //event failed, try next transition
      p->trsnIt++;

      //
      return p;
    }

    //=========> event successful

    //
    PARMAC_DEBUG_STEPS_PRINTF("=event success\n");

    //
    parmac_on_event_success(p,p->src,eventRet);

    //
    return p;
  }

  //======> no machine or event specified
  if(p->trsnIt->state==p->state) {

    //
    PARMAC_DEBUG_STEPS_PRINTF("=no machine or event\n");

    //
    parmac_on_event_success(p,p->src,p->src);

    //
    return p;
  }

  //===> transition fromState doesn't match state
  {
    PARMAC_DEBUG_STEPS_PRINTF("=incr trsn\n");
    //increment transition iterator
    p->trsnIt++;
    return p;
  }
}
