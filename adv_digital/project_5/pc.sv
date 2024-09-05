module pc(
    input logic clk,
    input logic reset,
    input logic irq,
    input logic Exception,
    input logic stall,
    input logic [31:0] pcin,
    output logic [31:0] ia
);
    always_ff @(posedge clk or posedge reset) begin
        if (stall) begin
            ia <= ia;
        end else begin
            if (reset)
                ia <= 32'h8000_0000;
            else if (irq && ~ia[31])
                ia <= 32'h8000_0008;
            else if (Exception)
                ia <= 32'h8000_0004;
            else
                ia <= pcin;
        end
    end
endmodule