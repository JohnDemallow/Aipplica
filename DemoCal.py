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

#Let find index of few tallest peaks in the Spectra
#This is done manually by examining peak values and corressponding pixels.
for i in [Difference.index(i) for i in sorted(Difference, reverse=True)][0:20]:
    print(i,Difference[i],";",end='')

# Looking at the captured Spectra and Specifications of HG2 Calibration source, We can make a list of 
# wavelength peaks corressponding pixels.

wavlens=[435.8,546,696.5,763.5,811.5]
pixels=[93,484,1027,1271,1443]

# Now Use numpy's polyfit function and fit a 2nd order polynomial to these points, with X axis as pixels and Y axis
# as Wavelengths. You can choose higher a order polynomial and more points from the spectra to calibrate.

Z=np.polyfit(pixels,wavlens,2)

print(Z)
# These are calibration coefficients of our second order polynomial Y = Z[2] + Z[1]*X + Z[0]*X^2
# Store these into the Spectrometer memory with a WriteCal.
Spec.WriteCal(-0.000002759,0.282167,409.7367)
time.sleep(0.1)
# Reread stored calibration Coefficients.
C=Spec.ReadCal()
print(C)

# Compose a Calibration polynomial with Calibration coefficients determined in the previous step. 
# Convert X axis from pixels to wavelengths and plot the captured spectra as wavelength versus amplitude.
Calpoly=np.poly1d(C)
Xval=np.arange(0,1500,1)
Xcal=Calpoly(Xval)
pyplot.plot(Xcal,Difference); pyplot.show()

quit()
