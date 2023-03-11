-- Testbench for Parking Access Control System

entity testPAC is
end testPAC;

architecture test of testPAC is
    component Parking is
        port(clk, reset, require_in, avail, timeout_1, timeout_2, sensor_in, insert_ticket, ticketOK, sensor_out: in Bit;
             gate_in_open, gate_out_open, car_in, ticket_inserted, car_out: out Bit);  
    end component;
    signal clk, reset, rin, avail, t1, t2, sin, inti, tiOK, sout, gio, goo, ci, ti, co: Bit;
begin
    -- Clock process
    process
    begin
        clk <= '0';
        wait for 10 ns;
        clk <= '1';
        wait for 10 ns;
    end process;
    -- PAC port mappings
    PAC: Parking port map(clk=>clk, reset=>reset, require_in=>rin, avail=>avail, timeout_1=>t1, timeout_2=>t2, sensor_in=>sin, insert_ticket=>inti, ticketOK=>tiOK, sensor_out=>sout, gate_in_open=>gio, gate_out_open=>goo, car_in=>ci, ticket_inserted=>ti, car_out=>co);
    -- Initial setup
    reset <= '0';
    avail <= '1';
    tiOK <= '1';
    -- Car leaves test (0 - 70 ns)
    rin <= '1', '0' after 25 ns;
    t1 <= '0', '1' after 70 ns;
    sin <= '0', '1' after 30 ns, '0' after 50 ns;
    -- Car arrives test (80 - 150 ns)
    inti <= '0', '1' after 80 ns, '0' after 100 ns;
    t2 <= '1', '0' after 80 ns, '1' after 150 ns;
    sout <= '0', '1' after 110 ns, '0' after 130 ns;
end test;

configuration config of work.testPAC is 
    for test
        for PAC: Parking use entity work.Parking(Automaton);
        end for;
    end for; 
end config; 
