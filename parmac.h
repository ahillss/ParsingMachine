#ifndef PARMAC_H
#define PARMAC_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct parmac;
struct parmac_state;

typedef const char *(*parmac_event)(const char *src,const char **name,void *data);

typedef void (*parmac_machine)(struct parmac *p,const char *src);

typedef void (*parmac_state_enter)(const struct parmac_state *fromState,
                                   const struct parmac_state *toState,
                                   const char *srcStart,
                                   const char *srcEnd,
                                   void *data);

typedef void (*parmac_state_leave)(const struct parmac_state *fromState,
                                   const struct parmac_state *toState,
                                   void *data);

struct parmac_state {
  const char *name;
  parmac_state_enter enter;
  parmac_state_leave leave;
};

struct parmac_transition {
  const struct parmac_state *fromState,*toState;
  parmac_event event;
  parmac_machine machine;
};

struct parmac {
  const char *name,*src;
  const struct parmac_transition *trsnStart,*trsnEnd,*trsnIt;
  const struct parmac_state *state,*startState,*endState;
};

#ifdef __cplusplus
extern "C" {
#endif

  struct parmac *parmac_set(struct parmac *p,
                            const char *name,
                            const char *src,
                            const struct parmac_state *startState,
                            const struct parmac_state *endState,
                            const struct parmac_transition *startTrsn,
                            const struct parmac_transition *endTrsn);

  bool parmac_run(struct parmac *stk,unsigned int *pDepth,void *data,bool *err);

#ifdef __cplusplus
}
#endif

#endif
