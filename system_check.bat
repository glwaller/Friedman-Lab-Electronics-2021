echo -------------------------------------- >> "Log Files\%1_output.log" 2>&1
echo System Check >> "Log Files\%1_output.log" 2>&1
echo %time% >> "Log Files\%1_output.log" 2>&1
echo %date% >> "Log Files\%1_output.log" 2>&1
(
	echo R
	
	timeout /t 3 > nul
	echo #F1L3
	timeout /t 3 > nul
	echo F
	timeout /t 3 > nul
	echo F
	timeout /t 3 > nul
	echo #F1L0
	
	timeout /t 3 > nul
	echo #F2L3
	timeout /t 3 > nul
	echo G
	timeout /t 3 > nul
	echo G
	timeout /t 3 > nul
	echo #F2L0
	
	timeout /t 3 > nul
	echo #F3L3
	timeout /t 3 > nul
	echo H
	timeout /t 3 > nul
	echo H
	timeout /t 3 > nul
	echo #F3L0
	
	timeout /t 3 > nul
	echo #F4L3
	timeout /t 3 > nul
	echo J
	timeout /t 3 > nul
	echo J
	timeout /t 3 > nul
	echo #F4L0
	
) | plink -batch -load "%1" >> "Log Files\%1_output.log" 2>&1
EXIT /B