/***************************************
 * Testbench for ALU
 *
 * Elizabeth Basha
 * Spring 2014
 */
 
 module testALU();
 
		// Define parameters when calling from do file
        parameter testFileName;
        parameter numTests;
        
        // Signal declarations
        logic [31:0] A, B, Y;
        logic [4:0] ALUOp;
        logic [7:0] aluOpIn;
        logic z, v, n;
        logic [3:0] zvnIn;
        
        logic [31:0] Yexpected;
        logic zexpected,vexpected,nexpected;
        logic [108:0] testVector[800:0];
        logic [31:0] errors;
        int i;
        
        // Module under test declaration
        alu dutAlu(.A(A),.B(B),.ALUOp(ALUOp),.Y(Y),.z(z),.v(v),.n(n));
        
        // Test
        initial
        begin         
          // Read memory
           $readmemh(testFileName, testVector);
          
          // Assign signals and check for results
          for(i=0; i<numTests; i++)
          begin
            {aluOpIn, A, B, Yexpected, zvnIn} = testVector[i];
            ALUOp = aluOpIn[4:0];
            {zexpected, nexpected, vexpected} = zvnIn[2:0];
            #10
			// Boolean and Shift don't really create z,v,n but full generates anyway; need to ignore
            if((ALUOp >= 5'b10000) || (ALUOp == 5'b01000) || (ALUOp == 5'b01001) || (ALUOp == 5'b01011))
			begin
				if((Y!==Yexpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected y = %h\t",Yexpected);
					$stop;
				end
			end else begin
				if((Y!==Yexpected)||(z!==zexpected)||(v!==vexpected)||(n!==nexpected))
				begin
					$display("Error at simulation time = %0t\n",$time);
					$display("Expected y = %h\tExpected z = %h\tExpected v = %h\tExpected n = %h\t",Yexpected,zexpected,vexpected,nexpected);
					$stop;
				end
			end
          end        
          
          #10
          $display("Test Successful!\n");
          $stop;
        end
        
 endmodule