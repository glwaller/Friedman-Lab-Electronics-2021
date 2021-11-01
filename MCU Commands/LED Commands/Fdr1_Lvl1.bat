(
	timeout /t 1 > nul
	echo #F1L1
	exit
) | plink -batch -load "%1"
EXIT /B