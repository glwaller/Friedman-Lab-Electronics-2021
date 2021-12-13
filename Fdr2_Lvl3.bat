(
	timeout /t 1 > nul
	echo #F2L3
	exit
) | plink -batch -load "%1"
EXIT /B