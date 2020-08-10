-------------------------------------------------------------------------------
-- Title      : xorSbox testbench
-- Project    : FSA in RLWE
-------------------------------------------------------------------------------
-- File       : xorSbox_tb.vhd
-- Author     : Felipe Valencia  <valena@usi.ch>
-- Company    : Alari
-- Created    : 2019-09-20
-- Last update: 2019-09-20
-- Platform   : ModelSim (simulation), Synopsys (synthesis)
-- Standard   : VHDL'2008
-------------------------------------------------------------------------------
-- Description: This module test the module xorSbox
-------------------------------------------------------------------------------
-- Copyright (c) 2019 Alari, USI Lugano
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2019-09-20  1.0      afvp	Created
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
use std.textio.all;
use ieee.std_logic_textio.all;


entity xorSbox_tb is
  generic(clkPeriod: time :=10 ns;
          stimulifilename:string := "../MATLAB/VAR/STI/stimuliTest.txt";
          simreptfilename : string := "../MATLAB/stimuliTestRTLout.txt";
          WIDTH : integer := 8
          ); 
end xorSbox_tb;


architecture xorSbox_behav of xorSbox_tb is   
    

    component xorSbox is
      port( ClkxCI    : in  std_logic;
            Operand1xDI     : in  std_logic_vector(WIDTH-1 downto 0);
      		Operand2xDI     : in  std_logic_vector(WIDTH-1 downto 0);
            OutxDO    : out std_logic_vector(WIDTH-1 downto 0));

    end component;
       

  signal ClkxC : std_logic; -- Driving clock
  signal controlxS: std_logic:='0';
  signal Outxd: std_logic; -- Out of the module
  
  signal In1StimulixD: std_logic_vector(WIDTH-1 downto 0); -- Input from stimulus file
  signal In2StimulixD: std_logic_vector(WIDTH-1 downto 0); -- Input from stimulus file
  signal OutStimulixD: std_logic_vector(WIDTH-1 downto 0); -- Output from stimulus file
  signal OutStimuliSyncxD: std_logic_vector(WIDTH-1 downto 0); -- Output from stimulus file after synchronization with the circuit output
  signal OutcircuitxD: std_logic_vector(WIDTH-1 downto 0); -- Out of the module

  -- declaration of stimuli, expected responses, and simulation report files

  file stimulifile: text open read_mode is stimulifilename;
  file simreptfile: text open write_mode is simreptfilename;
  

begin

  
  -- Instantiate the Unit Under Test (UUT)
   uut: xorSbox 
   port map( ClkxCI   => ClkxC,
            Operand1xDI  => In1StimulixD,
      		Operand2xDI  => In2StimulixD,
            OutxDO    => OutcircuitxD);

  Tb_clkgen : process
  begin
    if(controlxS='1') then
      wait;
    else
      ClkxC <= '0'; 
      wait for clkPeriod/2;
      ClkxC <= '1';
      wait for clkPeriod/2;  
    end if; 
  end process Tb_clkgen;
  
  outputSync:process (ClkxC)   begin
    if(ClkxC'event and ClkxC='1') then
      OutStimuliSyncxD<=OutStimulixD;
    end if;
  end process;
  
  Tb_stimuliIn : process
    variable INLine   : line;
    variable tempxD  : integer;
  begin
    while not endfile(stimulifile) loop
      wait until ClkxC = '1';
      readline(stimulifile, INLine);
      read(INLine, tempxD);
      In1StimulixD <= std_logic_vector(to_unsigned(tempxD, WIDTH));
      read(INLine, tempxD);
      In2StimulixD <= std_logic_vector(to_unsigned(tempxD, WIDTH));
      read(INLine, tempxD);
      OutStimulixD <= std_logic_vector(to_unsigned(tempxD, WIDTH));
      wait for clkPeriod;
    end loop;
    wait for clkPeriod;
    controlxS<='1';
    wait;
  end process;
  
  Tb_test: process
  variable OUTLine   : line;
  variable errors : integer := 0;
  begin

    errors := 0;
    wait for 2*clkPeriod;
    while (controlxS /='1') loop
      write(OUTLine,OutStimuliSyncxD);
      write(OUTLine,string'(" "));
      write(OUTLine,OutcircuitxD);
      writeline(simreptfile,OUTLine);
      
      if(OutStimuliSyncxD /= OutcircuitxD) then
        write(OUTLine,string'("Testbench failed"));
        writeline(OUTPUT,OUTLine);
        write(OUTLine,string'("OutCircuit "));
        write(OUTLine, OutcircuitxD);
        write(OUTLine,string'("  OutStimuliSyncxD "));
        write(OUTLine, OutStimuliSyncxD);
        writeline(OUTPUT,OUTLine);
        errors := errors + 1;
      end if;  
      
      wait for clkPeriod;   
    end loop;
    if (errors = 0) then
      write(OUTLine,string'("Testbench successful"));
      writeline(OUTPUT,OUTLine);
    end if;
    wait;    
  end process;
  


end xorSbox_behav;


