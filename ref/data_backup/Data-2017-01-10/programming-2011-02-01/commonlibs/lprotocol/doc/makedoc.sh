#!/bin/sh 

rm -rf html
rm -rf latex
doxygen doxyfile.cfg

#cd latex
#tex main.tex
#dvips main.dvi
#ps2pdf main.ps
#cd ..
