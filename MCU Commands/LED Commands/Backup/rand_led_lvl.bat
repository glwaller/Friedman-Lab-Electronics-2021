(
	timeout /t 1 > nul
	echo l4_rand
	exit
) | plink -batch -load "FR2355"
EXIT /B