(
	timeout /t 1 > nul
	echo #%2%
	timeout /t 4 > nul
	echo %3%
	exit
) | plink -batch -load "%1%"
EXIT /B
