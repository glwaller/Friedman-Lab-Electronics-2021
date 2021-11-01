(
	timeout /t 1 > nul
	echo #F2L3
	exit
) | plink -batch -load "FR2355"
EXIT /B