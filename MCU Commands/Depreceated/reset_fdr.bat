(
	timeout /t 1 > nul
	echo E
	exit
) | plink -batch -load "FR2355"
EXIT /B