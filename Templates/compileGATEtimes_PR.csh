#!/bin/csh 

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

#vlog HDL/GATE/xorSbox_1ns_pared.v 
#vcom HDL/TBENCH/xorSboxGATE_tb.vhd

#FOOTER_START

## --------------------------------

set timemax=#TIMEMAX # 
set timemin=#TIMEMIN # 
set timestep=#TIMESTEP
set timeunitsSim="ps"

# For the current data after analysis in Matlab period=250:10:2200 ps

set pathSTI="OUT/" 
set pathOut="OUT"
set stimulifilename="stimuli.txt"

echo "****************************************" > Progress.txt


set period = $timemax
while ( $period >= $timemin )
   echo "****************************************"
   echo "Max: $timemax    Current: $period       Min:$timemin"
   
   echo "****************************************" >> Progress.txt
   echo "Max: $timemax    Current: $period       Min:$timemin" >>Progress.txt
   
   
   echo "#SIM #LIB #SDFMAX #SDFMIN #FLAGS #TB -gclkPeriod=$period$timeunitsSim -gstimulifilename=$pathSTI$stimulifilename -gsimreptfilename=${pathOut}/file$period$timeunitsSim.rpt"> BIN/GATETimes.do


   echo "run -all" >>  BIN/GATETimes.do
   echo "exit" >>  BIN/GATETimes.do
    
   #SIM -c -do BIN/GATETimes.do > /dev/null
#   #SIM -c -do BIN/GATETimes.do
    
   
   @ period = $period - $timestep
end


echo "Finished"
echo "Finished" >> Progress.txt

exit




