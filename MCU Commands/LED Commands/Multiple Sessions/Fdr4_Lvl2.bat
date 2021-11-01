(
	timeout /t 1 > nul
	echo #F4L2
	exit
) | plink -batch -load "%1"
EXIT /B