all:
	cd src; make

clean: 
	rm lib/*.a;  cd testers; make clean ; cd .. ; cd src ; make clean
