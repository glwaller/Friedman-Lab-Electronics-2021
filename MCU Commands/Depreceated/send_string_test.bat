(
	timeout /t 1 > nul
	echo #asdfg
	exit
) | plink -batch -load "FR2355"
EXIT /B