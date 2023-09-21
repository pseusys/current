
-- 
-- Definition of  CryptoGarage
-- 
--      Wed 19 Apr 2023 09:59:21 AM CEST
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


architecture AutomatonBinary of CryptoGarage is
   signal nx0, state_3, nx872, nx10, state_2, state_1, nx26, nx30, state_0, 
      nx50, nx60, nx873, nx66, nx80, nx86, nx92, nx98, nx102, nx108, nx114, 
      nx130, nx142, nx148, nx874, nx156, nx160, nx170, nx178, nx184, nx194, 
      nx204, nx210, nx224, nx236, nx250, nx272, nx278, nx306, nx316, nx338, 
      nx354, nx368, nx388, nx402, nx882, nx884, nx886, nx888, nx891, nx893, 
      nx896, nx898, nx903, nx905, nx908, nx910, nx912, nx915, nx919, nx921, 
      nx926, nx929, nx931, nx933, nx940, nx943, nx946, nx948, nx950, nx953, 
      nx957, nx960, nx962, nx965, nx968, nx971, nx974, nx976, nx979, nx981, 
      nx984, nx989, nx992, nx995, nx997, nx1000, nx1004, nx1010, nx1013, 
      nx1016, nx1019, nx1023: std_logic ;

-- PSL default clock is (clk'event and clk='1');

-- PSL property key_required is
--     always((state_3 = '0' and state_2 = '0' and state_1 = '0' and state_0 = '1') or (state_3 = '0' and state_2 = '0' and state_1 = '0' and state_0 = '0') -> (state_2 /= '1' or state_1 /= '0' or state_0 /= '0') until aes_key = '1');

-- PSL property maintain_open_light is
--     always((state_2 = '1' and state_1 = '1' and state_0 = '0') -> (open_light = '1' -> command = "10" and bad_encryption = '0' and ACSC = '0') until (state_2 = '1' and state_1 = '0' and state_0 = '1'));

-- PSL property bad_encryption_management is
--     always({ (state_2 = '1' and state_1 = '0' and state_0 = '1') or (state_2 = '1' and state_1 = '1' and state_0 = '0') ; (command = "01" or command = "10") and bad_encryption = '0' ; (bad_encryption = '0' and ACSC = '0')[*] ; bad_encryption = '1' } |-> (state_3 = '1' and state_2 = '0' and state_1 = '1' and state_0 = '0') before! (state_2 = '0' and state_1 = '1' and state_0 = '1'));

-- PSL assert key_required;
-- PSL assert maintain_open_light;
-- PSL assert bad_encryption_management;

begin
   ix377 : NOR21 port map ( Q=>close_light, A=>nx882, B=>nx884);
   ix883 : CLKIN1 port map ( Q=>nx882, A=>switch);
   ix885 : AOI211 port map ( Q=>nx884, A=>nx886, B=>nx368, C=>nx354);
   ix887 : NOR21 port map ( Q=>nx886, A=>nx888, B=>command(0));
   ix889 : CLKIN1 port map ( Q=>nx888, A=>command(1));
   ix369 : OAI221 port map ( Q=>nx368, A=>ACSC, B=>nx891, C=>nx893, D=>nx912
   );
   ix892 : NAND31 port map ( Q=>nx891, A=>nx893, B=>state_3, C=>nx931);
   ix307 : NAND41 port map ( Q=>nx306, A=>nx896, B=>nx997, C=>nx1000, D=>
      nx1004);
   ix897 : AOI221 port map ( Q=>nx896, A=>nx898, B=>nx86, C=>nx931, D=>nx130
   );
   ix899 : NOR21 port map ( Q=>nx898, A=>nx10, B=>ACSC);
   ix11 : NAND21 port map ( Q=>nx10, A=>nx888, B=>command(0));
   ix87 : NOR40 port map ( Q=>nx86, A=>state_2, B=>nx905, C=>nx912, D=>
      state_1);
   reg_state_2 : DFC1 port map ( Q=>state_2, QN=>nx893, C=>clk, D=>nx306, RN
      =>nx903);
   ix904 : CLKIN1 port map ( Q=>nx903, A=>reset);
   ix339 : OAI2111 port map ( Q=>nx338, A=>nx893, B=>nx908, C=>nx989, D=>
      nx992);
   ix909 : AOI221 port map ( Q=>nx908, A=>nx910, B=>nx30, C=>nx931, D=>nx316
   );
   ix911 : NOR21 port map ( Q=>nx910, A=>nx912, B=>nx929);
   ix171 : NAND41 port map ( Q=>nx170, A=>nx915, B=>nx968, C=>nx971, D=>
      nx981);
   ix916 : AOI211 port map ( Q=>nx915, A=>nx108, B=>nx921, C=>nx160);
   ix109 : NOR21 port map ( Q=>nx108, A=>state_3, B=>nx919);
   reg_state_3 : DFC1 port map ( Q=>state_3, QN=>nx905, C=>clk, D=>nx338, RN
      =>nx903);
   ix920 : NAND21 port map ( Q=>nx919, A=>switch, B=>nx893);
   ix922 : NOR21 port map ( Q=>nx921, A=>state_0, B=>state_1);
   reg_state_0 : DFP1 port map ( Q=>state_0, QN=>nx912, C=>clk, D=>nx170, SN
      =>nx903);
   ix225 : NAND31 port map ( Q=>nx224, A=>nx926, B=>nx933, C=>nx943);
   ix927 : AOI221 port map ( Q=>nx926, A=>switch, B=>nx102, C=>nx931, D=>
      nx108);
   ix103 : NOR21 port map ( Q=>nx102, A=>nx929, B=>state_0);
   reg_state_1 : DFC1 port map ( Q=>state_1, QN=>nx929, C=>clk, D=>nx224, RN
      =>nx903);
   ix932 : NOR21 port map ( Q=>nx931, A=>nx912, B=>state_1);
   ix934 : AOI2111 port map ( Q=>nx933, A=>state_1, B=>nx66, C=>nx156, D=>
      nx148);
   ix67 : NOR21 port map ( Q=>nx66, A=>state_0, B=>nx873);
   ix347 : NAND21 port map ( Q=>nx873, A=>nx893, B=>state_3);
   ix157 : NOR40 port map ( Q=>nx156, A=>nx882, B=>state_2, C=>
      authentication, D=>nx874);
   ix235 : NAND21 port map ( Q=>nx874, A=>state_0, B=>state_1);
   ix149 : NOR40 port map ( Q=>nx148, A=>nx940, B=>nx888, C=>nx893, D=>nx142
   );
   ix941 : CLKIN1 port map ( Q=>nx940, A=>command(0));
   ix143 : NAND21 port map ( Q=>nx142, A=>nx912, B=>nx929);
   ix944 : AOI2111 port map ( Q=>nx943, A=>nx66, B=>nx184, C=>nx210, D=>
      nx178);
   ix185 : OAI211 port map ( Q=>nx184, A=>ACSC, B=>nx946, C=>nx950);
   ix947 : NOR21 port map ( Q=>nx946, A=>nx948, B=>reached_bottom);
   ix949 : NOR21 port map ( Q=>nx948, A=>command(1), B=>nx940);
   ix951 : CLKIN1 port map ( Q=>nx950, A=>bad_encryption);
   ix211 : OAI221 port map ( Q=>nx210, A=>nx893, B=>nx953, C=>nx204, D=>
      nx891);
   ix954 : AOI221 port map ( Q=>nx953, A=>timeout, B=>nx921, C=>nx194, D=>
      nx931);
   ix195 : NOR21 port map ( Q=>nx194, A=>nx950, B=>nx882);
   ix205 : NAND21 port map ( Q=>nx204, A=>nx948, B=>nx957);
   ix958 : CLKIN1 port map ( Q=>nx957, A=>ACSC);
   ix179 : NOR40 port map ( Q=>nx178, A=>nx960, B=>nx912, C=>nx893, D=>nx929
   );
   ix961 : AOI311 port map ( Q=>nx960, A=>nx962, B=>nx26, C=>nx965, D=>
      bad_encryption);
   ix963 : CLKIN1 port map ( Q=>nx962, A=>reached_top);
   ix27 : NAND21 port map ( Q=>nx26, A=>command(1), B=>nx940);
   ix966 : NOR21 port map ( Q=>nx965, A=>bad_encryption, B=>ACSC);
   ix161 : CLKIN1 port map ( Q=>nx160, A=>nx933);
   ix969 : AOI221 port map ( Q=>nx968, A=>nx26, B=>nx86, C=>nx931, D=>nx130
   );
   ix131 : NOR40 port map ( Q=>nx130, A=>bad_encryption, B=>nx882, C=>nx893, 
      D=>nx886);
   ix972 : AOI221 port map ( Q=>nx971, A=>aes_key, B=>nx114, C=>ACSC, D=>
      nx92);
   ix115 : OAI221 port map ( Q=>nx114, A=>state_1, B=>nx974, C=>nx919, D=>
      nx976);
   ix975 : NAND31 port map ( Q=>nx974, A=>nx905, B=>switch, C=>nx893);
   ix977 : NAND21 port map ( Q=>nx976, A=>state_1, B=>nx912);
   ix93 : OAI211 port map ( Q=>nx92, A=>bad_encryption, B=>nx979, C=>nx891);
   ix980 : NAND31 port map ( Q=>nx979, A=>nx912, B=>nx893, C=>state_3);
   ix982 : AOI311 port map ( Q=>nx981, A=>nx66, B=>nx965, C=>nx948, D=>nx60
   );
   ix61 : NOR40 port map ( Q=>nx60, A=>nx984, B=>bad_encryption, C=>nx893, D
      =>nx929);
   ix985 : AOI311 port map ( Q=>nx984, A=>nx948, B=>switch, C=>nx912, D=>
      nx50);
   ix51 : AOI211 port map ( Q=>nx50, A=>nx886, B=>nx965, C=>nx912);
   ix31 : NAND21 port map ( Q=>nx30, A=>nx26, B=>nx965);
   ix317 : AOI211 port map ( Q=>nx316, A=>nx26, B=>nx950, C=>nx882);
   ix990 : AOI221 port map ( Q=>nx989, A=>nx194, B=>nx236, C=>nx204, D=>nx86
   );
   ix237 : NOR31 port map ( Q=>nx236, A=>state_0, B=>nx929, C=>nx893);
   ix993 : OAI211 port map ( Q=>nx992, A=>nx946, B=>nx0, C=>nx995);
   ix1 : NAND21 port map ( Q=>nx0, A=>nx950, B=>nx957);
   ix996 : NOR40 port map ( Q=>nx995, A=>state_0, B=>state_1, C=>state_2, D
      =>nx905);
   ix998 : NAND31 port map ( Q=>nx997, A=>nx98, B=>authentication, C=>nx910
   );
   ix99 : NOR21 port map ( Q=>nx98, A=>nx882, B=>state_2);
   ix1001 : OAI211 port map ( Q=>nx1000, A=>nx278, B=>nx272, C=>state_2);
   ix279 : NOR21 port map ( Q=>nx278, A=>nx874, B=>nx30);
   ix273 : AOI2111 port map ( Q=>nx272, A=>command(1), B=>command(0), C=>
      state_1, D=>state_0);
   ix1005 : AOI311 port map ( Q=>nx1004, A=>nx236, B=>switch, C=>nx950, D=>
      nx250);
   ix251 : NOR40 port map ( Q=>nx250, A=>nx946, B=>bad_encryption, C=>ACSC, 
      D=>nx872);
   ix349 : NAND31 port map ( Q=>nx872, A=>nx921, B=>nx893, C=>state_3);
   ix355 : NOR40 port map ( Q=>nx354, A=>reached_bottom, B=>nx0, C=>nx872, D
      =>nx898);
   ix407 : CLKIN1 port map ( Q=>open_light, A=>nx1010);
   ix1011 : AOI211 port map ( Q=>nx1010, A=>switch, B=>nx402, C=>nx388);
   ix403 : OAI221 port map ( Q=>nx402, A=>nx10, B=>nx1013, C=>nx204, D=>
      nx891);
   ix1014 : NOR21 port map ( Q=>nx1013, A=>nx236, B=>nx995);
   ix389 : NOR40 port map ( Q=>nx388, A=>reached_top, B=>nx0, C=>nx874, D=>
      nx1016);
   ix1017 : NAND41 port map ( Q=>nx1016, A=>nx950, B=>switch, C=>state_2, D
      =>nx26);
   ix293 : NOR40 port map ( Q=>authentication_led, A=>nx882, B=>state_2, C=>
      nx1019, D=>nx874);
   ix1020 : CLKIN1 port map ( Q=>nx1019, A=>authentication);
   ix415 : AOI2111 port map ( Q=>key_led, A=>nx976, B=>nx80, C=>nx1023, D=>
      nx974);
   ix81 : NAND21 port map ( Q=>nx80, A=>state_0, B=>nx929);
   ix1024 : CLKIN1 port map ( Q=>nx1023, A=>aes_key);
end AutomatonBinary ;

