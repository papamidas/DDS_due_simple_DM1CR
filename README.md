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
A comparison of both variants can be found in one of the files.

![Due-DDS](https://github.com/papamidas/DDS_due_simple_DM1CR/blob/master/Documentation/20200426_172746_600x800.jpg "Due-DDS+Filter+ADALM2000")

