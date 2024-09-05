/***************************************
 * Testbench for Beta: PC
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module testPc();
 
		// Define parameters when calling from do file
        parameter testFileName;
        parameter numTests;
		parameter CYCLE_TIME=32'd10;
        
        // Signal declarations
        logic clk = 1'b0;
		logic reset = 1'b1;
        logic [31:0] ia;
           
        logic [3:0] resetIn;
               
        logic [31:0] IAexpected;
        logic [36:0] testVector[800:0];
        logic [31:0] errors;
        int i = 32'd0;
        
        // Module under test declaration
        pc dutPc(.clk(clk),.reset(reset),.ia(ia));
        
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
				{resetIn, IAexpected} = testVector[i];
			
				// Set signals
				reset = resetIn[0];
			
				// Wait until almost end of cycle
				#(2*CYCLE_TIME-1)
				
				// Check result
				if((ia!==IAexpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected ia = %h\t",IAexpected);
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