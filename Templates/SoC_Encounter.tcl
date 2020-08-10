 #===============================================================================
#  File        :  alu32top_par.tcl
#  Description :  Simple place+route script for ALU32 top-level model.
#  Notes       :  The script can be executed in Encounter console by running
#                     source path-to-tcl-script
#                 The script can also be run from a Linux shell as follows:
#                     encounter -init path-to-tcl-script -win
#  Author      :  Alain Vachoux, EPFL STI IEL LSM, alain.vachoux@epfl.ch
#                 for EDATP labs.
#  Tools       :  Encounter Digital Implementation 9.14
#===============================================================================
#  Utility procedure
#


# It is needed to set up before encounter: 
# setenv OA_UNSUPPORTED_PLAT "linux_rhel50_gcc44x"

# To init the analysis use
# encounter -init BIN/SoC_Encounter.tcl -cmd LOG/encounter.cmd -log LOG/encounter.log  -overwrite 
# encounter -cmd LOG/encounter.cmd -log LOG/encounter.log  -overwrite 


set NANGATE45_DIR /opt/designkits/nangate/FreePDK_45nm


#setMultiCpuUsage
#freeDesign
proc put_banner TITLE {
   puts ""
   puts "-----------------------------------------------"
   puts "       $TITLE"
   puts "-----------------------------------------------"
}
#  Design name
#

set ENTITY_NAME #M_ENT
set CLK_PERIOD #M_PER; # ns


set DESIGN_NAME "${ENTITY_NAME}_${CLK_PERIOD}ns"

#  Load configuration


# Fix sdc file
exec sed -i {s/\(([0-9]*)\)/abcd\0abcd/g} "../SNPS_DC/SDC/${DESIGN_NAME}_mapped.sdc"
exec sed -i {s/abcd(/[/g} "../SNPS_DC/SDC/${DESIGN_NAME}_mapped.sdc"
exec sed -i {s/)abcd/]/g} "../SNPS_DC/SDC/${DESIGN_NAME}_mapped.sdc"

#
put_banner "LOAD CONFIGURATION"
set init_lef_file "${NANGATE45_DIR}/Back_End/lef/NangateOpenCellLibrary.lef"
set init_verilog "HDL/GATE/${DESIGN_NAME}_mapped.v"
set init_top_cell "#M_ENT"

set init_pwr_net VDD 
set init_gnd_net VSS 

set init_mmmc_file "CONF/${init_top_cell}.view"

init_design 

setDrawView fplan
fit
saveDesign DB/${DESIGN_NAME}-import.enc

#  Specify the floorplan
#
put_banner "SPECIFY FLOORPLAN"
set ASPECT_RATIO   1.0     ;# rectangle with height = 1.0*width
set ROW_DENSITY    0.5    ;# 0.1..1.0
set CORE_TO_LEFT   10.0     ;# micron
set CORE_TO_BOTTOM 10.0     ;# micron
set CORE_TO_RIGHT  10.0     ;# micron
set CORE_TO_TOP    10.0     ;# micron
floorPlan \
   -r $ASPECT_RATIO \
   $ROW_DENSITY $CORE_TO_LEFT $CORE_TO_BOTTOM $CORE_TO_RIGHT $CORE_TO_TOP

saveDesign DB/${DESIGN_NAME}-fplan.enc


