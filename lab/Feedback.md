# Lab Feedback Questions
### Which parts of the lab specification have you found most confusing or difficult to understand? Be specific, and quote any parts of the lab description relevant to your answer (e.g., "modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is pressed.") I will make particular notice of anything that seems underspecified (e.g., words whose meaning is not clear from context, or conflicting interpretations of a deliverable).

Some of the terms in the lab description is vague to me. I would hope to see more detailed definition. 
####  "modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange"
Firstly, what's the definition for "timestamped"? How should it be represented in the outcome of the lab? 
It can be in many forms ![IMG_C084660CB55E-1](https://user-images.githubusercontent.com/84453030/202598439-eaa7dae1-8696-42e8-a85a-cab10f8205a9.jpeg)
And more importantly, what's the unit of each timestamp? Should it be in the machines absolute time(ns), the PIO cycle(1,2,3,4...), or other cycles? I think timing is very immportant in embedded design, it would be helpful if different unit and measurement of time could be discussed more.

Secondly, the instruction can be more detail. 
<img width="780" alt="image" src="https://user-images.githubusercontent.com/84453030/202599582-a1660402-eea2-4ddb-b4aa-d7d52ddfa4b9.png">

I spent lots of time "guessing" in which step I should adjust the sampling rate and capture the change. Now my understanding is pin --> ISR --> RX FIFO --> through DMA to PC. I still don't know if it's correct. Until I saw the extra credit description above, I start to know the right way is to modify the data sampling rule *before push them to DMA*. This requirement is more clear than the original one. 


### Which lab topics have you found most confusing or difficult to understand? E.g., "serial communication with Python," "aliased bitwise operations," "programming the PIO," etc. Be specific, and describe any lingering areas of confusion and/or anything that has helped you navigate them.

I would say PIO. I could understand that PIO can be used to program customized IO pin, but I felt confused what's the advantage of pio to write to registers, how to get pio module work in the c main program, how to program the .pio, what role does DMA plays in it, does DMA comes in the .pio or .c...
For example, 
#### "modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register"
I was stucked in where to start to use PIO in my original sequencer, and I didn't know whether my orininal sequencer is working in the right track. I think it would be helpful if the lab can be cut into two pieces. For example, in last week when most of the students finished part 6, it would be a good time to go over the sequencer part together on class. Share what's a better way to do it, what's the problem of the directly r/w to register, why does timing matters. That would be a good start point to show why we need the PIO and how PIO works better. 

Some instructions like "1. Go to xxxx.pio and try to understand line 20 to line 45 2. Create a sequencer.pio to include the pio r/w function 3. Go to your sequencer to change the direct r/w function to pio_r/w"  would help.

Similar in part 8,
For example, 
#### "Use the capabilities of your sequencer to implement the ADPS9960 protocol and control the sensor."
<img width="628" alt="image" src="https://user-images.githubusercontent.com/84453030/202734660-6a779d28-6522-44ec-b22c-f8d326fc198a.png">
More definition and clarify are needed for "one-off PIO implementation". I am confused what is called one-off pio and what's the difference from using sequencer. As from part 7, the sequencer included the pio low level functions, and the i2c.pio also has pio in it. 

And I really appreciate the professors' patience to answering questions!


### Which parts of the lab have you found most difficult to implement? Again, be specific, citing any parts of the lab materials & your own code that are relevant to your answer.
I had difficulty in getting python script and c code working together in the sequencer part, because I have never use two different languages and platforms in one project before. However, after clarifying the duty for C and python, like cutting the sequencer to two parts, it became clear. So I hope the instruction could give us more direction, like
#### 1. Creat a python script, import the pyserial library, use it to connect and take output from serial console
#### 2. Get your C file run, instead of open minicom as before, run your python script and see the original minicom output in your python end.
#### 3. Start from there, try use python to generate .csv file, and write into the board with your python script, etc.

Another task that hold me back is the PIO related part mentioned before. I think I need deeper understanding for the RP2040 memory structure, how pio, dma work together and the data's flowing path, to better do the lab. I have read the datasheet, but the description there is quite vague and general, and PIO is such unique that there's no much information I could find when googled it. I would be really grateful if more resources can be shared.


### What steps have you taken to resolve these difficulties? Any other barriers you have faced to completing this assignment? Mention any students, repos, or other resources you have found helpful in completing the lab so far.
I watched the cornell's RP2040 course，it was helpful.
