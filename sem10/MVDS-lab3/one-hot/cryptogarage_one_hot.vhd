
-- 
-- Definition of  CryptoGarage
-- 
--      Wed 19 Apr 2023 10:10:37 AM CEST
--      
--      LeonardoSpectrum Level 3, 2015a.6
-- 

library IEEE;
use IEEE.STD_LOGIC_1164.all;

library c35_CORELIB;
use c35_CORELIB.vcomponents.all;

entity CryptoGarage is
	generic (
		init_closed: boolean := true  -- Initially the door is closed (true) or open (false)
	);

	port (
		reset,  -- Asynchronous high reset
		clk,  -- Clock with frequency 100MHz, full cycle in 10ns, semicycle in 5ns
		aes_key,  -- AES encryption key is available
		authentication,  -- User has been identified
		reached_bottom,  -- Garage door has reached its fully closed position
		reached_top,  -- Garage door has reached its fully open position
		ACSC,  -- Anti-Crush-Small-Children: it is necessary to cause an emergency stop of the movement of the door
		bad_encryption,  -- Transmission is not encrypted with the correct key
		timeout,  -- Timeout for user to think if he wants to deactivate the gate
		switch  -- Power switch button pressed
			: in std_logic;

		command  -- Entered command: d_close ("01"), d_open ("10"), and deactivate ("11").
			: in std_logic_vector(0 to 1);

		key_led,  -- Flash key light when AES key is entered
		authentication_led,  -- Flash authentication light when authentication is completed
		open_light,  -- Turn on the lights while opening the garage door
		close_light  -- Turn on the lights while closing the garage door
			: out std_logic
	);
end CryptoGarage;

architecture AutomatonOneHot of CryptoGarage is
   signal state_8, nx1514, nx10, nx16, nx20, nx24, state_7, nx1515, nx28, 
      state_6, state_4, state_3, nx42, state_5, nx54, nx60, nx74, state_2, 
      nx84, state_1, nx100, nx106, nx120, nx142, nx154, nx162, state_9, 
      nx1517, nx176, nx182, nx198, nx204, nx218, nx220, nx1525, nx1529, 
      nx1531, nx1533, nx1536, nx1539, nx1543, nx1547, nx1549, nx1551, nx1554, 
      nx1556, nx1559, nx1562, nx1565, nx1567, nx1569, nx1571, nx1573, nx1576, 
      nx1580, nx1584, nx1586, nx1588, nx1592, nx1596, nx1599, nx1604, nx1607, 
      nx1610, nx1612, nx1615, nx1617, nx1618, nx1620, nx1622, nx1625, nx1628, 
      nx1630, nx1633, nx1636, nx1638, nx1642, nx1645, nx1648: std_logic ;

