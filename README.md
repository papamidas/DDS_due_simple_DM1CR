# DDS_due_simple_DM1CR
Implementation of a simple DDS sine generator on a Arduino Due

Mainly written for educational purposes, or just for playing around with DDS!

Building and connecting a reconstruction filter to the DAC output makes things even more interesting.
If you want to design your own reconstruction filter, pay attention to the load that you put on the DAC outputs.
Some people, including me, have fried a DAC output because the datasheet contains no specifications for the max. load.
Using a 1.5-kOhm-resistor as first resistor of a Sallen-Key-Filter (which was the outcome of the TI Filter Designer Tool) definitively killed the DAC0 output of my Due...
Later on I found this link:
https://microchipsupport.force.com/s/article/Drive-strength-of-DAC-pins-on-SAM3X-devices
Luckily the DAC1 output still worked, so I increased all R's by a factor of 10 and decreased all C's by the same factor and connected the filter to DAC1.
The filter still works with these values, and it works fine! In some sense it works even better than before.
A comparison of both variants can be found in the files "Filter_5kHz_AC.jpg" and "Filter_5kHz.jpg" together with the LTSpice simulation file "Filter_5kHz.asc" in the "Documentation"-Subfolder.

![Due-DDS](https://github.com/papamidas/DDS_due_simple_DM1CR/blob/master/Documentation/20200426_172746_600x800.jpg "Due-DDS+Filter+ADALM2000")

The user interface is primitive and works with the Arduino serial monitor (115200 bd):
Enter 'F', 'S', 'P', 'A' or 'O' and the Due will interpret the following number entries as new values for the output frequency ('F'), sample frequency ('S'), sample period ('P'), amplitude ('A') or offset value ('O').
Entering other letters leads to a continuous output of some interesting values of a DDS, including the phase register, every half of a second.

Try sampling frequencies of up to about 100000 samples per second, higher values will overload the processor.
If you have a classic scope, a USB scope and/or a spectrum analyzer, connect the DAC output and the filter output to your scope and watch both outputs simultaneously (or one after the other, if you do not have a 2-channel-device) at different sampling frequencies, different output frequencies and so on.
Test your knowledge of digital signal processing by explaining to yourself the phenomenas that you see.

Oversampling, undersampling, DDS resolution, aliasing, DAC resolution, SFDR, sinc response, ...(you name it); all these things can be explored here in practice by making some measurements and interpreting the results.

Have fun! 73 papamidas (DM1CR)


 