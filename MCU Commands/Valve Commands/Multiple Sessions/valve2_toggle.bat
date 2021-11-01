(
	timeout /t 1 > nul
	echo G
	exit
) | start /B plink -batch -load "FR2355"

(
	timeout /t 1 > nul
	echo G
	exit
) | start /B plink -batch -load "FR2355_2"

(
	timeout /t 1 > nul
	echo G
	exit
) | start /B plink -batch -load "FR2355_3"

(
	timeout /t 1 > nul
	echo G
	exit
) | start /B plink -batch -load "FR2355_4"
EXIT /B