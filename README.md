[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/-AGB4N5E)
This is the template repo for homework 1. Please start from here.


Part 1
-------
In this part, I created a custom shell while still being allowed to use library functions with systemcalls. The process consists of a step where the stdin line is read and tokenized, a step to check if the user performed a valid exit, a step to check if the user performed a valid cd, and a fork/execution step to run the required command. The shell also manually handles interrupt calls like CTRL-C, ignoring them for the parent and stopping the execution of the command for the child. 

For this assignment, I assumed the shell should just ignore CTRL-C. Thus, pressing those two buttons shouldn't do anything, and the user will stay on the command line. Additionally, my shell takes both /bin/(command) and (command) into account, so the user does not have to worry about that. Given these assumptions, the shell works as expected.

Part 2
-------
For this part, the same shell was made, but no library functions were allowed for the system calls. Everything here works the same as part 1, but I replaced the library functions with the correct system calls given it was seen in the trace.txt folder. Overall, this works exactly the same as part 1, and nothing more needs to really be said. 

Part 3
-------
For this part, I use the provided assembly code to print out "hello, world" to the center of the VGA. Additionally, I use the other provided assembly code to get the instruction pointer and stack pointers. Then, I print them out following the "hello world" in the format "hello, world (ip) (sp)". Everything that has a value has a white background, making it visible. Also, as mentioned above, they are all centered. 

Overall, this part of the assignment was relatively clear given the instructions, and I used that as a basis for how I approached the assignment. The main thing that confused me was the start point portion, as the instructions stated that the assembly code shifted the starting address (or the segment base). I didn't know how to access that specific point, but a TA and someone I met at office hours helped me. Also, I had an issue where my hello, world was printing, but my instruction and stack pointer values were not. After looking at it for some time with the TAs, we realized it was because I called the get-ip() function after I printed out hello, world, which prevented the other stuff for printing. Putting that at the top of the main function fixed that problem, though I'm not sure why this issue existed in the first place. Otherwise, the assignment was relatively straightforward.