-- PSL default clock is (clk'event and clk='1');

-- PSL property key_required is
--     always(state_1 = '1' or nx1596 /= '1' -> state_4 /= '1' until aes_key = '1');

-- PSL property maintain_open_light is
--     always(state_6 = '1' -> (open_light = '1' -> command = "10" and bad_encryption = '0' and ACSC = '0') until state_5 = '1');

-- PSL property bad_encryption_management is
--     always({ state_5 = '1' or state_6 = '1' ; (command = "01" or command = "10") and bad_encryption = '0' ; (bad_encryption = '0' and ACSC = '0')[*] ; bad_encryption = '1' } |-> nx1607 /= '1' before! state_3 = '1');

-- PSL assert key_required;
-- PSL assert maintain_open_light;
-- PSL assert bad_encryption_management;

begin
   ix237 : AOI211 port map ( Q=>close_light, A=>nx1525, B=>nx1625, C=>nx1536
   );
   ix1526 : OAI211 port map ( Q=>nx1525, A=>state_5, B=>state_7, C=>nx1529);
   ix61 : NOR21 port map ( Q=>nx60, A=>nx1529, B=>nx1533);
   ix1530 : NOR21 port map ( Q=>nx1529, A=>nx1531, B=>command(0));
   ix1532 : CLKIN1 port map ( Q=>nx1531, A=>command(1));
   ix1534 : AOI221 port map ( Q=>nx1533, A=>state_5, B=>nx54, C=>reached_top, 
      D=>nx1515);
   ix55 : NOR21 port map ( Q=>nx54, A=>bad_encryption, B=>nx1536);
   ix1537 : CLKIN1 port map ( Q=>nx1536, A=>switch);
   ix211 : NOR31 port map ( Q=>nx1515, A=>nx1539, B=>bad_encryption, C=>ACSC
   );
   reg_state_7 : DFC1 port map ( Q=>state_7, QN=>nx1539, C=>clk, D=>nx204, 
      RN=>nx1567);
   ix205 : OAI211 port map ( Q=>nx204, A=>nx10, B=>nx1543, C=>nx1620);
   ix11 : NAND21 port map ( Q=>nx10, A=>nx1531, B=>command(0));
   ix1544 : AOI221 port map ( Q=>nx1543, A=>state_6, B=>nx54, C=>nx1618, D=>
      nx1517);
   reg_state_6 : DFC1 port map ( Q=>state_6, QN=>nx1617, C=>clk, D=>nx162, 
      RN=>nx1567);
   ix163 : OAI211 port map ( Q=>nx162, A=>nx1547, B=>nx1551, C=>nx1580);
   ix1548 : NOR21 port map ( Q=>nx1547, A=>command(1), B=>nx1549);
   ix1550 : CLKIN1 port map ( Q=>nx1549, A=>command(0));
   ix1552 : AOI211 port map ( Q=>nx1551, A=>state_6, B=>nx54, C=>nx154);
   ix155 : NOR40 port map ( Q=>nx154, A=>bad_encryption, B=>ACSC, C=>nx1554, 
      D=>nx1556);
   ix1555 : CLKIN1 port map ( Q=>nx1554, A=>reached_bottom);
   reg_state_8 : DFC1 port map ( Q=>state_8, QN=>nx1556, C=>clk, D=>nx1514, 
      RN=>nx1567);
   ix227 : OAI311 port map ( Q=>nx1514, A=>reached_bottom, B=>nx1556, C=>
      nx1559, D=>nx1562);
   ix1560 : NAND21 port map ( Q=>nx1559, A=>nx10, B=>nx16);
   ix17 : NOR21 port map ( Q=>nx16, A=>bad_encryption, B=>ACSC);
   ix1563 : OAI211 port map ( Q=>nx1562, A=>nx218, B=>nx1515, C=>nx1529);
   ix219 : OAI221 port map ( Q=>nx218, A=>nx1565, B=>nx1569, C=>ACSC, D=>
      nx1573);
   reg_state_5 : DFC1 port map ( Q=>state_5, QN=>nx1565, C=>clk, D=>nx60, RN
      =>nx1567);
   ix1568 : CLKIN1 port map ( Q=>nx1567, A=>reset);
   ix1570 : NAND21 port map ( Q=>nx1569, A=>nx1571, B=>switch);
   ix1572 : CLKIN1 port map ( Q=>nx1571, A=>bad_encryption);
   reg_state_9 : DFC1 port map ( Q=>state_9, QN=>nx1573, C=>clk, D=>nx182, 
      RN=>nx1567);
   ix183 : OAI311 port map ( Q=>nx182, A=>nx1547, B=>nx1573, C=>nx1529, D=>
      nx1576);
   ix1577 : OAI211 port map ( Q=>nx1576, A=>nx176, B=>nx1517, C=>ACSC);
   ix177 : NOR21 port map ( Q=>nx176, A=>bad_encryption, B=>nx1539);
   ix193 : OAI211 port map ( Q=>nx1517, A=>bad_encryption, B=>nx1556, C=>
      nx1573);
   ix1581 : OAI2111 port map ( Q=>nx1580, A=>nx1531, B=>nx1549, C=>timeout, 
      D=>state_4);
   ix143 : OAI311 port map ( Q=>nx142, A=>nx1584, B=>timeout, C=>nx1586, D=>
      nx1588);
   ix1585 : NOR21 port map ( Q=>nx1584, A=>nx1531, B=>nx1549);
   reg_state_4 : DFC1 port map ( Q=>state_4, QN=>nx1586, C=>clk, D=>nx142, 
      RN=>nx1567);
   ix1589 : NAND31 port map ( Q=>nx1588, A=>state_3, B=>authentication, C=>
      switch);
   ix121 : NAND31 port map ( Q=>nx120, A=>nx1592, B=>nx1607, C=>nx1612);
   ix1593 : OAI2111 port map ( Q=>nx1592, A=>state_1, B=>state_2, C=>aes_key, 
      D=>switch);
   reg_state_1 : DFP1 port map ( Q=>state_1, QN=>OPEN, C=>clk, D=>nx106, SN
      =>nx1567);
   ix107 : NOR21 port map ( Q=>nx106, A=>nx1536, B=>nx1596);
   reg_state_0 : DFC1 port map ( Q=>OPEN, QN=>nx1596, C=>clk, D=>nx100, RN=>
      nx1567);
   ix101 : AOI311 port map ( Q=>nx100, A=>nx1599, B=>nx1596, C=>nx1604, D=>
      switch);
   ix1600 : NOR21 port map ( Q=>nx1599, A=>state_1, B=>state_2);
   reg_state_2 : DFC1 port map ( Q=>state_2, QN=>OPEN, C=>clk, D=>nx84, RN=>
      nx1567);
   ix85 : NOR31 port map ( Q=>nx84, A=>nx1599, B=>aes_key, C=>nx1536);
   ix1605 : NOR31 port map ( Q=>nx1604, A=>state_5, B=>state_6, C=>state_3);
   reg_state_10 : DFC1 port map ( Q=>OPEN, QN=>nx1607, C=>clk, D=>nx74, RN=>
      nx1567);
   ix75 : AOI311 port map ( Q=>nx74, A=>nx1610, B=>nx1539, C=>nx1556, D=>
      nx1571);
   ix1611 : OAI211 port map ( Q=>nx1610, A=>state_5, B=>state_6, C=>switch);
   ix1613 : AOI311 port map ( Q=>nx1612, A=>command(1), B=>command(0), C=>
      state_4, D=>nx42);
   ix43 : NOR31 port map ( Q=>nx42, A=>nx1615, B=>authentication, C=>nx1536
   );
   reg_state_3 : DFC1 port map ( Q=>state_3, QN=>nx1615, C=>clk, D=>nx120, 
      RN=>nx1567);
   ix1619 : CLKIN1 port map ( Q=>nx1618, A=>ACSC);
   ix1621 : NAND31 port map ( Q=>nx1620, A=>nx1515, B=>nx1622, C=>nx24);
   ix1623 : CLKIN1 port map ( Q=>nx1622, A=>reached_top);
   ix25 : NAND21 port map ( Q=>nx24, A=>command(1), B=>nx1549);
   ix1626 : AOI211 port map ( Q=>nx1625, A=>nx1529, B=>nx220, C=>nx20);
   ix221 : NAND21 port map ( Q=>nx220, A=>nx1628, B=>nx1630);
   ix1629 : AOI221 port map ( Q=>nx1628, A=>nx1618, B=>state_9, C=>state_5, 
      D=>nx54);
   ix1631 : NAND21 port map ( Q=>nx1630, A=>state_7, B=>nx16);
   ix21 : NOR40 port map ( Q=>nx20, A=>reached_bottom, B=>nx1556, C=>nx1547, 
      D=>nx1633);
   ix1634 : NAND21 port map ( Q=>nx1633, A=>nx1571, B=>nx1618);
   ix247 : AOI211 port map ( Q=>open_light, A=>nx1636, B=>nx1638, C=>nx1536
   );
   ix1637 : OAI211 port map ( Q=>nx1636, A=>state_6, B=>state_8, C=>nx1547);
   ix1639 : AOI221 port map ( Q=>nx1638, A=>nx1515, B=>nx28, C=>nx1547, D=>
      nx198);
   ix29 : NOR21 port map ( Q=>nx28, A=>reached_top, B=>nx1529);
   ix199 : OAI221 port map ( Q=>nx198, A=>ACSC, B=>nx1642, C=>nx1617, D=>
      nx1569);
   ix1643 : AOI211 port map ( Q=>nx1642, A=>nx1571, B=>state_8, C=>state_9);
   ix129 : NOR21 port map ( Q=>authentication_led, A=>nx1615, B=>nx1645);
   ix1646 : NAND21 port map ( Q=>nx1645, A=>authentication, B=>switch);
   ix117 : NOR31 port map ( Q=>key_led, A=>nx1599, B=>nx1648, C=>nx1536);
   ix1649 : CLKIN1 port map ( Q=>nx1648, A=>aes_key);
end AutomatonOneHot ;

