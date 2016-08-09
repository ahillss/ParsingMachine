##Parsing Machine

A parser written in C which uses a hierarchical finite state machine. Where the **state** *declarations*, state **enter/leave** *callbacks*, **transition** table *declarations*, **events** *functions* and **machine** *functions* are declared by the user.

The motivation for this library is to allow the creation of parsers using hierarchical FSMs without the need for a code generator.

####Usage

######State Declaration
A state is a struct that has three fields. A name used for debugging and by the state callbacks, and the enter and leave callbacks.

```C
static const struct parmac_state 
  state_start={"start",NULL,NULL},
  state_A={"A",on_enter_state_A,on_leave_state_A},
  state_B={"B",NULL,NULL},
  state_end={"end",NULL,NULL};
```

######State Enter Callback
A callback function for when a state has been entered. 'machineName' is the name of the machine passed by **parmac_set**. **fromStateName** and **toStateName** are from the first parameter of the state delclarations. The **from** is the previous state and the **to** is the entered state. **fromPos** is the position from before the state was entered, and **toPos** the position the state was entered at.

```C
void on_enter_state_A(PARMAC_DEPTH stkDepth,
                                    const char *machineName,
                                    const char *fromStateName,
                                    const char *toStateName,
                                    PARMAC_POS fromPos,
                                    PARMAC_POS toPos,
                                    void *userdata) {

}
```

######State Leave Callback
A callback function for when a state has been left. **from** is the state that has been left and **to** is the state that is being entered. This callback lacks the **fromPos/toPos**

```C
void on_leave_state_A(PARMAC_DEPTH stkDepth,
                                    const char *machineName,
                                    const char *fromStateName,
                                    const char *toStateName,
                                    void *userdata) {
}

```

######Transition Table Declaration
A transition has four fields. The to and from state pointers, and the event and machine function pointers.

A transition can either contain an event or a machine, If neither is specified then that transition will always succeed. It cannot accept both an event and a machine.

A machine must always have separate designated start and end states. The end state must always being transition to and not from. The start state must always be transitioned from and not to.

```C
  static const struct parmac_transition trsns[]={
    {&state_start, &state_A, event_A, NULL},
    {&state_A, &state_B, NULL, machine_B},
    {&state_B &state_end, NULL, NULL},
  };

```
######Event
A function representing an event, it is used in the field of a transition. The return boolean determines whether or not the event succeeds. The postPtr is a pointer to a variable containing the current parsing position. If the event returns true then the current position will be updated with the value being pointed to. The userdata is used to point to the data being parsed.

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
###### Machine
A function representing a machine. Used to both initialise the machine stack and as a field in a transition. The 'p' represents the start of the stack when initialising, and when used in a transition it represents the current stack position. The 'pos' parameter represents the current position of the parser, zero is used when initialising.

The 'parmac_set' function is called as shown below.
* the 1st and 3rd parameters must be the same as the current function's 'p' and 'pos'
* the 2nd parameter represents the machine's name which is used both in debugging and in the state callbacks
* the 4th and 5th parameters are the machine's start and end state
* the 6th and 7th parameter are the transition tables start and end pointers

```C
void root_machine(struct parmac *p,PARMAC_POS pos) {
  //usually statically declare states for the machine here...
  
  //usually statically declare the transition table  for the machine here...

  parmac_set(p,"root",pos,&state_start,&state_end,trsns,trsns+sizeof(trsns)/sizeof(struct parmac_transition));
}
```

###### Running
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

######Debugging
There are to ways to debug a machine which can be used together. 

*Debug Steps* which can be enable by defining the macro **PARMAC_DEBUG_STEPS**, this prints out the stack position and all the steps taken. This was mainly used in debugging problems with the library itself.

*Debug Callbacks* can be enabled by defining the macro **PARMAC_DEBUG_CALLBACKS**, for each enter and leave state this prints their names and parsing positions. This is useful for tracking the path taken by the fsms. 

