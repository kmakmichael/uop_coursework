/***************************************
 * Testbench for Beta: Ctl
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module testCtl();
 
		// Define parameters when calling from do file
        parameter testFileName;
        parameter numTests;
		parameter CYCLE_TIME=32'd10;
        
        // Signal declarations
		logic clk = 1'b0;
        logic reset = 1'b1;
		logic [5:0] opCode, funct;
		logic RegDst, ALUSrc, RegWrite, MemWrite, MemRead, MemToReg;
		logic [4:0] ALUOp;
           
        logic [7:0] cntlIn, opIn, funcIn, aluOpIn;
                
		logic RegDstExpected, ALUSrcExpected, RegWriteExpected, MemWriteExpected, MemReadExpected, MemToRegExpected;
		logic [4:0] ALUOpExpected;
        logic [31:0] testVector[800:0];
        int i = 32'd0;
        
        // Module under test declaration
        ctl dutCtl(.reset(reset),.opCode(opCode),.funct(funct),.RegDst(RegDst),.ALUSrc(ALUSrc),.RegWrite(RegWrite),.MemWrite(MemWrite),.MemRead(MemRead),.MemToReg(MemToReg),.ALUOp(ALUOp));
        
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
				{opIn, funcIn, cntlIn, aluOpIn} = testVector[i];

				// Set signals
				opCode = opIn[5:0];
				funct = funcIn[5:0];
				reset = cntlIn[6];
				RegDstExpected = cntlIn[5];
				ALUSrcExpected = cntlIn[4];
				RegWriteExpected = cntlIn[3];
				MemWriteExpected = cntlIn[2];
				MemReadExpected = cntlIn[1];
				MemToRegExpected = cntlIn[0];
				ALUOpExpected = aluOpIn[4:0];
			
				// Wait until almost end of cycle
				#(2*CYCLE_TIME-1)
				
				// Check result
				if((RegDst!==RegDstExpected) || (ALUSrc!==ALUSrcExpected) || (RegWrite!==RegWriteExpected) || (MemWrite!==MemWriteExpected) || (MemRead!==MemReadExpected) || (MemToReg!==MemToRegExpected) || (ALUOp!==ALUOpExpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected RegDst = %h\t ALUSrc = %h\t RegWrite = %h\t MemWrite = %h\n",RegDstExpected, ALUSrcExpected, RegWriteExpected, MemWriteExpected);
					$display("Expected MemRead = %h\t MemToReg = %h\t ALUOp = %h\n",MemReadExpected, MemToRegExpected, ALUOpExpected);
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