# Lab Feedback Questions
### Which parts of the lab specification have you found most confusing or difficult to understand? Be specific, and quote any parts of the lab description relevant to your answer (e.g., "modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is pressed.") I will make particular notice of anything that seems underspecified (e.g., words whose meaning is not clear from context, or conflicting interpretations of a deliverable).

The terms in the lab description is vague to me. I would hope to see more detailed definition. 
####  modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange
Firstly, what's the definition for "timestamped"? Everybody knows the literal meaning of timestamped, but what should it be look like in the outcome of the lab? 
It can be in many forms ![IMG_C084660CB55E-1](https://user-images.githubusercontent.com/84453030/202598439-eaa7dae1-8696-42e8-a85a-cab10f8205a9.jpeg)
And what's the unit of each timestamp? Is it in the machines absolute time(ns), the PIO cycle(1,2,3,4...), or other cycles? I think if timing is very immportant in embedded design, then different unit and measurement of time should be discussed on class, with slides and explanation. 

Secondly, the instruction can be more detail. We didn't know the goal is to modify the data sampling rule before push them to DMA until Wednesday midnight.
<img width="780" alt="image" src="https://user-images.githubusercontent.com/84453030/202599582-a1660402-eea2-4ddb-b4aa-d7d52ddfa4b9.png">

I spent lots of time "guessing" in which step I should adjust the sampling rate and capture the change. Now my understanding is pin --> ISR --> RX FIFO --> through DMA to PC. I still don't know if it's correct. The extra credit description posted at the due time is the clearest instruction I've seen so far. If it can be in the original lab I believe we won't waste time doing irrelevant data processing. So as in part 7.


### Which lab topics have you found most confusing or difficult to understand? E.g., "serial communication with Python," "aliased bitwise operations," "programming the PIO," etc. Be specific, and describe any lingering areas of confusion and/or anything that has helped you navigate them.

I would say PIO. I could understand that PIO can be used to program customized IO pin, but I felt confused what's the advantage of pio to write to registers, how to get pio module work in the c main program, how to program the .pio, what role does DMA plays in it, does DMA comes in the .pio or .c...
For example, 
#### modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register
I didn't know how to write to register with PIO. And I didn't know where to start to use PIO in my original sequencer, and I didn't know whether my orininal sequencer is working in the right track. I think it would be helpful if the lab can be cut into two pieces. For example, in last week when most of the students finished part 6, it would be a good time to go over the sequencer part together on class. Share what's a better way to do it, what's the problem of the directly r/w to register, why does timing matters. That would be a good start to show why we need the PIO and how PIO works better. 

Some instructions like "1. Go to xxxx.pio and try to understand line 20 to line 45 2. Create a sequencer.pio to include the pio r/w function 3. Go to your sequencer to change the direct r/w function to pio_r/w"  would help.

The only way that helps me navigate the problem is through Ed and talked to professor. I saw there are still many questions with no instructor's reply in Ed, which meant to be the best place to address questions. I am the head TA for another course, and we are required to make sure all the questions are answered within 24 hours. I show respect to all course members and appreciate the professors' patience to answering questions, but delay in getting help really slow down the process of our study. If the instruction were more detail, it would save time for both course staffs and students.


### Which parts of the lab have you found most difficult to implement? Again, be specific, citing any parts of the lab materials & your own code that are relevant to your answer.






### What steps have you taken to resolve these difficulties? Any other barriers you have faced to completing this assignment? Mention any students, repos, or other resources you have found helpful in completing the lab so far.
I watched the cornell's RP2040 course, they have more structural content, thorough explanation to memory, PIO, general protocols, and clear slides for each class.

