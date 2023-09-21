library ieee;
use ieee.std_logic_1164.all;



entity CryptoGarageTest is
end CryptoGarageTest;



architecture Test of CryptoGarageTest is
	component CryptoGarage is
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
	end component;
	signal reset, clk, aes_key, authentication, reached_bottom, reached_top, ACSC, bad_encryption, timeout, switch, key_led, authentication_led, open_light, close_light, key_led_gray, authentication_led_gray, open_light_gray, close_light_gray: std_logic;
	signal command: std_logic_vector(0 to 1);

-- PSL default clock is (clk'event and clk='1');

-- PSL property equivalence is
--     always(key_led = key_led_gray and authentication_led = authentication_led_gray and open_light = open_light_gray and close_light = close_light_gray);

-- PSL assert equivalence;

begin
	-- clock process
	process
	begin
		clk <= '0';
		wait for 5 ns;
		clk <= '1';
		wait for 5 ns;
	end process;

	-- CG/G port mappings
	CG: CryptoGarage
		generic map (init_closed=>true)
		port map (reset=>reset, clk=>clk, aes_key=>aes_key, authentication=>authentication, reached_bottom=>reached_bottom, reached_top=>reached_top, ACSC=>ACSC, bad_encryption=>bad_encryption, timeout=>timeout, switch=>switch, key_led=>key_led, authentication_led=>authentication_led, open_light=>open_light, close_light=>close_light, command=>command);

	CGG: CryptoGarage
		generic map (init_closed=>true)
		port map (reset=>reset, clk=>clk, aes_key=>aes_key, authentication=>authentication, reached_bottom=>reached_bottom, reached_top=>reached_top, ACSC=>ACSC, bad_encryption=>bad_encryption, timeout=>timeout, switch=>switch, key_led=>key_led_gray, authentication_led=>authentication_led_gray, open_light=>open_light_gray, close_light=>close_light_gray, command=>command);

	-- powerup test (from 0 to 20 ns)
	reset <= '1', '0' after 3 ns;

	-- no key test (from 20 to 40 ns)
	aes_key <= '0', '1' after 20 ns;

	-- deactivated test (from 40 to 60 ns), then turning off test
	switch <= '1', '0' after 40 ns, '1' after 60 ns, '0' after 360 ns;

	-- activated test (from 80 to 100)
	authentication <= '0', '1' after 80 ns;

	-- return to deactivated test (from 100 to 120 ns), then moving on up test, then moving on down test
	command <= "00", "11" after 100 ns, "00" after 120 ns, "10" after 160 ns, "01" after 200 ns, "10" after 300 ns;

	-- down test (from 140 to 160 ns)
	timeout <= '0', '1' after 140 ns;

	-- moving on up test (from 160 to 180 ns), up test (from 180 to 200 ns)
	reached_top <= '0', '1' after 180 ns, '0' after 280 ns;

	-- safety error test (from 220 to 240 ns)
	ACSC <= '0', '1' after 220 ns, '0' after 240 ns;

	-- moving on down test (from 200 to 220 ns and from 260 to 280 ns)
	reached_bottom <= '0', '1' after 280 ns;

	-- security error test (from 320 to 340 ns)
	bad_encryption <= '0', '1' after 340 ns;
end test;



configuration CryptoGarageConfig of work.CryptoGarageTest is 
	for Test
		for CG: CryptoGarage use entity work.CryptoGarage(Automaton);
		end for;
		for CGG: CryptoGarage use entity work.CryptoGarage(AutomatonGray);
		end for;
	end for; 
end CryptoGarageConfig; 

