/***************************************
 * Testbench for Beta: Reg File
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module testReg();
 
		// Define parameters when calling from do file
        parameter testFileName;
        parameter numTests;
		parameter CYCLE_TIME=32'd10;
        
        // Signal declarations
        logic clk = 1'b0;
		logic RegWrite = 1'b0;
		logic RegDst = 1'b0;
		logic [4:0] ra, rb, rc;
        logic [31:0] wdata;
		logic [31:0] radata, rbdata;
           
        logic [3:0] cntlIn;
		logic [7:0] raIn, rbIn, rcIn;
                
        logic [31:0] raDataExpected, rbDataExpected;
        logic [123:0] testVector[800:0];
        int i = 32'd0;
        
        // Module under test declaration
        regfile dutReg(.clk(clk),.RegWrite(RegWrite),.RegDst(RegDst),.ra(ra),.rb(rb),.rc(rc),.wdata(wdata),.radata(radata),.rbdata(rbdata));
        
		// Generate clock signal
		always #(CYCLE_TIME) clk = ~clk;
		
        // Test
        initial
        begin         
          // Read memory
           $readmemh(testFileName, testVector);
        end
        
		always @(posedge clk)
		begin
			// Assign signals and check for results
			if(i<numTests)
			begin
				{cntlIn, raIn, rbIn, rcIn, wdata, raDataExpected, rbDataExpected} = testVector[i];

				// Set signals
				RegWrite = cntlIn[1];
				RegDst = cntlIn[0];
				ra = raIn[4:0];
				rb = rbIn[4:0];
				rc = rcIn[4:0];
			
				// Wait until almost end of cycle
				#(2*CYCLE_TIME-1)
				
				// Check result
				if((radata!==raDataExpected) || (rbdata!==rbDataExpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected radata = %h\t rbdata = %h",raDataExpected, rbDataExpected);
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