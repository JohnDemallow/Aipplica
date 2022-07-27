// SPIApp2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdio.h>
#include "windows.h"
#include "libMPSSE_spi.h"
#include "libAPSpecW.h"

int main()
{
	uint32 i, j, INT_TIME;
	uint8* pidbuf;
	uint8 idbfr[8];
	uint8 fourbytes[4];
	uint8 NumCalCoeff;
	pidbuf = &idbfr[0];
	uint8 retval;

	uint8 ptrnbfr[3000] = { 0x0 };
	uint8 RefS[3000] = { 0x0 };
	uint8 SigS[3000] = { 0x0 };
	uint8* pptrnbfr, * pRefS, * pSigS;
	pptrnbfr = &ptrnbfr[0];
	pRefS = &RefS[0];
	pSigS = &SigS[0];
	char Resp;

	AP_InitUSB();

	printf("Initalized USB Chip\n");

	for (i = 0;i < 10;i++) {
		AP_ReadID(pidbuf);
		if ((idbfr[1] == 0x1) && (idbfr[3] == 0x1) && (idbfr[5] == 0x01) && (idbfr[7] == 0x02)) break;
	}
	printf("Read ID  Successful %d\n", i);

	retval = AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n", retval);
	else {
		printf("USB Initialization failed, Please Retry\n");
		exit(1);
	}

	AP_ReadID(pidbuf);
	printf("%d %d %d %d\n", idbfr[1], idbfr[3], idbfr[5], idbfr[7]);
	printf("Read ID  Successful\n");

	AP_ReadCal(pptrnbfr);
	NumCalCoeff = ptrnbfr[1] / 4;
	printf("NumofCalCoeff: 0%d\n", NumCalCoeff);
	for (i = 0;i < NumCalCoeff;i++) {
		for (j = 1;j < 5;j++) { fourbytes[j - 1] = ptrnbfr[8 * i + 2 * j + 1]; }
		printf("C[%d] %f    \n", i, *(float*)fourbytes);
	}
	printf("Read Cal Successful\n");

	FILE* gnuplot = _popen("gnuplot", "w");
	if (!gnuplot) {
		perror("popen");
		exit(EXIT_FAILURE);
	}

	INT_TIME = 0x1000;
	printf("Setting Integration Time\n");
	AP_SetIntTime(INT_TIME);

	printf("Reading Dark Signal\n");

	retval = AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n", retval);

	AP_ReadArray(pRefS);

	printf("Turn ON the light source and Type a character\n");
	scanf_s("%c", &Resp,1);
	printf("Reading Spectral Signal \n");

	retval = AP_ReadPattern(pptrnbfr);
	if (retval <= 15) printf("Verification passed %d\n", retval);

	AP_ReadArray(pSigS);

	fprintf(gnuplot, "plot '-' u 1:2 t 'Spectrum' w lp\n");
	for (i = 0;i < 1500;i++) {
		fprintf(gnuplot, "%d %d\n", i, (pRefS[2 * i] - pSigS[2 * i]) * 256 + pRefS[2 * i + 1] - pSigS[2 * i + 1]);
	}
	printf("Turn OFF the light source and Type a character\n");
	scanf_s("%c", &Resp,1);

	fprintf(gnuplot, "e\n");
	fprintf(stdout, "Type Ctrl+d to quit...\n");
	fflush(gnuplot);
	getchar();
	_pclose(gnuplot);

	AP_Close();
	exit(EXIT_SUCCESS);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
