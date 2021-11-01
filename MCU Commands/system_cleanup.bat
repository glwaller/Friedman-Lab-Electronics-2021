(
:: loop start
	timeout /t 1 > nul
	echo F
	timeout /t 3 > nul
	echo F
	timeout /t 3 > nul
	echo F
	
	timeout /t 3 > nul
	echo G
	timeout /t 3 > nul
	echo G
	timeout /t 3 > nul
	echo G
	
	timeout /t 3 > nul
	echo H
	timeout /t 3 > nul
	echo H
	timeout /t 3 > nul
	echo H
	
	timeout /t 3 > nul
	echo J
	timeout /t 3 > nul
	echo J
	timeout /t 3 > nul
	echo J
	
) | plink -batch -load "%1"
EXIT /B