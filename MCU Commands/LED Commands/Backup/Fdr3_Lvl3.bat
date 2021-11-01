(
	timeout /t 1 > nul
	echo #F3L3
	exit
) | plink -batch -load "FR2355"
EXIT /B