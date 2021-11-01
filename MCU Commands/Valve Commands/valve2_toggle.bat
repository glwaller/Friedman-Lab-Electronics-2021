(
	timeout /t 1 > nul
	echo G
	exit
) | plink -batch -load "%1"
EXIT /B