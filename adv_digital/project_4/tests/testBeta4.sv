/***************************************
 * Testbench for Beta: Lab 4
 *
 * Elizabeth Basha
 * Spring 2015
 */
 
 module testBeta4();
 
		// Define parameters when calling from do file
        parameter testFileName;
        parameter numTests;
		parameter CYCLE_TIME=32'd10;
        
        // Signal declarations
		logic clk = 1'b0;
        logic reset = 1'b1;
		logic irq = 1'b0;
		logic [31:0] id, memReadData, ia, memAddr, memWriteData;
		logic MemWrite, MemRead;
           
        logic [3:0] cntlIn;
		logic [31:0] iaExpected, memAddrExpected;
                
		logic MemWriteExpected, MemReadExpected;
		logic [67:0] testVector[800:0];
        int i = 32'd0;
        
        // Module under test declaration
        beta dutBeta(.clk(clk),.reset(reset),.irq(irq),.id(id),.memReadData(memReadData),.ia(ia),.memAddr(memAddr),.memWriteData(memWriteData),.MemWrite(MemWrite),.MemRead(MemRead));
        
		// Create instruction memory
		imem4 dutImem(.clk(clk),.ia(ia),.id(id));
		
		// Create data memory
		dmem4 dutDmem(.clk(clk),.memAddr(memAddr),.memWriteData(memWriteData),.memReadData(memReadData),.MemWrite(MemWrite),.MemRead(MemRead));
		
		// Generate clock signal
		always #(CYCLE_TIME) clk = ~clk;
		
        // Test
        initial
        begin         
           // Read test file
           $readmemh(testFileName, testVector);
        end
        
		always @(posedge clk)
		begin
			// Assign signals and check for results
			if(i<numTests)
			begin
				{cntlIn, iaExpected, memAddrExpected} = testVector[i];

				// Set signals
				irq = cntlIn[3];
				reset = cntlIn[2];
				MemReadExpected = cntlIn[1];
				MemWriteExpected = cntlIn[0];
				
				// Wait until almost end of cycle
				#(2*CYCLE_TIME-1)
				
				// Check result
				if((ia!==iaExpected) || (memAddr!==memAddrExpected) || (MemWrite!==MemWriteExpected) || (MemRead!==MemReadExpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected ia = %h\t memAddr = %h\n",iaExpected, memAddrExpected);
					$display("Expected MemWrite = %h\t MemRead = %h\n",MemWriteExpected, MemReadExpected);
					$stop;
				end
				
				// Increment i
				i=i+32'd1;
			end else begin
				// If all done, exit cleanly
				$display("Test Successful!\n");
				$stop;
			end
		end
		
 endmodule