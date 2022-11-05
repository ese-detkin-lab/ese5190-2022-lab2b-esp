### TODO:

Create a REPL to let you read and write RP2040 registers from a console. You should be able to:
- select any 32-bit address to read/write (even if not a valid RP2020 address)
- read/write any 32-bit value to this address
- read/write using any of the atomic bit-setting aliases and a 32-bit mask

## Lab 2b _ Worked with Amogh and saurabh.
# Startup Screen of REPL

![repl_start](https://user-images.githubusercontent.com/57740824/200087790-a45a0590-581a-48b9-9117-a0e24dd2b357.png)

# Read Command on REPL (Command = 'r')

https://user-images.githubusercontent.com/57740824/200087921-f50e2887-ee96-4f81-8485-cf1114597a81.mp4

WE are reading the SIO GPIO input register, to read the register we need to send the character 'r' (command), and enter the address. When I send the read command for the second time, I have pressed the BOOT button, hence the values changes

# Write Command on REPL (Command = 'w')

https://user-images.githubusercontent.com/57740824/200088649-685f9bd1-80ee-48b9-9a54-c750fc0b0d0f.mp4

First we write 1 to the register 0xd0000010, and then we read it to see if the value was written to it

# Record and Play Command on REPL (Record command = 'a', Play Command = 'p')

![image](https://user-images.githubusercontent.com/57740824/200088841-f63153ca-f027-4fbf-a922-27ab9155f7d2.png)

After sending the command 'a', we see the above screen for 25 secs, while blue led lights up on the Qtpy board indicating that we can now record the BOOT button presses

https://user-images.githubusercontent.com/57740824/200090872-c6400f8d-f7c9-48c2-863e-05dd007dc54c.mp4

We have recorded the BOOT button presses, now we can play them back by sending the play command 'p', the replay can be played at various speeds and with different colours(r/g/b)  
Speed Options: -4:4x slower, -2:2x Slower,0: Normal Speed, 2: 2x Faster, 4:4x Faster

Normal Speed

https://user-images.githubusercontent.com/57740824/200090988-3465e9a7-69a5-4031-ad83-fbdf7ecfc411.mp4

2x Slower

https://user-images.githubusercontent.com/57740824/200091246-bf7e666f-f003-4974-b769-196f74ba28cc.mp4

# Save and Upload on REPL (Save Command = 's', Upload Command = 'u')
When we send the save command, it generates a string, which can be stored and uploaded later, Now I will save a string and reset the board, and upload the same string and play it

Saving the Sequence

https://user-images.githubusercontent.com/57740824/200091502-48e5ee58-0c21-4ecd-a9b3-f3d08c26cd1b.mp4

Uploading the Sequence and playing it

https://user-images.githubusercontent.com/57740824/200091631-2826f62f-e0ba-4978-8db5-936770e7d9ce.mp4

# Conclusion
We have created the REPL code such that more functionality and commands can be easily added, to try out the REPL you can directly upload the ".uf2" to your Qtpy board and have fun
