(
	echo r
	timeout /t 2 > nul
	echo g
	timeout /t 2 > nul
	echo A
	timeout /t 2 > nul
	echo R
) | plink -batch -load "MSP430"