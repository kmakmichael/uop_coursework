module pc(
    input logic clk,
    input logic reset,
    output logic [31:0] ia   
);
    always_ff @(posedge clk or posedge reset) begin
        if (reset) begin
            ia <= 32'd0;
        end else begin
            ia <= ia + 32'd4;
        end
    end
endmodule