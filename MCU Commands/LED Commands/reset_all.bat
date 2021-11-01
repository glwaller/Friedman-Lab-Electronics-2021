(
	timeout /t 1 > nul
	echo R
	exit
) | plink -batch -load "%1"
EXIT /B