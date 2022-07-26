// Copyright © 2000-2022 Aipplica Systems Pte Ltd
//  
//  
//  THIS SOFTWARE IS PROVIDED BY AIPPLICA SYSTEMS PTE LIMITED ``AS IS'' AND ANY EXPRESS 
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AIPPLICA SYSTEMS PTE INTERNATIONAL LIMITED
//  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.
// 
//  Pre-release Alpha Version. 0.1 dtd 25 Jul 2022
//
//  This a sample program to capture and display a Spectrum from Aipplica Spectrometer plugged into USB port
//  It captures dark signal (with light source Off) and subtracts it from Spectral Signal (light source On)
//  and displays the result using gnuplot
//

#include <stdio.h>
#include <stdlib.h>
#include "/usr/local/include/ftd2xx.h"
#include "./libMPSSE_spi.h"
#include "./libAPSpec.h"
int main() 
{
	uint32 i,j,INT_TIME;
	uint8 *pidbuf;
	uint8 idbfr[8];
	uint8 fourbytes[4];
	uint8 NumCalCoeff;
	pidbuf = &idbfr[0];
	uint8 retval;

	uint8 ptrnbfr[3000],RefS[3000],SigS[3000];
	uint8 *pptrnbfr,*pRefS,*pSigS;
	       pptrnbfr = &ptrnbfr[0];
	       pRefS = &RefS[0];
	       pSigS = &SigS[0];
	char Resp;

  	AP_InitUSB();

	printf("Initalized USB Chip\n");

	for(i=0;i<10;i++) {
		AP_ReadID(pidbuf);
		if((idbfr[1] == 0x1) & (idbfr[3] == 0x1) & (idbfr[5] ==0x01) & (idbfr[7] == 0x02)) break;
	}
	printf("Read ID  Successful %d\n",i); 

	retval=AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n",retval);
	 else { printf("USB Initialization failed, Please Retry\n");
	 		exit(1);
	 }

	AP_ReadID(pidbuf);
	printf("%d %d %d %d\n",idbfr[1],idbfr[3],idbfr[5],idbfr[7]);
	printf("Read ID  Successful\n");

	AP_ReadCal(pptrnbfr);
        NumCalCoeff = ptrnbfr[1]/4; 
        printf("NumofCalCoeff: 0%d\n", NumCalCoeff); 
	for(i=0;i<NumCalCoeff;i++) {
               	for(j=1;j<5;j++) { fourbytes[j-1] = ptrnbfr[8*i+2*j+1]; }
               	printf("C[%d] %f    \n",i,*(float *)fourbytes);
	}
	printf("Read Cal Successful\n");

    	FILE *gnuplot = popen("gnuplot", "w");
    	if (!gnuplot) {
        	perror("popen");
        	exit(EXIT_FAILURE);
    	}

	INT_TIME=0x1000;
	printf("Setting Integration Time\n");
	AP_SetIntTime(INT_TIME);

	printf("Reading Dark Signal\n");

	retval=AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n",retval);

       	AP_ReadArray(pRefS);

	printf("Turn ON the light source and Type a character\n");
	scanf("%c",&Resp);
	printf("Reading Spectral Signal \n");

	retval=AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n",retval);

       	AP_ReadArray(pSigS);

    	fprintf(gnuplot, "plot '-' u 1:2 t 'Spectrum' w lp\n");
	for(i=0;i<1500;i++) { 
		fprintf(gnuplot,"%d %d\n",i,(pRefS[2*i]-pSigS[2*i])*256 + pRefS[2*i+1]-pSigS[2*i+1]); 
	}
	printf("Turn OFF the light source and Type a character\n");
	scanf("%c",&Resp);

    	fprintf(gnuplot, "e\n");
    	fprintf(stdout, "Type Ctrl+d to quit...\n");
    	fflush(gnuplot);
    	getchar();
    	pclose(gnuplot);

	AP_Close();
    	exit(EXIT_SUCCESS);

}
