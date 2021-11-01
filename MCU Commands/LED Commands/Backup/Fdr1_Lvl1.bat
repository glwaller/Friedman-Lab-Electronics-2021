(
	timeout /t 1 > nul
	echo #F1L1
	exit
) | plink -batch -load "FR2355"
EXIT /B