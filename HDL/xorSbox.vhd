-------------------------------------------------------------------------------
-- Title      : First stage AES
-- Project    : FSA tool
-------------------------------------------------------------------------------
-- File       : xorSbox.vhd
-- Author     : Felipe Valencia  <valena@usi.ch>
-- Company    : Alari
-- Created    : 2019-09-20
-- Last update: 2019-09-20
-- Platform   : ModelSim (simulation), Synopsys (synthesis)
-- Standard   : VHDL'2008
-------------------------------------------------------------------------------
-- Description: This module is an abstraction of the the first stage of AES.
-- It is composed of the xor and sbox.
-------------------------------------------------------------------------------
-- Copyright (c) 2017 Alari, USI Lugano
-------------------------------------------------------------------------------
-- Revisions  :
-- Date        Version  Author  Description
-- 2019-09-20  1.0      afvp	Created
-------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use IEEE.std_logic_unsigned.all;

entity xorSbox is
  port( ClkxCI    : in  std_logic;
        Operand1xDI     : in  std_logic_vector(7 downto 0);
  		Operand2xDI     : in  std_logic_vector(7 downto 0);
        OutxDO    : out std_logic_vector(7 downto 0));

end xorSbox;

    


architecture xorSbox_arch of xorSbox is

    
    component Sbox is
    port(
            inputxDI  : in  std_logic_vector(7 downto 0);
            outputxDO : out std_logic_vector(7 downto 0));

    end component;

    signal sboxxDI: std_logic_vector(7 downto 0);
    signal sboxxDO: std_logic_vector(7 downto 0);
  
begin
            
    sboxxDI <= Operand1xDI xor Operand2xDI;
            
    genSbox: Sbox 
    port map(
            inputxDI  => sboxxDI,
            outputxDO => sboxxDO);
            
    
    output: process (ClkxCI) begin
        if RISING_EDGE(clkxCI) then
            OutxDO <= sboxxDO; 
        end if;
    end process;




end architecture xorSbox_arch;
