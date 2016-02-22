* make sure when on excursion on the root machine to skip the endstate transitions and go back after reaching the end of the transitions (code is already there, just to need to place it in the correct place and expand it for excursions, replacing what is there currently)
* add markStart, markEnd as parameters to the event function prototype
* make error messages use preallocated memory like the parser object already does
* add callback for error messages
* maybe instead of the prev/next pointers, store integers which when added to the root machine's pointer will return the right pointer, so that if the memory is reallocated, the prevs/nexts/etc will never be incorrect
* add col/row int pointers to event function as well
* when an excursion is successful a cavity is formed in the preallocated memory block, make sure to move the current machine to the start of that cavity
