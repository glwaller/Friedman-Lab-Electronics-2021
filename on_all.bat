(
	timeout /t 1 > nul
	echo #F1L%2%
	timeout /t 3 > nul
	echo #F2L%2%
	timeout /t 3 > nul
	echo #F3L%2%
	timeout /t 3 > nul
	echo #F4L%2%
	exit
) | plink -batch -load "%1"
EXIT /B