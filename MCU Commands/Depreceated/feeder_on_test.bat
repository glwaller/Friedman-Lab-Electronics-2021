(
	timeout /t 1 > nul
	echo g
	exit
) | plink -batch -load "FR2355"
EXIT /B