Narflung's Macro Permutation Engine [DVL Version]

Overview:
This engine was created to change the physical order of the macro,
while keeping the same logical order.  Using this technique, scanning 
(but not emulation) of the macro would become increasingly difficult.

Methology:
The engine mutates the physical macro by the use of "Goto" statements.
The labels are of random length and content, thus acting as some other
macro engines do.

Permutation Process:
1: The engine generates a random, but controlled, number to place
   the first "Goto" statement.
2: It generates another number, being the number of "Goto" statements
   to add in this execution.
3: The engine generates the length of the labels for this particular
   execution.
4: The engine generates 3 labels.
5: It reads into the buffer the line of code it will move.
6: It checks to see if the line is "movable", meaning it will not affect
   the logical execution of code if moved.
7: The engine deletes the line.
8: Now the engine creates the "Goto" statements and labels in the form of:

	goto a:
	b:

	...
	[block of unmoved code]
	...

	goto c:
	a:
	[line of code]
	goto b:
	c:

9: Then it loops and repeats for the rest of the permutations.

Limitations:
This engine has many limitations, and is more a theoretical exercise or
idea for others to take up on, than an engine designed for real use.

1: This engine (so far) can only work in one sub, which certainly isnt
   practical for real macro viruses.
2: This engine will expand the size of the macro greatly within just a few
   generations.  A way to fix this would be to trace back through the code,
   then restore the original, unpermuted code, and then permute, as opposed
   to permuting permuted code (which is what the current version does.)
3: Occasionally a runtime error (Run-time error '-2147024809 (80070057)':
   Invalid procedure call or argument) occurs.  However, on next execution,
   or after a few more permutations, this error disappears.
4: Size is rather large.

I am sure that there are others, but in short, this is more of a theoretical
engine, which does work within its scope.  However, it would need to be greatly
refine before being introduced into a real situation.

Conclusion:
As stated before, this engine was never intended for use in a virus, but more
for educational purposes, such as giving other engine writers out there ideas.
I am not sure whether I should continue work on this engine, but if someone else
does, please send the modification to me.

Greets:
Buddy Music, Darkman, Duke, Perikles, Tally, Z0mbie
And all writers and collectors out there with new ideas and a friendly dispositon.
                                 ---EOF---