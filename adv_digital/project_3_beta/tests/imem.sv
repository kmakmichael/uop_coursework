/***************************************
 * I-Mem for Beta
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module imem(input logic clk,
			 input logic [31:0] ia,
			 output logic [31:0] id);
			 
	logic [31:0] instrMemory [255:0];
	logic [31:0] iaWord;	// Need to use word aligned version of address
	
	initial
	begin
		$readmemh("tests/mipsCode.txt", instrMemory);
	end
			
	assign iaWord = ia>>2;
	assign id = instrMemory[iaWord];
	
endmodule