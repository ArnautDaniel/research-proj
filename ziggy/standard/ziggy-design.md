Thoughts in this file go toward trying to understand the solution to a most beguiling problem;  the art of storing things and retrieving them.

Requirements for Ziggy:
	     Allow for write-many by imagining a SPLIT command
	     Atomicity is another word for faulty design
	     Ghost variables should exist
	     We should be able to ask "what if" and not only
	     	"what is?".
	     Parallelization is primitive
	     Simplicity is the key to understanding


What is Ziggy?:

     Ziggy is a event-source database written in the form of N-state machines which encapsulate the changes in values by running a virtual machine where all operations are performed in a forth-like language.

The Ziggy State Machine:

    The state machine is based off of a 1-stack machine.  There are 5 registers that allow operations to be performed.  Instructions are not only indexed by number but also by time itself (FIX: And tags).  The state machine's memory is stored in plaintext as operations in a Assembly/Forth like language which can be manipulated in ways corresponding to the virutal machines instruction set before being run through a small C interpreter.
    Every variable is itself a stack machine.  Operations are saved,  not values (except for a possible initial value).
    Stack machines are also file descriptor monsters.  A Ziggy VM's input is either a generic file descriptor that operates on the furthest bound of memory, or an output file descriptor where the VM outputs it's current value, or an array of all the values it's ever computed.  Earlier memory is never modified unless the STTO (stack to) command is called.  Ziggy VM's are orthrogonical to each other in that one stack machine can connect the input to another stack machine, and vice versa.  In this way not only are current values connected but every possible combination of the machine's computations can be summoned.
    Typically this would be based on time.
    Ziggy VM's are divisible,  in that any VM can make a copy of itself and run a computation different from the main one.  A risky JOIN instruction can be used to rebase two VM's into each other.  This isn't as dangerous necessarily because the original values of both VM's are stored in the new one.

The Willis Supervisor:

    Every Ziggy needs his Willis.  Willis is the overarching system responsible for scheduling VM's to run,  indexing them,  storing them,  finding them, and implementing the ZiggyForth language.  Willis is complemented by a Mini-Prolog like query language.  The query language is responsible for implementing the ability to ask "what if";  the "what is", "what could be", and "what has been" are covered by ZiggyForth.

The Willis Primitives:

    Willis is only aware of 4 things.
    	   Strings
	   Integers
	   Floats
	   Objects

   Primitives lie in a heirarchy of light hearted OOP.  This means objects contain general methods for every "thing" which can get specialized on the actual object.  Essentially,  sequences are generic,  calling the method they actually need based off of a tagging system and specialized methods.  This should be fast enough since the total operations should lie under 50. (FIX:  Type based function dispatch instead of OOP?)

The Playground:
    Every VM is forked into a playground when an input socket is connected to it.  This means the original VM is guaranteed to be untouched.  The VM in the playground can be full (all commands since origin) or partial (all commands since a certain point) or current (the current register values with a fresh memory bank).  This also means we get partial write-many (many people writing at once) for free.
    For instance we might imagine a Ziggy VM "A" which contains the word
    	"Hello" as it's current computation.

	We may connect a socket to this which puts it in the playground by wrapping the original VM in a SPLIT command.
	This means an object with "Hello" is now in the playground.
	During this time we may have another operation connect to VM "A".

	Our current playground would then look like:
	    "HELLO" (B)
	    "HELLO" (C)
	Now imagine we call an APPEND on both with different words such that we are left with.
	
	    "WORLD APPEND"  -> "HELLO WORLD"  (B)
	    "DENNIS APPEND" -> "HELLO DENNIS" (C)
	    
	Currently we are now in an untenable position since both values are completely different.  If we were to do nothing, and simply call JOIN once the socket disconnected we would end up with this situation.  (If C disconnected last)
	
	    "HELLO WORLD" (A)
	    ... More computations/time
	    "HELLO DENNIS" (A) <--- (current value)
	    
	    In a sense,  this is wrong in one way and right in another.  It is true that since C disconnected last it should have the prima value.  It is also true that the computation for "HELLO WORLD" was not lost but was grafted into the memory bank.  However,  if both VM's B and C called append then depending on time the value should have been...
	    
	    "HELLO WORLD DENNIS"
	    	   or
            "HELLO DENNIS WORLD"

	    There are a couple of routes we could take with this.  For one, we could lie down and accept that time is nuisance.  For another we could create commands in the VM that could specify which value is prime.  For instance,  since we save computations it would be no big deal to say a word "ALPHA" or "BETA" exists.
	    
	    "ALPHA":  Clone the VM,  perform operations,  when regrafted ensure that the first steps run by the new memory bank are the ones returned by this VM.
	    "BETA": Clone the VM,  perform operations,  when regrafted if other commands exist, come after them.

	    In this way,  we could assign priority to our operations such that we are no longer operating on the actual value,  but simply the functions that need to be called on the value,  and in what priority the functions should be performed.
	    Another way we could handle this is to agree to always hand the programmer back an array of values and never an actual value.  In this way we could offer two values when he asks for one.  The resolution of the conflict could come from the user side without effecting data in the VM.

	    In some cases we may find ourselves not caring what order the operations come in.  For instance if the operations performed are of the same type,  then necessisarily it can be proven their order does not matter.

	    (2 +) -> (2) A
	    (3 +) -> (2) B
	    ...
	    (2 + 3 +) -> 7 C
	    (3 + 2 +) -> 7 C

	    Either order of operations results in the same values.  This holds true as long as we do not mix unlike functions.

	    The real question is how do we deal with reads during this time?

	    If items are being worked on in the playground,  which item do we hand back on a read?
	    Essentially we would have to mark a VM as being in the playground, and either wait or hand back the current value.  The other option would be to redirect a read into the playground VM and reconnect the output to the playground VM's output.
	    However,  we would still benefit from the individuality of variables because reads on other parts of the Willis Object would still be free to work.

