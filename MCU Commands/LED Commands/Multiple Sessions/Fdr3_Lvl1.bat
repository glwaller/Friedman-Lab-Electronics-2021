(
	timeout /t 1 > nul
	echo #F3L1
	exit
) | start /B plink -batch -load "%1"

(
	timeout /t 1 > nul
	echo #F3L1
	exit
) | start /B plink -batch -load "%2"

(
	timeout /t 1 > nul
	echo #F3L1
	exit
) | start /B plink -batch -load "%3"

(
	timeout /t 1 > nul
	echo #F3L1
	exit
) | start /B plink -batch -load "%4"

EXIT /B