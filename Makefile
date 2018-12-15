all:
	antlr -gt lego.g
	dlg -ci parser.dlg scan.c
	g++ -o lego lego.c scan.c err.c -I/home/soft/PCCTS_v1.33/include -Wno-write-strings
run:
	./lego < e
	
manual:
	./lego
	
clean:
	rm -r lego