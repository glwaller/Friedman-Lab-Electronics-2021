(
	timeout /t 1 > nul
	echo k
	exit
) | plink -batch -load "%1"
EXIT /B