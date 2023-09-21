Clear outputs: `rm -rf work/`

Launch simulation: `vsim`

Compile project: `vcom cryptogarage.vhd`

Compile testbench: `vcom cryptogarage_test.vhd`

All: `rm -rf work/ && vcom -cover bfs cryptogarage.vhd && vcom cryptogarage_test.vhd && vsim -coverage -assertdebug`

