(
	timeout /t 1 > nul
	echo k
	exit
) | plink -batch -load "FR2355"
EXIT /B