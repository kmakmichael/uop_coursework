# Script to run testbench

# Compile Design
vlog -reportprogress 300 -work work alu.sv
        
# Compile Testbench
vlog -sv -reportprogress 300 -work work tests/testALU.sv

do tests/aluRadix

# Simulate
set first "FIRST"
set bool "BOOL"
set arith "ARITH"
set comp "COMP"
set shift "SHIFT"
set full "FULL"
set condSigF [string compare $1 $first]
set condSigB [string compare $1 $bool]
set condSigA [string compare $1 $arith]
set condSigC [string compare $1 $comp]
set condSigS [string compare $1 $shift]
set condSigD [string compare $1 $full]
if "$condSigF==0" {
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/first.txt" -gnumTests=4 testALU
} elseif "$condSigB==0" {
    vlog -reportprogress 300 -work work bool.sv
    vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/bool.txt" -gnumTests=180 testALU
} elseif "$condSigA==0" {
	vlog -reportprogress 300 -work work arith.sv
	vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/arith.txt" -gnumTests=44 testALU
} elseif "$condSigC==0" {
	vlog -reportprogress 300 -work work arith.sv
	vlog -reportprogress 300 -work work comp.sv
	vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/comp.txt" -gnumTests=42 testALU
} elseif "$condSigS==0" {
	vlog -reportprogress 300 -work work shift.sv
	vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/shift.txt" -gnumTests=481 testALU
} elseif "$condSigD==0" {
	vlog -reportprogress 300 -work work bool.sv
	vlog -reportprogress 300 -work work arith.sv
	vlog -reportprogress 300 -work work comp.sv
	vlog -reportprogress 300 -work work shift.sv
	vsim -t 1ps -L work -voptargs="+acc" -gtestFileName="tests/full.txt" -gnumTests=742 testALU
}

# Run simulation and plot
add wave -radix hex A
add wave -radix hex B
add wave -radix ALU_LABELS ALUOp
add wave -radix hex Y
add wave z
add wave v
add wave n

# Add your debug signals here

# Plot signal values
view structure
view signals
run 10 ns