(
	timeout /t 1 > nul
	echo #F4L3
	exit
) | plink -batch -load "%1"
EXIT /B