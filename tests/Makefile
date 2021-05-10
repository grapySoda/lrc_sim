all:
	sudo hdparm -F /dev/sdc
	sudo hdparm -a0 -A0 -W0 /dev/sdc
	sudo fio write.fio
	gnuplot drive-type.gp
	# cd .. && ./run.sh
	evince drive-type.eps

clean:
	rm -rf *.eps *.log