The Nature of Willis Objects:

    Willis Objects are structures that can be predefined.
    They can contain any number of elements in any type.
    { Obj NUM NUM STRING NUM STRING }
    Each variable is it's own VM.
    Since each type is known on creation, the VM only needs to have knowledge of the methods that operate on it's type.  This means type checking only has to take place on the input.
    Objects can be dependant.  Consider an object that stores a VM for a state's tax rate.  And another that stores a invoice amount for a sale.  The output of the invoice VM can be dependant by connecting it's output to the input of the state tax rate VM.  Thus if the states tax rate is changed the computation proceeds as normal.  (invoice) -> (tax rate) -> output.  The DEPEND command should be able to specify another object and a TIME of when the computation should enter.


Some VM instructions:
     DEPEND JOIN SPLIT ADD MULTIPLY SUBTRACT DIVIDE
     APPEND REVERSE ALPHA BETA TIME STACKTO SPLIT
     SUBSEQ DUPINTO REPLACE DESTROY!! CONVERTTO
     OUTPUT STRING-NTH EXPT PRIORITY

META VM instructions (1+ variables of the same type)
     MAP REDUCE EACH

Meta Objects:

     Willis objects are meta objects containing variable VMs.  This means we can ask the Willis system to perform higher order functions on tagged objects. For instance we may ask Willis something like
     ACCOUNTING PAYOUT 3 > [ 2 * ] MAP to perform a [ 2 * ] operation on all objects with the accounting tag with a payout VM that currently has a greater than 3 value.
     But in this system we might also want to ask:
     FAUX 02-28-90 02-29-91 TIME ACCOUNTING PAYOUT 3 > [ 2 * ] MAP
     FAUX tells the Willis system to not actually change the values.  TIME takes two dates and only operates on computations that occured during that time frame, the rest is the same.
     
     Essentially what we propose to do is create a database that marries the idea of "event-sourcing" with the concatenative ideals of Forth.  In this manner we challenge the concepts of time, and remove the importance from the variables themselves onto the process that brought them into being.  Functions are essentially serial killers and thus we have Ghost Variables.
    
	     

More on Alpha/Beta Queues:

	The meta runtime for Willis is the Alpha/Beta queue.   An Alpha/Beta queue is a queue supporting both dependency graphing and out of order execution through Ghost Variables.  
	We may imagine a simple queue as the sole SPLIT of a VM A
	| PLAYGROUND |
	| Queue Num | TAG | Dependany Graph |
	| #1	    | INT | <#pointer>	    | -->  | VM "B" |
	The most important part is the dependancy graph which is a function that traces all Ziggy VM's currently in use.  A simple algorithm for the case where another SPLIT is called on VM A is as follows...
	1. Is VM "C"'s parent contained in the playground? 
	2. If so append to current queue
	3. If not create new queue

	In this case our queue now looks like this:
	--> | VM "B" | VM "C"
	Each corresponding "|" is an actual function that handles rerouting sockets/fd's and handling write output. (However it is imaginary syntactically)
	
	We now imagine that VM "B" runs and outputs a new stack which gets rerouted into VM "C".  
	VM "A" | STACK: 3
	VM "B" | INSTR: 3 +
	VM "B" | STACK: 6
	VM "C" | STACK: 6
	VM "C" | INSTR: 2 +
	VM "C" | STACK: 8
	
	In this case due to rereouting C should start it's VM with a new stack corresponding to the output of B.  But what if it matters which order these go in?  In this case we may assign C to being ALPHA and B to being BETA. 
	It's easy enough to imagine C going first and then B,  but what if B has already computed right as C enters the queue?
	We need a way to reverse instructions with a new stack.  In this case C will be given the original stack from the origin VM. But then we end up with a situation like this:

	VM "B" | STACK: 6
	VM "C" | STACK: 3
	VM "C" | INSTR: 2 +
	VM "C" | STACK: 5
	And what if the call to B is actually "3 *"
	What we need is to backtrack and call B again with the new Ghost Variable from C.
	In essence the queue becomes this linear thread of changes being made at any time to the original VM without affecting it.  
