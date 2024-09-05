# Script to run testbench

set pc "PC"
set reg "REG"
set ctl "CTL"
set lab3 "LAB3"
set condSigP [string compare $1 $pc]
set condSigR [string compare $1 $reg]
set condSigC [string compare $1 $ctl]
set condSigL [string compare $1 $lab3]

if "$condSigP==0" {
    # Compile Design
    vlog -reportprogress 300 -work work pc.sv
        
    # Compile Testbench
    vlog -sv -reportprogress 300 -work work tests/testPc.sv

    # Simulate
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/pc.txt" -gnumTests=100 testPc

    # Add waves
    add wave clk
    add wave reset
    add wave -radix hex ia

} elseif "$condSigR==0" {
	# Compile Design
    vlog -reportprogress 300 -work work -suppress 7061 regfile.sv
        
    # Compile Testbench
    vlog -sv -reportprogress 300 -work work tests/testReg.sv

    # Simulate
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/reg.txt" -gnumTests=55 testReg

   # Add waves
    add wave clk
    add wave RegWrite
	add wave RegDst
	add wave -radix hex ra
	add wave -radix hex rb
	add wave -radix hex rc
	add wave -radix hex wdata
	add wave -radix hex radata
	add wave -radix hex rbdata
	
} elseif "$condSigC==0" {
	# Compile Design
    vlog -reportprogress 300 -work work ctl.sv
        
    # Compile Testbench
    vlog -sv -reportprogress 300 -work work tests/testCtl.sv

    # Simulate
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/ctl.txt" -gnumTests=129 testCtl

	do tests/opRadix.txt
	do tests/funcRadix.txt

    # Add waves
    add wave reset
	add wave -radix OP_LABELS opCode
	add wave -radix FUNC_LABELS funct
    add wave RegDst
	add wave ALUSrc
	add wave RegWrite
	add wave MemWrite
	add wave MemRead
	add wave MemToReg
	add wave -radix hex ALUOp
	
} elseif "$condSigL==0" {
	# Compile Design
    vlog -reportprogress 300 -work work bool.sv
	vlog -reportprogress 300 -work work arith.sv
	vlog -reportprogress 300 -work work comp.sv
	vlog -reportprogress 300 -work work shift.sv
	vlog -reportprogress 300 -work work alu.sv
	vlog -reportprogress 300 -work work pc.sv
	vlog -reportprogress 300 -work work -suppress 7061 regfile.sv
	vlog -reportprogress 300 -work work ctl.sv
	vlog -reportprogress 300 -work work beta.sv
        
    # Compile Testbench
	vlog -sv -reportprogress 300 -work work tests/imem.sv
	vlog -sv -reportprogress 300 -work work tests/dmem.sv
    vlog -sv -reportprogress 300 -work work tests/testBeta.sv

    # Simulate
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/basicCode.txt" -gnumTests=172 testBeta

	do tests/opRadix.txt
	do tests/funcRadix.txt
	do tests/regRadix.txt
	
    # Add waves
	add wave -label Clk clk
    add wave -label Reset reset
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
}

#### Add your debug signals here ####
add wave -radix hex dutBeta/A
add wave -radix hex dutBeta/B
add wave -radix hex dutBeta/Y


# Plot signal values
view structure
view signals
run 10 ns