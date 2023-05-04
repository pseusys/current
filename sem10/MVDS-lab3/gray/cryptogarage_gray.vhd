
-- 
-- Definition of  CryptoGarage
-- 
--      Wed 19 Apr 2023 10:15:32 AM CEST
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

architecture AutomatonGray of CryptoGarage is
   signal nx8, nx12, state_3, nx873, state_1, state_2, state_0, nx22, nx26, 
      nx874, nx36, nx40, nx44, nx58, nx64, nx875, nx78, nx86, nx90, nx876, 
      nx96, nx108, nx112, nx877, nx120, nx128, nx152, nx158, nx172, nx188, 
      nx196, nx234, nx242, nx274, nx284, nx302, nx314, nx886, nx888, nx891, 
      nx894, nx897, nx900, nx902, nx904, nx907, nx910, nx913, nx916, nx920, 
      nx923, nx926, nx929, nx932, nx934, nx936, nx940, nx942, nx945, nx947, 
      nx950, nx953, nx955, nx958, nx961, nx964, nx967, nx969, nx974, nx976, 
      nx981, nx983, nx985, nx988, nx992, nx994, nx997, nx1001, nx1004, 
      nx1008, nx1011: std_logic ;

-- PSL default clock is (clk'event and clk='1');

-- PSL property key_required is
--     always((state_3 = '0' and state_2 = '0' and state_1 = '0' and state_0 = '1') or (state_3 = '0' and state_2 = '0' and state_1 = '0' and state_0 = '0') -> (state_3 /= '0' or state_2 /= '1' or state_1 /= '1' or state_0 /= '0') until aes_key = '1');

-- PSL property maintain_open_light is
--     always((state_3 = '0' and state_2 = '1' and state_1 = '0' and state_0 = '1') -> (open_light = '1' -> command = "10" and bad_encryption = '0' and ACSC = '0') until (state_3 = '0' and state_2 = '1' and state_1 = '1' and state_0 = '1'));

-- PSL property bad_encryption_management is
--     always({ (state_3 = '0' and state_2 = '1' and state_1 = '1' and state_0 = '1') or (state_3 = '0' and state_2 = '1' and state_1 = '0' and state_0 = '1') ; (command = "01" or command = "10") and bad_encryption = '0' ; (bad_encryption = '0' and ACSC = '0')[*] ; bad_encryption = '1' } |-> (state_3 = '1' and state_2 = '1' and state_1 = '1' and state_0 = '1') before! (state_3 = '0' and state_2 = '0' and state_1 = '1' and state_0 = '0'));

-- PSL assert key_required;
-- PSL assert maintain_open_light;
-- PSL assert bad_encryption_management;

begin
   ix293 : OAI211 port map ( Q=>close_light, A=>nx40, B=>nx888, C=>nx997);
   ix41 : NAND21 port map ( Q=>nx40, A=>command(1), B=>nx886);
   ix887 : CLKIN1 port map ( Q=>nx886, A=>command(0));
   ix889 : AOI211 port map ( Q=>nx888, A=>switch, B=>nx274, C=>nx284);
   ix275 : NOR40 port map ( Q=>nx274, A=>nx891, B=>state_1, C=>ACSC, D=>
      nx877);
   ix129 : NAND41 port map ( Q=>nx128, A=>nx894, B=>nx976, C=>nx985, D=>
      nx988);
   ix895 : AOI211 port map ( Q=>nx894, A=>ACSC, B=>nx120, C=>nx112);
   ix121 : AOI2111 port map ( Q=>nx120, A=>nx897, B=>state_0, C=>nx904, D=>
      state_1);
   ix243 : OAI2111 port map ( Q=>nx242, A=>nx40, B=>nx900, C=>nx961, D=>
      nx967);
   ix901 : AOI221 port map ( Q=>nx900, A=>nx902, B=>nx942, C=>nx875, D=>
      nx876);
   ix903 : NOR21 port map ( Q=>nx902, A=>nx904, B=>state_3);
   ix159 : NAND31 port map ( Q=>nx158, A=>nx907, B=>nx955, C=>nx958);
   ix908 : OAI2111 port map ( Q=>nx907, A=>state_3, B=>nx891, C=>state_2, D
      =>nx913);
   reg_state_3 : DFC1 port map ( Q=>state_3, QN=>nx897, C=>clk, D=>nx242, RN
      =>nx910);
   ix911 : CLKIN1 port map ( Q=>nx910, A=>reset);
   reg_state_2 : DFC1 port map ( Q=>state_2, QN=>nx904, C=>clk, D=>nx158, RN
      =>nx910);
   ix197 : NAND31 port map ( Q=>nx196, A=>nx916, B=>nx926, C=>nx945);
   ix917 : OAI211 port map ( Q=>nx916, A=>nx188, B=>nx58, C=>nx26);
   ix189 : AOI211 port map ( Q=>nx188, A=>nx22, B=>timeout, C=>nx920);
   ix23 : NAND21 port map ( Q=>nx22, A=>command(1), B=>command(0));
   ix921 : NAND21 port map ( Q=>nx920, A=>nx891, B=>nx902);
   ix59 : NOR31 port map ( Q=>nx58, A=>state_2, B=>state_3, C=>nx923);
   ix924 : CLKIN1 port map ( Q=>nx923, A=>switch);
   ix27 : NAND21 port map ( Q=>nx26, A=>nx891, B=>nx913);
   ix927 : AOI221 port map ( Q=>nx926, A=>nx876, B=>nx96, C=>bad_encryption, 
      D=>nx108);
   ix135 : NOR21 port map ( Q=>nx876, A=>nx891, B=>nx929);
   ix930 : NAND21 port map ( Q=>nx929, A=>switch, B=>nx902);
   ix97 : NOR21 port map ( Q=>nx96, A=>nx932, B=>nx936);
   ix933 : NOR21 port map ( Q=>nx932, A=>nx934, B=>command(0));
   ix935 : CLKIN1 port map ( Q=>nx934, A=>command(1));
   ix937 : NOR21 port map ( Q=>nx936, A=>nx891, B=>state_1);
   reg_state_1 : DFC1 port map ( Q=>state_1, QN=>nx913, C=>clk, D=>nx196, RN
      =>nx910);
   ix109 : OAI211 port map ( Q=>nx108, A=>nx891, B=>nx929, C=>nx940);
   ix941 : NAND21 port map ( Q=>nx940, A=>state_2, B=>nx942);
   ix943 : NOR21 port map ( Q=>nx942, A=>state_0, B=>state_1);
   reg_state_0 : DFP1 port map ( Q=>state_0, QN=>nx891, C=>clk, D=>nx128, SN
      =>nx910);
   ix946 : AOI311 port map ( Q=>nx945, A=>state_0, B=>state_1, C=>nx947, D=>
      nx172);
   ix948 : NOR21 port map ( Q=>nx947, A=>nx904, B=>nx897);
   ix173 : NOR40 port map ( Q=>nx172, A=>nx950, B=>ACSC, C=>nx874, D=>nx953
   );
   ix951 : CLKIN1 port map ( Q=>nx950, A=>reached_top);
   ix167 : NAND21 port map ( Q=>nx874, A=>state_2, B=>nx897);
   ix954 : NAND21 port map ( Q=>nx953, A=>nx40, B=>nx942);
   ix956 : NAND41 port map ( Q=>nx955, A=>nx897, B=>authentication, C=>
      switch, D=>nx152);
   ix153 : NOR31 port map ( Q=>nx152, A=>state_0, B=>state_2, C=>nx913);
   ix959 : AOI311 port map ( Q=>nx958, A=>nx22, B=>nx891, C=>nx902, D=>nx876
   );
   ix205 : NAND21 port map ( Q=>nx875, A=>state_0, B=>nx913);
   ix962 : AOI221 port map ( Q=>nx961, A=>ACSC, B=>nx120, C=>bad_encryption, 
      D=>nx234);
   ix235 : NAND21 port map ( Q=>nx234, A=>nx940, B=>nx964);
   ix965 : NAND21 port map ( Q=>nx964, A=>state_0, B=>nx86);
   ix87 : NOR21 port map ( Q=>nx86, A=>nx923, B=>nx874);
   ix968 : OAI2111 port map ( Q=>nx967, A=>nx969, B=>state_0, C=>nx873, D=>
      nx913);
   ix970 : CLKIN1 port map ( Q=>nx969, A=>reached_bottom);
   ix257 : AOI211 port map ( Q=>nx873, A=>nx934, B=>command(0), C=>nx877);
   ix253 : NAND21 port map ( Q=>nx877, A=>state_2, B=>state_3);
   ix113 : OAI311 port map ( Q=>nx112, A=>nx964, B=>nx932, C=>nx936, D=>
      nx974);
   ix975 : NAND21 port map ( Q=>nx974, A=>bad_encryption, B=>nx108);
   ix977 : AOI221 port map ( Q=>nx976, A=>nx12, B=>nx90, C=>nx942, D=>nx78);
   ix13 : NAND21 port map ( Q=>nx12, A=>nx934, B=>command(0));
   ix91 : NOR21 port map ( Q=>nx90, A=>nx875, B=>nx929);
   ix79 : OAI211 port map ( Q=>nx78, A=>nx969, B=>nx981, C=>nx983);
   ix982 : NAND21 port map ( Q=>nx981, A=>nx12, B=>nx947);
   ix984 : NAND31 port map ( Q=>nx983, A=>nx904, B=>nx897, C=>switch);
   ix986 : AOI311 port map ( Q=>nx985, A=>nx873, B=>nx40, C=>nx936, D=>nx64
   );
   ix65 : NOR31 port map ( Q=>nx64, A=>nx983, B=>aes_key, C=>nx891);
   ix989 : AOI311 port map ( Q=>nx988, A=>nx44, B=>reached_top, C=>nx902, D
      =>nx36);
   ix45 : NOR21 port map ( Q=>nx44, A=>nx932, B=>nx26);
   ix37 : NOR40 port map ( Q=>nx36, A=>nx992, B=>nx994, C=>nx942, D=>nx920);
   ix993 : CLKIN1 port map ( Q=>nx992, A=>timeout);
   ix995 : NOR21 port map ( Q=>nx994, A=>nx934, B=>nx886);
   ix285 : OAI221 port map ( Q=>nx284, A=>nx913, B=>nx964, C=>nx26, D=>nx929
   );
   ix998 : NAND41 port map ( Q=>nx997, A=>nx8, B=>nx873, C=>switch, D=>nx942
   );
   ix9 : NOR31 port map ( Q=>nx8, A=>bad_encryption, B=>reached_bottom, C=>
      ACSC);
   ix325 : OAI211 port map ( Q=>open_light, A=>nx12, B=>nx1001, C=>nx1004);
   ix1002 : AOI211 port map ( Q=>nx1001, A=>switch, B=>nx314, C=>nx90);
   ix315 : AOI2111 port map ( Q=>nx314, A=>state_0, B=>ACSC, C=>state_1, D=>
      nx877);
   ix1005 : NAND31 port map ( Q=>nx1004, A=>nx302, B=>nx86, C=>nx44);
   ix303 : NOR31 port map ( Q=>nx302, A=>bad_encryption, B=>reached_top, C=>
      ACSC);
   ix155 : NOR40 port map ( Q=>authentication_led, A=>nx1008, B=>state_0, C
      =>state_2, D=>nx913);
   ix1009 : NAND31 port map ( Q=>nx1008, A=>nx897, B=>authentication, C=>
      switch);
   ix329 : NOR31 port map ( Q=>key_led, A=>nx983, B=>nx1011, C=>nx891);
   ix1012 : CLKIN1 port map ( Q=>nx1011, A=>aes_key);
end AutomatonGray ;

