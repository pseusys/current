library ieee;
use ieee.std_logic_1164.all;





entity cryptogarage is
	generic (
		init_closed: boolean := true -- Initially the door is closed (true) or open (false)
	);

	port(
		reset, -- Asynchronous low reset
		clk, -- Clock with frequency 100MHz, full cycle in 10ns, semicycle in 5ns
		aes_key, -- AES encryption key is available
		authentication, -- User has been identified
		reached_bottom, -- Garage door has reached its fully closed position
		reached_top, -- Garage door has reached its fully open position
		ACSC, -- Anti-Crush-Small-Children: it is necessary to cause an emergency stop of the movement of the door
		bad_encryption, -- Transmission is not encrypted with the correct key
		timeout -- Timeout for user to think if he wants to deactivate the gate
			: in std_logic;

		switch: in std_logic; -- Power switch button pressed

		command: in std_logic_vector(0 to 1); -- Entered command: d_close ("01"), d_open ("10"), and deactivate ("11")

		key_led, -- Flash key light when AES key is entered
		authentication_led, -- Flash authentication light when authentication is completed
		open_light, -- Turn on the lights while opening the garage door
		close_light -- Turn on the lights while closing the garage door
			: out std_logic;

		LEDR: out std_logic_vector(9 downto 0); -- for red leds

		LEDG: out std_logic_vector(7 downto 0); -- for green leds
		
		seg_val_hex3, -- to display "command"
		seg_val_hex0 -- to display the state
			: out integer range 0 to 127
	);
end cryptogarage;





architecture Automaton of cryptogarage is
type States is (
	powerdown,  -- Switch is off, no power
	powerup,  -- Initial state
	nokey,
	deactivated,
	activated,
	up,
	down,
	movingonup,
	movingondown,
	safetyerror,
	securityerror
);
signal
	state,
	nextstate
		: States;

signal ck_1Hz: std_logic; -- Clock with frequency 1HZ

attribute chip_pin: string;
attribute chip_pin of reset: signal is "R22"; -- rightmost button
attribute chip_pin of clk: signal is "L1"; -- 50 MHz internal quartz

-- assignment of inputs to switches (here from left to right):
attribute chip_pin of command: signal is "L2, M1"; -- SW 9,8
attribute chip_pin of aes_key: signal is "M2"; -- SW 7
attribute chip_pin of authentication: signal is "U11"; -- SW 6
attribute chip_pin of reached_bottom: signal is "U12"; -- SW 5
attribute chip_pin of reached_top: signal is "W12"; -- SW 4
attribute chip_pin of ACSC: signal is "V12"; -- SW 3
attribute chip_pin of bad_encryption: signal is "M22"; -- SW 2
attribute chip_pin of timeout: signal is "L21"; -- SW 1
attribute chip_pin of switch: signal is "L22"; -- SW 0

-- 7-segment displays:
attribute chip_pin of seg_val_hex0: signal is "J2, J1, H2, H1, F2, F1, E2";
attribute chip_pin of seg_val_hex3: signal is "F4, D5, D6, J4, L8, F3, D4";

-- assignment of outputs LEDR and LEDG to leds (here from left to right):
attribute chip_pin of LEDR: signal is "R17, R18, U18, Y18, V19, T18, Y19, U19, R19, R20";
attribute chip_pin of LEDG: signal is "Y21, Y22, W21, W22, V21, V22, U21, U22";
-- idem for LEDG (to be completed)



