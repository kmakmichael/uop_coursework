/***************************************
 * D-Mem for Beta
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module dmem(input logic clk,
			 input logic [31:0] memAddr,
			 input logic [31:0] memWriteData,
			 input logic MemWrite, MemRead,
			 output logic [31:0] memReadData);
			 
	logic [31:0] dataMemory[1023:0];
	logic [31:0] memAddrWord;	// need to use word aligned version
	
	initial
	begin
		$readmemb("tests/dataMem.txt", dataMemory);
	end
	
	assign memAddrWord = memAddr>>2;
	
	always_ff @(posedge clk)
	begin
		if(MemWrite)
			dataMemory[memAddrWord] <= memWriteData;
	end
	
	assign memReadData = MemRead ? dataMemory[memAddrWord] : 32'd0;
	
endmodule