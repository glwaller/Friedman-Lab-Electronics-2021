(
	timeout /t 1 > nul
	echo R
	exit
) | plink -batch -load "FR2355_2"
EXIT /B