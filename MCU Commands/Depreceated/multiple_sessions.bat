(
	timeout /t 1 > nul
	echo k
	exit
) | start /B plink -batch -load "%1"

(
	timeout /t 1 > nul
	echo k
	exit
) | start /B plink -batch -load "%2"

EXIT /B