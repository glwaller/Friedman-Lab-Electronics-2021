(
	timeout /t 1 > nul
	echo H
	exit
) | plink -batch -load "%1"
EXIT /B