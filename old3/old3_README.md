#Parsing Machine

A parser written in C which uses a hierarchical finite state machine. Where the **state** declarations, state **enter/leave** callbacks, **transition** table declarations, **events** functions and **machine** functions are declared by the user.

The motivation for this library is to allow the creation of parsers using hierarchical FSMs without the need for a code generator.

##Usage

####State Declaration
A state is a struct that has three fields. A *name* used by the state callbacks and for debugging. An *event* and a *machine* function pointers. An optional *enter* callback and an optional *leave* callback. 

A state can either contain an event or a machine, if neither is specified then that transition will always succeed. It cannot accept both an event and a machine.

When a transition comes across a state using a machine instead of an event, the sub machine will be pushed on to the stack. If the sub machine manages to reach its end state, then the parent machine's transition will suceed and will transition to the *toState* specified. If the sub machine does not manage to transition past its start state then the parent machine's transition will have failed and will continue on with the next transition. But if the sub machine does manage to get past its start state but fails from there after, then the parser will fail (so there would be no backtracking to complicate the implementation of state callbacks).

```C
static const struct parmac_state 
  state_start={"start", NULL,NULL, NULL,NULL},
  state_A={"A", event_A,NULL, on_enter_state_A,on_leave_state_A},
  state_B={"B", NULL,machine_B, NULL,NULL},
  state_end={"end", NULL,NULL, NULL,NULL};
```

####Enter State Callback
The *from state* is the previous state being left and the *to state* is the state being entered. The *fromPos* parameter is the position from before the state was entered, and *toPos* parameter is the position after the state was entered.

```C
void on_enter_state_A(PARMAC_DEPTH stkDepth,
                                    const char *machineName,
                                    const char *fromStateName,
                                    const char *toStateName,
                                    PARMAC_POS fromPos,
                                    PARMAC_POS toPos,
                                    void *userdata) {
  const char *src=(const char*)userdata;
  printf("on enter: '%.*s'\n",(int)(toPos-fromPos),&src[fromPos]);

}
```

####Leave State Callback
The *from state* is the state being left and the *to state* is the next state being entered.

```C
void on_leave_state_A(PARMAC_DEPTH stkDepth,
                                    const char *machineName,
                                    const char *fromStateName,
                                    const char *toStateName,
                                    PARMAC_POS fromPos,
                                    PARMAC_POS toPos,
                                    void *userdata) {
  const char *src=(const char*)userdata;
  printf("on leave: '%.*s'\n",(int)(toPos-fromPos),&src[fromPos]);
}

```

####Transition Table Declaration
A transition has four fields. The to and from state pointers, and the event and machine function pointers.

A machine must always have a separate designated start and end states. The end state must always being transition to and not from, and the start state must always be transitioned from and not to.

```C
  static const struct parmac_transition trsns[]={
    {&state_start, &state_A},
    {&state_A, &state_B},
    {&state_B &state_end},
  };

```

####Event
A function representing an event, it is used in a *transition*. The return boolean determines whether or not the event succeeds. The *postPtr* is a pointer to a variable containing the current parsing position. If the event returns true then the current position will be updated with the value being pointed to. The *userdata* is used to point to the data being parsed.

```C
bool event_A(PARMAC_POS *posPtr,void *userdata) {
  const char *src=(const char*)userdata;
  
  if(src[*posPtr]=='A') {
    (*posPtr)++;
    return true;
  }
  
  return false;
}

```
#### Machine
A function representing a machine. Used to both initialise the machine stack and optionally in a *transition*. The *p* represents the start of the stack when initialising, and when used in a transition it represents the current stack position. The *pos* parameter represents the current position of the parser, zero is used when initialising.

The *parmac_set* function must be called as shown below.
* the 1st and 2nd parameters must be the same as the function's parameters *p* and *pos*
* the 3rd parameter represents the machine's name which is used both in debugging and in the state callbacks
* the 4th and 5th parameters are the machine's start and end state
* the 6th and 7th parameter are the transition table's start and end pointers

```C
void root_machine(struct parmac *p,PARMAC_POS pos) {
  //... statically declare states for the machine here ...
  
  //... statically declare the transition table for the machine here ...

  static const struct parmac_transition *trsnsEnd=trsns+sizeof(trsns)/sizeof(struct parmac_transition);
  
  parmac_set(p,pos,"root",&state_start,&state_end,trsns,trsnsEnd);
}
```

####Running
The *stkDepth* must be intialised to zero. The *stk* must be initialised with the root machine and the parsing position initialised to zero. The *stk* must be large enough to contain the max depth of the hierahical FSM specified.

If the machine is recursive (i.e. no max depth) then the stack must always have at least a max depth of one past the current depth. Then in the while loop check if the stack depth==maxDepth and if so then you must resize the stack before calling *parmac_run* again, otherwise you may get a stack overflow.

The *parmac_run* first parameter is the stack, the second is a pointer to the stack depth which will be used by the parser to keep track of the current depth, and the last parameter is a pointer to the data being parsed.

The *parmac_failed* is used to determine whether the machine failed or succeeded and the *parmac_last_pos* returns the parsing position that was reached last.

```C

int main() {
  
  const char *src="ABBB";
  
  PARMAC_DEPTH stkDepth=0; //init stk depth to 0
  struct parmac stk[2];
  
  root_machine(stk,0); //init stk, with root machine and src pos at 0
  
  while(parmac_run(stk,&stkDepth,(void*)src) {
  }
  
  if(parmac_failed(stk)) {
    printf("Did not reach the end state on root machine.\n");
  } else if(src[parmac_last_pos(stk,stkDepth)]!='\0') {
    printf("Did not parse entire src.\n");
  } else {
    printf("success!\n");
  }
  
  return 0;
}
```

####Debugging
There are two ways to debug a machine which can be also be used together. 

*Debug Steps* which can be enable by defining the macro **PARMAC_DEBUG_STEPS**, this prints out the stack position and all the steps taken. This was mainly used in debugging problems with the library itself, though it can be useful to get an idea of why your FSM may not be behaving as expected.

*Debug Callbacks* can be enabled by defining the macro **PARMAC_DEBUG_CALLBACKS**, for each enter and leave state this prints their names and parsing positions. This is useful for tracking the path taken by the FSM.


####Examples

There are two examples, a basic CSV parser and a TCL parser. A TCL expr parser is currently being worked on.
