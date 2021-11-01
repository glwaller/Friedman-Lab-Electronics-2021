(
	timeout /t 1 > nul
	echo J
	exit
) | plink -batch -load "%1"
EXIT /B