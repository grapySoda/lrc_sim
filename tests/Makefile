all:
	# sudo hdparm -F /dev/sdc
	sudo hdparm -a0 -A0 -W0 /dev/sdc
	# sudo hdparm -a1 -A1 -W1 /dev/sdc
	sudo fio write.fio
	cd .. && ./run.sh
	gnuplot drive-type.gp
	evince drive-type.eps

clean:
	rm -rf *.eps *.log