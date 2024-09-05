/***************************************
 * I-Mem for Beta (Lab 4)
 *
 * Elizabeth Basha
 * Spring 2015
 */
 
 module imem4(input logic clk,
			 input logic [31:0] ia,
			 output logic [31:0] id);
			 
	logic [31:0] instrMemory [255:0];
	logic [31:0] iaWord;	// Need to use word aligned version of address
	
	initial
	begin
		$readmemh("tests/lab4imem.txt", instrMemory);
	end
			
	assign iaWord = (ia & 32'h7fffffff)>>2;	// Now also need to ignore supervisor bit
	assign id = instrMemory[iaWord];
	
endmodule