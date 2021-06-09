DEVICE = /dev/md6

all:
	# sudo hdparm -F $(DEVICE)
	# sudo hdparm -a0 -A0 -W0 $(DEVICE)
	# sudo hdparm -a1 -A1 -W1 $(DEVICE)
	./mktrace.py $(DEVICE)
	sudo fio write.fio
	# cd .. && ./translate
	# gnuplot drive-type.gp
	# evince drive-type.eps
	cp rand-write_lat.1.log rand-write_lat.csv
	sudo chown ccs rand-write_lat.1.log
	cd .. && ./run.sh
	# ./plot.py

clean:
	rm -rf *.eps *.log