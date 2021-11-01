(
	timeout /t 1 > nul
	echo #F1L2
	exit
) | plink -batch -load "FR2355"
EXIT /B