(
	timeout /t 1 > nul
	echo #F1L0
	exit
) | plink -batch -load "FR2355"
EXIT /B