#  Connect global nets
#
put_banner "CONNECT GLOBAL NETS"
set PWR_NET VDD    ;# power net name as in LEF file
set GND_NET VSS    ;# ground net name as in LEF file
clearGlobalNets
globalNetConnect $PWR_NET -type pgpin -pin $PWR_NET -inst * -module {} -verbose
globalNetConnect $GND_NET -type pgpin -pin $GND_NET -inst * -module {} -verbose
#  Add power rings
#
put_banner "ADD POWER RINGS"
set CENTER_RING   1         ;# center rings between I/O and core
set WIDTH_BOT     2.0       ;# width of bottom ring segments
set WIDTH_RIGHT   2.0       ;# width of right ring segments
set WIDTH_TOP     2.0       ;# width of top ring segments
set WIDTH_LEFT    2.0       ;# width of left ring segments
set SPACING_BOT   1.5       ;# spacing of bottom ring segments
set SPACING_RIGHT 1.5      ;# spacing of right ring segments
set SPACING_TOP   1.5       ;# spacing of top ring segments
set SPACING_LEFT  1.5      ;# spacing of left ring segments
set LAYER_BOT     metal1       ;# LEF metal layer of bottom ring segments
set LAYER_RIGHT   metal2       ;# LEF metal layer of right ring segments
set LAYER_TOP     metal1       ;# LEF metal layer of top ring segments
set LAYER_LEFT    metal2       ;# LEF metal layer of left ring segments
set EXT_BL        ""         ;# 0 | 1 - extend bottom segment to left
set EXT_BR        br         ;# 0 | 1 - extend bottom segment to right
set EXT_RB        ""         ;# 0 | 1 - extend right segment to bottom
set EXT_RT        ""         ;# 0 | 1 - extend right segment to top
set EXT_TR        ""         ;# 0 | 1 - extend top segment to right
set EXT_TL        ""         ;# 0 | 1 - extend top segment to left
set EXT_LT        ""         ;# 0 | 1 - extend left segment to top
set EXT_LB        ""         ;# 0 | 1 - extend left segment to bottom

setAddRingMode -stacked_via_top_layer metal10

addRing  \
    -skip_via_on_wire_shape Noshape  \
    -skip_via_on_pin Standardcell  \
    -type core_rings  \
    -jog_distance 0.095  \
    -threshold 0.095  \
    -nets {VDD VSS}  \
    -follow core  \
    -layer "bottom $LAYER_BOT top $LAYER_TOP right $LAYER_RIGHT left $LAYER_LEFT" \
    -width "bottom $WIDTH_BOT top $WIDTH_TOP right $WIDTH_RIGHT left $WIDTH_LEFT"  \
    -spacing "bottom $SPACING_BOT top $SPACING_TOP right $SPACING_RIGHT left $SPACING_LEFT"  \
    -offset 0.5 \
    -extend_corner "$EXT_TL $EXT_TR $EXT_BL $EXT_BR $EXT_LT $EXT_LB $EXT_RT $EXT_RB"


   
#  Add power stripes
#
put_banner "ADD POWER STRIPES"
set STRP_LAYER      metal2       ;# LEF metal layer of stripe segments
set STRP_WIDTH      2.0       ;# width of stripe segments
set STRP_SPACING    1.5       ;# spacing of stripe segments
set STRP_NUM_SETS   1         ;# number of stripe sets
set STRP_XLEFT_OFFS 100.0     ;# start point of stripes from core boundary

addStripe \
   -nets " $GND_NET $PWR_NET " \
   -layer $STRP_LAYER \
   -width $STRP_WIDTH -spacing $STRP_SPACING \
   -number_of_sets $STRP_NUM_SETS \
   -xleft_offset $STRP_XLEFT_OFFS
#  Route power nets
#
put_banner "ROUTE POWER NETS"
set ALLOW_JOG  1    ;# 0 | 1 - allow jogs during routing to avoid DRC errors
sroute \
   -connect { blockPin corePin floatingStripe } \
   -blockPin { onBoundary bottomBoundary rightBoundary } \
   -allowJogging $ALLOW_JOG
saveDesign DB/${DESIGN_NAME}-power.enc


#  Place design
#
put_banner "PLACE STD CELLS"
set PROCESS 45    ;# process technology value [micron]
setDesignMode -process $PROCESS
setPlaceMode -timingDriven true
placeDesign -noPrePlaceOpt
setDrawView place
checkPlace
put_banner "PRE-CTS OPTIMIZATION"
#optDesign -preCTS -outDir RPT/${DESIGN_NAME}
saveDesign DB/${DESIGN_NAME}-placed.enc

