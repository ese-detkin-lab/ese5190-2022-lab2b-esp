### TODO:

- modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is pressed
- with a partner, connect a second Qt Py to the Stemma Qt bus of an ADPS9960 that's attached to a first Qt Py running the Lab 1 Python code
- record a trace of this exchange


-----------------------------------------------------------------------------------------------

Parameters we tried adjusted:
```
const uint CAPTURE_N_SAMPLES 
the *div* in logic_analyser_init(PIO pio, uint sm, uint pin_base, uint pin_count, float div) 
```

Time stamps we created:
![image](https://user-images.githubusercontent.com/84453030/202830353-1eeac9cc-7129-485f-a1ac-b98af94f9a07.png)
We count the gap between each changing edge. If the value is identical to previous one, it's represented by '*', and the times of '*' are recorded. Knowing every edge(from 1 to 0 or 0 to 1), and the time between each edge, we can replay the signal successfully.


![](https://github.com/Thea-E/ese5190-2022-lab2b-esp/blob/main/lab/06_pioscope/p6.gif)


