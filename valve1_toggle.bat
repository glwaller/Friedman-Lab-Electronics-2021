(
	timeout /t 1 > nul
	echo F
	exit
) | plink -batch -load "%1"
EXIT /B