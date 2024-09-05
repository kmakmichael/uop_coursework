module regfile(
    input logic clk,
    input logic RegWrite,
    input logic [1:0] RegDst,
    input logic [4:0] ra,   // rs
    input logic [4:0] rb,   // rt
    input logic [4:0] rc,   // rd
    input logic [31:0] wdata, // write data
    output logic [31:0] radata, // read data 1
    output logic [31:0] rbdata  // read data 2
);

    logic [31:0] memory [31:0];
    // init for modelsim
    initial begin
        for (int i = 0; i < 32; i++)
            memory[i] = 32'd0;
    end

    assign radata = memory[ra];
    assign rbdata = memory[rb];

    always_ff @(posedge clk) begin
        if (RegWrite) begin
            case (RegDst)
                2'b00:  // rc
                    if (rc != 5'd0) begin
                        memory[rc] <= wdata; // check for == 1, 31?
                    end
                2'b01:  // rb
                    if (rb != 5'd0) begin
                        memory[rb] <= wdata;
                    end
                2'b10:  // ra
                    memory[5'd31] <= wdata;
                2'b11:  // xp
                    memory[5'd1] <= wdata;
                default:  
                    if (rc != 5'd0) begin
                        memory[rc] <= wdata;
                    end
            endcase
        end
    end
    
endmodule