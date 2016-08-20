#ifndef PARMAC_H
#define PARMAC_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#if !defined(PARMAC_POS) || !defined(PARMAC_DEPTH)
#include <stdlib.h>
#endif

#ifndef PARMAC_POS
#define PARMAC_POS size_t
#endif

#ifndef PARMAC_DEPTH
#define PARMAC_DEPTH size_t
#endif

struct parmac;

typedef bool (*parmac_event)(PARMAC_POS *posPtr,void *userdata);

typedef void (*parmac_machine)(struct parmac *p,PARMAC_POS pos);

typedef void (*parmac_state_callback)(PARMAC_DEPTH stkDepth,
                                   const char *machineName,
                                   const char *fromStateName,
                                   const char *toStateName,
                                   PARMAC_POS fromPos,
                                   PARMAC_POS toPos,
                                   void *userdata);

struct parmac_state {
  const char *name;
  parmac_event event;
  parmac_machine machine;
  parmac_state_callback enter,leave;
};

struct parmac_transition {
  const struct parmac_state *fromState,*toState;
};

struct parmac {
  const char *name;
  PARMAC_POS pos,prevPos;
  const struct parmac_transition *trsnIt,*trsns;
  const struct parmac_state *state,*startState,*endState;
};

#ifdef __cplusplus
extern "C" {
#endif

  struct parmac *parmac_set(struct parmac *p,
                            PARMAC_POS pos,
                            const char *name,
                            const struct parmac_state *startState,
                            const struct parmac_state *endState,
                            const struct parmac_transition *trsns);

  bool parmac_run(struct parmac *stk,
                  PARMAC_DEPTH *stkDepthPtr,
                  void *userdata);

  bool parmac_failed(struct parmac *stk);
  PARMAC_POS parmac_last_pos(struct parmac *stk,PARMAC_DEPTH stkDepth);

#ifdef __cplusplus
}
#endif

#endif
