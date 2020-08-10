
source /opt/designconfig/startup/cshrc 

# Work workspace
rm -rf DLIB/work
vlib DLIB/work/
vmap work DLIB/work/

#HEADER_END

#CMD *.vhd vcom 
#CMD *.v vlog 
#CMD SIM vsim 
#DATA FLAGS -c -sdfnoerror -t 100fs   
#DATA LIBS FreePDK45
#DATA SDFMIN uut=../CDS_SOCE/TIM/
#DATA SDFMAX uut=../CDS_SOCE/TIM/

#CMD SIM_CS #SIM #LIB #FLAGS #SDFMAX #SDFMIN #TB



