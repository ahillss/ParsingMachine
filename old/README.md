##Parsing Machine

A parser written in C which uses a hierarchical finite state machine. Where the states, transition tables, events and machines are declared by the user.

The purpose of this library is to allow the creation of a parser without the need for an extensive library or something like a code generator. Having only to design a FSM and represented purely in C.

####Details

It uses a **transition table** which will try different **transitions** in the order given in the table. A **transition** can occur either when either an **event** or a sub **machine** succeeds. Note after a **machine** **transitions** from a **start state**, any failure from then on will cause the **parser** to fail. Transitions to **end states** cannot use an **event** or a **sub machine**, this and the lack of backtracking is due to how **excursions** work (described below).

When a **sub machine** comes upon a **transition** to an **end state**, the machine will take what is called an **excursion**, basically it lets the machine look ahead to see what would happen if it exited at that point. If the **excursion** fails it will continue on from where it left off to try the next **transition**. Also when a **root machine** comes across a **transition** to an **end state** it will skip over it, trying the other **transitions** and come back if none of those succeed. This is useful when the **machine** used for the **root** is also used as a **sub machine** which uses **excursions**.

**Excursions** allow a **machine** to exit early, for example if a **machine** comes across a closing bracket it can return to the **machine** below to parse the closing bracket. This is useful in languages like TCL where a closing square bracket without an open square bracket is considered a normal character. In this case without **excursions** the closing square bracket would be parsed as a character in the current **machine**, and the **machine** below would be without a closing square bracket to parse, causing the **parser** to fail.

The results of the **parser** are accessed via the **enter** and **leave** **callbacks** stored in the **state** objects. The **enter callback** allows you to **mark** the start and end of a string. The **leave callback** allows you to make use of the **marked** string. **Marks** set in **sub machines** are propagated to the **machines** below, allowing you to **mark** a string in a **sub machine** and use it later on in a **machine** below. The **enter callback** has a boolean to let you know if the previous **state** was different to the current **state**, same with the **leave callback** but it lets you know for the next **state**.

**Events** are functions used to parse the input string. If it fails, it can return an **error message**. Each one is also expected to count the rows and columns it comes across which are used for **error** handling. **Events** can also make use of the **marked** strings instead of or in conjunction with parsing the input string, which can be used to check things like keywords parsed from a previous **state**/**event**. Which is helpful due to the lack of backtracking where you can't tell if a string of characters is not part of a larger word (at least not without hassle).

**Error messages** are accumulated whenever a **transition's event** fails and returns an **error message**, they are also propogated down to the **machines** below. If a **transition** succeeds, they are all cleared. If the **parser** fails, the **error messages** are accessed through a callback. **Error messages** are good for letting the end user know of things like missing closing brackets, quotes or other syntax.

The **parser** does not allocate any memory, rather a pointer to a block of memory is used (an array). If it runs out of memory a callback can be used to allocate more memory.

####Excursions
Excursions are used when you are in a sub machine which is at a transition to the end state. The excursion represents the machine below the current one if you had actually transitioned to the end state. It also behaves like a normal machine, able to push its own excursions.

1. a sub machine is currently at a transition to the end state
2. an excursion is pushed, representing the machine below the current one as if the current one had transitioned to the end state
3. an excursion can also push its own excursion, and possbily go all the way to the root machine
4. once the lowest machine that can be reached is visited, if it manages to transition to an end state (if it is the root machine then after trying all other transitions), it will go back up to the previous excursion, iterating it past the end state that it was at
5. if this previous excursion sees any more end states (only a badly designed machine would contain more than one transition to an end state from the same state), they are skipped over, and if it runs out of transitions, it will go back up to the excursion before it
6. if it can go all the way back to the initiating machine that pushed the first excursion, and it will iterate past the end state transition and popping all the excursions which were unsuccessful, so it can continue on like normal
7. if it doesn't manage to get to the initiating machine, then it either has succeeded in finding a transition or it will have failed and gone back down to the lowest machine the excursion reached and will successfully end there

####Status:
The algorithm design has been completed, but has not been fully implemented or tested yet.

Also planning on making excursions optional since they are about 50% of the algorithm and aren't necessary for simple parsers.

####TODO:
* maybe instead of the prev/next pointers, store integers which when added to the root machine's pointer will return the right pointer, so that if the memory is reallocated, the prevs/nexts/etc will never be incorrect
* when an excursion is successful a cavity is formed in the preallocated memory block, make sure to move the current machine to the start of that cavity