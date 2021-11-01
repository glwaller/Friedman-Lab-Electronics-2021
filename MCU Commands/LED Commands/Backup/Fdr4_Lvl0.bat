(
	timeout /t 1 > nul
	echo #F4L0
	exit
) | plink -batch -load "FR2355"
EXIT /B