#  Clock tree synthesis
#
put_banner "CREATE CLOCK TREE"
set CLOCK_NAME #M_CLOCK   ;# clock net name
setCTSMode -engine ck
createClockTreeSpec -output CTS/${DESIGN_NAME}_spec.cts \
   -bufferList BUF_X1 BUF_X2 BUF_X4 BUF_X8 BUF_X32 INV_X1 INV_X2 INV_X4 INV_X8 INV_X32

clockDesign -noDeleteClockTree -specFile CTS/${DESIGN_NAME}_spec.cts -outDir RPT/${DESIGN_NAME}


#ckECO -postCTS -useSpecFileCellsOnly -report RPT/${DESIGN_NAME}/cts.rpt
put_banner "POST-CTS OPTIMIZATION"
optDesign -postCTS -hold -outDir RPT/${DESIGN_NAME}
saveDesign DB/${DESIGN_NAME}-cts.enc
#  Route design
#
put_banner "ROUTE DESIGN"
set ROUTE_TIMING     true  ;# true | false - timing driven routing
set ROUTE_TDR_EFFORT 5     ;# 0..10 - 0: opt. congestion; 1: opt. timing
setNanoRouteMode \
   -routeWithTimingDriven $ROUTE_TIMING \
   -routeTdrEffort $ROUTE_TDR_EFFORT
routeDesign

put_banner "POST-ROUTE OPTIMIZATION"
#optDesign -postRoute -hold -si -outDir RPT/${DESIGN_NAME}
setDelayCalMode -engine default -siAware true
setDelayCalMode -engine aae -siAware false
optDesign -postRoute -hold -outDir RPT/${DESIGN_NAME}
saveDesign DB/${DESIGN_NAME}-routed.enc


#  Add filler cells
#
put_banner "ADD FILLER CELLS"
set FILLER_CELLS \
   "FILLCELL_X1 FILLCELL_X2 FILLCELL_X4 FILLCELL_X8 FILLCELL_X16 FILLCELL_X32"
set PREFIX  FILLER
addFiller \
   -cell " $FILLER_CELLS " \
   -prefix $PREFIX
setDrawView place
saveDesign DB/${DESIGN_NAME}-filled.enc
#  Verify design
#
put_banner "VERIFY DESIGN"
verifyConnectivity -type all -report RPT/${DESIGN_NAME}/connectivity.rpt
verifyGeometry -report RPT/${DESIGN_NAME}/geometry.rpt
#  Generate reports
#
put_banner "GENERATE REPORTS"
reportNetStat
reportGateCount -outfile RPT/${DESIGN_NAME}/gateCount.rpt
summaryReport -outdir RPT/${DESIGN_NAME}/summary
#  Generate SDF timing file
#
put_banner "GENERATE SDF TIMING FILE"
set SDF_VERSION 2.1    ;# 2.1 | 3.0
set SDF_PREC    4      ;# number of digits for delay values
write_sdf -version $SDF_VERSION -precision $SDF_PREC \
   TIM/${DESIGN_NAME}_pared_v${SDF_VERSION}_prec${SDF_PREC}.sdf
#  Generate Verilog netlist
#
put_banner "GENERATE VERILOG NETLIST"
saveNetlist -excludeLeafCell HDL/GATE/${DESIGN_NAME}_pared.v
saveNetlist -phys HDL/GATE/${DESIGN_NAME}_pared_physical.v
#  Generate GDS2 file
#
#put_banner "GENERATE GDS2 FILE"
#streamOut DEX/${DESIGN_NAME}_pared.gds \
#   -mapFile DEX/gds2out.map \
#   -libName ${DESIGN_NAME} \
#   -merge ../IP/RF16X32/SYAA90_16X32X1CM2.gds
put_banner "GENERATE SPEF FILE"
#setExtractRCMode -detail
setExtractRCMode -engine postRoute -effortLevel low
extractRC
#genTlfModel 
set file_genTlfModel "RPT/${init_top_cell}.lib"
do_extract_model -blackbox_2d -view AV_TC -force $file_genTlfModel 
rcOut -spef TIM/${DESIGN_NAME}.spef

# Exit
exit

