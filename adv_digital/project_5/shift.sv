module shift(
    input logic [1:0] op,
    input logic [31:0] a,
    input logic [31:0] b,
    output logic [31:0] y
);

    always_comb begin
        case (op)
            2'b00:  // SLL
                y <= a << b[4:0];
            2'b01:  // SRL
                y <= a >> b[4:0];
            2'b11:  // SR w/ sign extend
                y <= signed' (a) >>> b[4:0];
            default:
                y <= a; 
        endcase
    end

endmodule