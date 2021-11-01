(
	timeout /t 1 > nul
	echo #F1L3
	exit
) | plink -batch -load "%1"
EXIT /B