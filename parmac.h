#ifndef PARMAC_H
#define PARMAC_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

// #define PARMAC_DEBUG_STEPS
#define PARMAC_DEBUG_CALLBACKS
// #define PARMAC_DEBUG_INSTANCES

// #define PARMAC_NO_EXCUR

struct parmac;

typedef const char *(*parmac_event)(const char*,bool*,const char**);
typedef void (*parmac_machine)(struct parmac*,const char*);
typedef void (*parmac_state_enter)(const char*,const char*,bool,
                                        const char**,const char**);
//
typedef void (*parmac_state_leave)(const char*,const char*,bool);

struct parmac_state {
  const char *name;
  parmac_state_enter enter;
  parmac_state_leave leave;
};

struct parmac_transition {
  const struct parmac_state *state,*toState;
  parmac_event event;
  parmac_machine machine;
};

struct parmac_expecting {
  const char *msg;
  struct parmac_expecting *next;
};

struct parmac {
  const char *src;
  const struct parmac_transition *trsnStart,*trsnEnd,*trsnIt;
  const struct parmac_state *state,*startState, *endState;

  struct parmac *prev,*next; //here
#ifndef PARMAC_NO_EXCUR
  struct parmac *excurUp,*excurDown; //here
#endif
  const char *name;
  const char *markStart,*markEnd;
  struct parmac_expecting *expecting; //here
};

#ifdef __cplusplus
extern "C" {
#endif
  struct parmac *parmac_set(struct parmac *p,const char *name,const char *src,
                            const struct parmac_state *startState,
                            const struct parmac_state *endState,
                            const struct parmac_transition *startTrsn,
                            const struct parmac_transition *endTrsn);



  struct parmac *parmac_run(struct parmac *p,void *data,bool *err,char *errMsg);
#ifdef __cplusplus
}
#endif

#endif

/*
Notes:
* there is no backtracking, if a machine has transitioned past the start state, any failure to get to the end state will cause the parsing to fail
* after a machine transitions from the start state, it will go back through the machines on the stack and call the trsn->state->leave(), starting from the first (bottom) to the one just before the current machine (top)
* trsn->toState->enter is called after the state has been transitioned to
* trsn->state->leave is called just before the successful transition to trsn->toState
* if a trsn contains a machine, the trsn->toState->enter will be called after the containing machine has finished

Notes2:
* transitions to the end state do not use any event or machine
* to transition to the end state, there must be a state that can be transitioned to outside the current machine
* on transition to end state of root machine (p->prev==NULL), then check if any excurs have any paths, by iterating them until all trsnEnd then go back to root and transition to end, else abort root toEnd and use the transition found
* some enter/leave callbacks will not be called where the parse fails, due to some enter/leave callbacks only being called after a parse further on succeeds
* the need for excursions is for recursive submachines so that a state(A) cannot be transitioned into if a machine lower on the stack can transition into a state(B) at that point, which is done by putting a transition to an end state first, and then the state(A) after (e.g. "puts a]" works and "puts [return a]]" shouldn't)



* add check so toState cannot be a startState
*/


/*
 * Usage:
 * * start and end states in a machine cannot be the same state
 * * cannot have a start to start/end transition
 * * transitions to end states ignore any event/machine specified
*/
