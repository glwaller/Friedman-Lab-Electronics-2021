set /a FDR=(%random% * 4/32768) +1
set /a LVL=(%random% * 4/32768) +0

(
	timeout /t 1 > nul
	echo #F%FDR%L%LVL%
	exit
) | plink -batch -load "%1"
EXIT /B