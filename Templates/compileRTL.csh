
source /opt/designconfig/startup/cshrc 

# Work workspace
rm -rf DLIB/work
vlib DLIB/work/
vmap work DLIB/work/

#HEADER_END

#CMD *.vhd vcom 
#CMD *.v vlog 
#CMD SIM vsim 
#DATA FLAGS -c
#CMD SIM_CR #SIM #FLAGS #TB