begin

	process (state, aes_key, authentication, reached_bottom, reached_top, ACSC, bad_encryption, timeout, switch, command)
	begin
		case state is
			when powerup =>
				if switch = '0' then nextstate <= powerdown;
				elsif aes_key = '1' then nextstate <= deactivated;
				else nextstate <= nokey;
				end if;
			when nokey =>
				if switch = '0' then nextstate <= powerdown;
				elsif aes_key = '1' then nextstate <= deactivated;
				else nextstate <= nokey;
				end if;
			when powerdown =>
				if switch = '0' then nextstate <= powerdown;
				else nextstate <= powerup;
				end if;
			when deactivated =>
				if switch = '0' then nextstate <= powerdown;
				elsif authentication = '0' then nextstate <= deactivated;
				else nextstate <= activated;
				end if;
			when activated =>
				if command = "11" then nextstate <= deactivated;
				elsif timeout = '0' then nextstate <= activated;
				elsif not init_closed then nextstate <= up;
				else nextstate <= down;
				end if;
			when down =>
				if switch = '0' then nextstate <= powerdown;
				elsif bad_encryption = '1' then nextstate <= securityerror;
				elsif command = "10" then nextstate <= movingonup;
				else nextstate <= down;
				end if;
			when up =>
				if switch = '0' then nextstate <= powerdown;
				elsif bad_encryption = '1' then nextstate <= securityerror;
				elsif command = "01" then nextstate <= movingondown;
				else nextstate <= up;
				end if;
			when movingonup =>
				if bad_encryption = '1' then nextstate <= securityerror;
				elsif acsc = '1' then nextstate <= safetyerror;
				elsif command = "01" then nextstate <= movingondown;
				elsif reached_top = '1' then nextstate <= up;
				else nextstate <= movingonup;
				end if;
			when movingondown =>
				if bad_encryption = '1' then nextstate <= securityerror;
				elsif acsc = '1' then nextstate <= safetyerror;
				elsif command = "10" then nextstate <= movingonup;
				elsif reached_bottom = '1' then nextstate <= down;
				else nextstate <= movingondown;
				end if;
			when safetyerror =>
				if acsc = '0' and command = "01" then nextstate <= movingondown;
				elsif acsc = '0' and command = "10" then nextstate <= movingonup;
				else nextstate <= safetyerror;
				end if;
			when securityerror =>
				nextstate <= deactivated;
		end case;
	end process;


	process (state, aes_key, authentication, reached_bottom, reached_top, ACSC, bad_encryption, timeout, switch, command)
	begin
		key_led <= '0';
		authentication_led <= '0';
		open_light <= '0';
		close_light <= '0';

		LEDG(7) <= '0';
		LEDG(6) <= '0';
		LEDG(5) <= '0';
		LEDG(4) <= '0';
		LEDG(3) <= '0';
		LEDG(2) <= '0';
		LEDG(1) <= '0';
		LEDG(0) <= '0';

		if switch = '1' and ((state = powerup and aes_key = '1') or state = nokey) then
			key_led <= '1';
			LEDG(7) <= '1';
		end if;
		if switch = '1' and state = deactivated then
			authentication_led <= '1';
			LEDG(6) <= '1';
		end if;
		if switch = '1' and ((state = down and command = "10")
			or (state = movingondown and command = "10")
			or (state = safetyerror and acsc = '0' and command = "10")
			or (state = movingonup and bad_encryption = '0' and acsc = '0' and command /= "01" and reached_top = '0'))
				then
			open_light <= '1';
			LEDG(1) <= '1';
		end if;
		if switch = '1' and ((state = up and command = "01")
			or (state = movingonup and command = "01")
			or (state = safetyerror and acsc = '0' and command = "01")
			or (state = movingondown and bad_encryption = '0' and acsc = '0' and command /= "10" and reached_bottom = '0'))
				then
			close_light <= '1';
			LEDG(0) <= '1';
		end if;

		seg_val_hex0 <= 2#1111111#;
		case state is
			when powerup =>
				seg_val_hex0 <= 2#0000001#; -- 0
			when nokey =>
				seg_val_hex0 <= 2#1001111#; -- 1
			when powerdown =>
				seg_val_hex0 <= 2#0010010#; -- 2
			when deactivated =>
				seg_val_hex0 <= 2#0000110#; -- 3
			when activated =>
				seg_val_hex0 <= 2#1001100#; -- 4
			when down =>
				seg_val_hex0 <= 2#0100100#; -- 5
			when up =>
				seg_val_hex0 <= 2#0100000#; -- 6
			when movingonup =>
				seg_val_hex0 <= 2#0001111#; -- 7
			when movingondown =>
				seg_val_hex0 <= 2#0000000#; -- 8
			when safetyerror =>
				seg_val_hex0 <= 2#0000100#; -- 9
			when securityerror =>
				seg_val_hex0 <= 2#0001000#; -- A
		end case;

		if command = "00" then seg_val_hex3 <= 2#0000001#; -- 0
		elsif command = "01" then seg_val_hex3 <= 2#1001111#; -- 1
		elsif command = "10" then seg_val_hex3 <= 2#0010010#; -- 2
		elsif command = "11" then seg_val_hex3 <= 2#0000110#; -- 3
		else seg_val_hex3 <= 2#1111111#; -- None
		end if;

		LEDR(9) <= command(0);
		LEDR(8) <= command(1);
		LEDR(7) <= aes_key;
		LEDR(6) <= authentication;
		LEDR(5) <= reached_bottom;
		LEDR(4) <= reached_top;
		LEDR(3) <= ACSC;
		LEDR(2) <= bad_encryption;
		LEDR(1) <= timeout;
		LEDR(0) <= switch;
	end process;


	-- Clock divider
	process (clk)
	variable cnt: integer range 0 to 67108863;
	constant verrou_t: integer := 50000000;
	begin
		if (clk'event and clk = '1') then
			if (reset = '0') or (cnt = verrou_t) then cnt := 0;
			else  cnt := cnt + 1;
			end if;
		end if;
		if (cnt = verrou_t) then ck_1Hz <= '1'; -- ck_1Hz local signal
		else ck_1Hz <= '0';
		end if;
	end process;


	process (ck_1Hz, reset)
	begin
		if reset = '0' then state <= powerup;
		elsif ck_1Hz'event and ck_1Hz = '1' then state <= nextstate;
		end if;
	end process;

end Automaton;

