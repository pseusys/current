library ieee;
use ieee.std_logic_1164.all;



entity CryptoGarage is
	generic (
		init_closed: boolean  -- Initially the door is closed (true) or open (false)
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



architecture Automaton of CryptoGarage is
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

-- PSL default clock is (clk'event and clk='1');

-- PSL property key_required is
--     always(state = powerup or state = powerdown -> state /= activated until aes_key = '1');

-- PSL property maintain_open_light is
--     always(state = down -> (open_light = '1' -> command = "10" and bad_encryption = '0' and ACSC = '0') until state = up);

-- PSL property bad_encryption_management is
--     always({ state = up or state = down ; (command = "01" or command = "10") and bad_encryption = '0' ; (bad_encryption = '0' and ACSC = '0')[*] ; bad_encryption = '1' } |-> state = securityerror before! state = deactivated);

-- PSL assert key_required;
-- PSL assert maintain_open_light;
-- PSL assert bad_encryption_management;

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
		if switch = '1' and (state = powerup or state = nokey) and aes_key = '1' then key_led <= '1';
		end if;
		if switch = '1' and state = deactivated and authentication = '1' then authentication_led <= '1';
		end if;
		if switch = '1' and ((state = down and command = "10")
			or (state = movingondown and command = "10")
			or (state = safetyerror and acsc = '0' and command = "10")
			or (state = movingonup and bad_encryption = '0' and acsc = '0' and command /= "01" and reached_top = '0'))
				then open_light <= '1';
		end if;
		if switch = '1' and ((state = up and command = "01")
			or (state = movingonup and command = "01")
			or (state = safetyerror and acsc = '0' and command = "01")
			or (state = movingondown and bad_encryption = '0' and acsc = '0' and command /= "10" and reached_bottom = '0'))
				then close_light <= '1';
		end if;
	end process;

	process (clk, reset)
	begin
		if reset = '1' then state <= powerup;
		elsif clk'event and clk = '1' then state <= nextstate;
		end if;
	end process;

end Automaton;

