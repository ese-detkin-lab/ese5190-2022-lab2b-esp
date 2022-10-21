## Lab2B ese 5190
  Harish Ramesh, done with Sugata Sen

## 1.The working LED GIF

We made the LED blink and stay on when any number > 5 is entered, and turn off and stay off when any number <5 is entered. Code can be seen in the same repo.

![](https://github.com/harishramesh98/ese5190-2022-lab2b-esp/blob/7f2551856d2505a3d17dff841f0640aad521cb72/ledREPLControl.gif)

## 2. Project Proposal

We plan to make a LED Morse code converter, and output visualizer.

For this, we will take character inputs via REPL, and make an LED array display the corresponding Dots and Dashes as output, then wait for the next character input from user.

This would be a logical extension to the gpio blinking task, and make use of multiple GPIO Pins, while based on a simple premise.

	Extended goal if time permits:  We can make a receiver using LDR, and use ADC to record the resulting waveform on the pc.

## 3. Components list

  Multiple LEDs( approx. 5 )
  Resistors (360 Ohm x5, 250 Ohm x5)
  Proto-board/Breadboard
  Jumper wires(At least 10)
	LDR X2
	BC 547 npn transistorx1
