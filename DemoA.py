#!/usr/bin/env python3
from APSpec import Spectrometer
from matplotlib import pyplot 
import time

# Instantiate an Instance of the Spectrometer
Spec = Spectrometer()

# Set integration time to 20msec.
Spec.Setinttime(0.020)

# Initialize a list to store "Reference" Spectral Data 
Ref_Data=[]

# Capture Reference Spectra or "Dark Signal" from the Spectrometer
# Switch off the light signal or cover the exposed fiber connector with a dustcap.
# If your device has a slit, cover the input so as to block any ambient light. 
# Do not touch the slit opening with you hands. 
Ref_Data=Spec.GetSpectra()

# Examine the first few element of captured spectral data. If the light path is blocked properly, expect values in 800 +/- 50 range.
print(Ref_Data[0:15])
print("Turn on the light source")

#plot the reference data or the Dark Signal for a review
pyplot.plot(Ref_Data)
pyplot.show()

# Initialize a list to capture spectral data (with the Optical light source ON)
# Turn the light source ON and capture the data from Spectrometer
Spectral_Data=[]

# Reset Integration time, if needed
Spec.Setinttime(0.020)

# MAKE SURE THE LIGHT SOURCE is ON
Spectral_Data=Spec.GetSpectra()

# Substract the Reference reading from the captured Spectra
Difference = [(Ref_Data[i]-Spectral_Data[i]) for i in range(len(Ref_Data))]

#plot the DIfference
pyplot.plot(Difference)
pyplot.show()

quit()
