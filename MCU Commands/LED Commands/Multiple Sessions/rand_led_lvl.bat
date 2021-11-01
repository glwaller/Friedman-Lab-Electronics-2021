(
	timeout /t 1 > nul
	echo l4_rand
	exit
) | plink -batch -load "%1"
EXIT /B