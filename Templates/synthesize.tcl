# To run dc_shell -f BIN/synthesize.tcl |& tee RPT/output.txt

set ENTITY_NAME #M_ENT
set ARCH_NAME #M_ARCH
set CLK_NAME #M_CLOCK
set CLK_PERIOD #M_PER;# ns


set DESIGN "${ENTITY_NAME}_${CLK_PERIOD}ns"

sh rm -rf DLIB/WORK/*
remove_design -all

define_design_lib work -path DLIB/WORK

#  analyze VHDL sources


#
puts "-i- Analyze VHDL sources"
#analyze -library work -format vhdl {HDL/RTL/multmod.vhd}

#M_SRCS





#  elaborate design
#
puts "-i- Elaborate design"

##  
#elaborate ${ENTITY_NAME} -architecture ${ARCH_NAME} -library work
elaborate ${ENTITY_NAME} -library work -update



#  save elaborated design
#
puts "-i- Save elaborated design"
write -hierarchy -format ddc -output DB/${DESIGN}_elab.ddc


#  define constraints
#
puts "-i- Define constraints"
puts "-i-   set_max_area 0"
puts "-i-   create_clock $CLK_NAME -period $CLK_PERIOD"
puts "-i-   set_fix_multiple_port_nets -all"
set_max_area 0
set_flatten true 
ungroup -flatten -all
create_clock $CLK_NAME -period $CLK_PERIOD
set_fix_multiple_port_nets -all


set_input_delay -clock $CLK_NAME 0 [all_inputs]
set_output_delay -clock $CLK_NAME 0 [all_outputs]


#  map and optimize design
#
puts "-i- Map and optimize design"
compile -map_effort high -area_effort high

# check designs
#
check_design > RPT/check_link.txt
link >> RPT/check_design.txt

check_design 
link 
report_area 
report_timing 

#  save mapped design
#
puts "-i- Save mapped design"
write -hierarchy -format ddc -output DB/${DESIGN}_mapped.ddc
#  generate reports
#

puts "-i- Generate reports"
report_constraint -nosplit -all_violators > RPT/${DESIGN}_allviol.rpt
report_area > RPT/${DESIGN}_area.rpt
report_timing > RPT/${DESIGN}_timing.rpt
report_resources -nosplit -hierarchy > RPT/${DESIGN}_resources.rpt
report_reference -nosplit -hierarchy > RPT/${DESIGN}_references.rpt



#  generate Verilog netlist
#

#  generate Verilog netlist
#
puts "-i- Generate Verilog netlist"
change_names -rules verilog -hierarchy
write -format verilog -hierarchy -output HDL/GATE/${DESIGN}_mapped.v

#  generate SDF timing file for Verilog
#
puts "-i- Generate SDF file for Verilog netlist"
write_sdf -version 2.1 TIM/${DESIGN}_mapped_vlog.sdf
#  generate design constraint file
#
puts "-i- Generate SDC design constraint file"
write_sdc -nosplit SDC/${DESIGN}_mapped.sdc

puts "-i- Generate VHDL netlist"
change_names -rules vhdl -hierarchy
write -format vhdl -hierarchy -output HDL/GATE/${DESIGN}_mapped.vhd


exit



