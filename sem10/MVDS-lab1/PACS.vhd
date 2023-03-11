-- Parking Access Control System

entity Parking is
     port(clk, reset, require_in, avail, timeout_1, timeout_2, sensor_in, insert_ticket, ticketOK, sensor_out: in Bit;     
          gate_in_open, gate_out_open, car_in, ticket_inserted, car_out: out Bit);  
end Parking;

architecture Automaton of Parking is
type States is (idle, verif_ticket, open_out, close_out, open_in, close_in);
signal state, nextstate: States;
begin
    -- Next state state assignment process
    process(state, require_in, avail, timeout_1, timeout_2, sensor_in, insert_ticket, ticketOK, sensor_out)
    begin
        case state is
            when idle =>
                if insert_ticket = '1' then nextstate <= verif_ticket;
                elsif require_in = '1' and avail = '1' and not insert_ticket = '1' then nextstate <= open_in;
                else nextstate <= idle;
                end if;
            when verif_ticket =>
                if ticketOK = '1' then nextstate <= open_out;
                else nextstate <= idle;
                end if;
            when open_out =>
                if timeout_2 = '1' then nextstate <= close_out;
                elsif not timeout_2 = '1' and sensor_out = '1' then nextstate <= close_out;
                else nextstate <= open_out;
                end if;
            when close_out =>
                nextstate <= idle;
            when open_in =>
                if timeout_1 = '1' then nextstate <= close_in;
                elsif not timeout_1 = '1' and sensor_in = '1' then nextstate <= close_in;
                else nextstate <= open_in;
                end if;
            when close_in =>
                nextstate <= idle;
        end case;
    end process;
    -- Output assignment process
    process(state, require_in, avail, timeout_1, timeout_2, sensor_in, insert_ticket, ticketOK, sensor_out)
    begin
        gate_in_open <= '0';
        gate_out_open <= '0';
        car_in <= '0';
        ticket_inserted <= '0';
        car_out <= '0';
        if state = open_in and not timeout_1 = '1' then
            if sensor_in = '1' then car_in <= '1';
            else gate_in_open <= '1';
            end if;
        end if;
        if state = open_out and not timeout_2 = '1' then
            if sensor_out = '1' then car_out <= '1';
            else gate_out_open <= '1';
            end if;
        end if;
        if state = idle and insert_ticket = '1' then ticket_inserted <= '1';
        end if;
    end process;
    -- State switching process
    process(clk)
    begin
        if clk'event and clk = '1' then
            if reset = '1' then state <= idle;
            else state <= nextstate;
            end if;
        end if;
    end process;
end Automaton;
