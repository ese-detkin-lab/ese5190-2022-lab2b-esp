### TODO:

- modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register 

---------------------------------

Based on part 6, coordinate with our python script, we have a mode that begins a capture when the BOOT button is pressed and ends the capture when the BOOT button is released, sending the capture data over serial and logging it to a file.

The captured data only consist of the changing edge(from 0 to 1 or 1 to 0), with their timestamps. They are passed to python console and written to .csv file as before.

<img width="114" alt="image" src="https://user-images.githubusercontent.com/84453030/202830604-f2217903-6fcf-40a9-b4d4-81f32fe0f796.png">

After recorded the values, they will be replayed on LED.
![](https://github.com/Thea-E/ese5190-2022-lab2b-esp/blob/main/lab/07_pio_sequencer/p7.gif)


