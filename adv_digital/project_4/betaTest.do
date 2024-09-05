# Script to run testbench

# Compile Beta
vlog -reportprogress 300 -work work bool.sv
vlog -reportprogress 300 -work work arith.sv
vlog -reportprogress 300 -work work comp.sv
vlog -reportprogress 300 -work work shift.sv
vlog -reportprogress 300 -work work alu.sv
vlog -reportprogress 300 -work work pc.sv
vlog -reportprogress 300 -work work -suppress 7061 regfile.sv
vlog -reportprogress 300 -work work ctl.sv
vlog -reportprogress 300 -work work beta.sv
	
### ADD YOUR DESIGN FILES HERE FOR COMPILATION ###
vlog -reportprogress 300 -work work flowctl.sv

# Compile Testbench
vlog -sv -reportprogress 300 -work work tests/imem4.sv
vlog -sv -reportprogress 300 -work work tests/dmem4.sv
vlog -sv -reportprogress 300 -work work tests/testBeta4.sv

# Simulate
vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/lab4test.txt" -gnumTests=97 testBeta4

do tests/opRadix.txt
do tests/funcRadix.txt
do tests/regRadix.txt

# Add waves
add wave -label Clk clk
add wave -label Reset reset
add wave -label IRQ irq
add wave -radix hex -label IA ia
add wave -radix hex -label ID id
add wave -radix OP_LABELS -label OpCode {id[31:26]}
add wave -radix FUNC_LABELS -label Funct {id[5:0]}
add wave -radix REG_LABELS -label Rs {id[25:21]}
add wave -radix REG_LABELS -label Rt {id[20:16]}
add wave -radix REG_LABELS -label Rd {id[15:11]}
add wave -radix hex -label MemAddr memAddr
add wave -radix hex -label MemReadData memReadData
add wave -radix hex -label MemWriteData memWriteData
add wave -label MemWrite MemWrite
add wave -label MemRead MemRead

#### Add your debug signals here ####


# Plot signal values
view structure
view signals
run 10 ns