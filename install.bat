@echo off

make -f Makefile.win clean
make -f Makefile.win uninstall
make -f Makefile.win
make -f Makefile.win install

@echo on

pause

