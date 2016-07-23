#ifndef PARMAC_H
#define PARMAC_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

enum parmac_status {
  parmac_ok,
  parmac_error,
  parmac_done
};

struct parmac;
struct parmac_state;

typedef const char *(*parmac_event)(const char *src,void *userdata);

typedef void (*parmac_machine)(struct parmac *p,const char *src);

typedef void (*parmac_state_callback)(unsigned int depth,bool dif,
                                      const char *srcStart,const char *srcEnd,
                                      void *userdata);

struct parmac_state {
  const char *name;
  parmac_state_callback enter,leave;
};

struct parmac_transition {
  const struct parmac_state *fromState,*toState;
  parmac_event event;
  parmac_machine machine;
};

struct parmac {
  const char *name,*src,*prevSrc;
  const struct parmac_transition *trsn,*trsnStart,*trsnEnd;
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

  enum parmac_status parmac_run(struct parmac *stk,unsigned int *pDepth,void *userdata);

#ifdef __cplusplus
}
#endif

#endif
