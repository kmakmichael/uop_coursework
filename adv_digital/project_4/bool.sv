module bool(
    input logic [3:0] op,
    input logic [31:0] a,
    input logic [31:0] b,
    output logic [31:0] y
);

    always_comb begin
        case (op)
            4'b1010:    y <= a;           // A
            4'b1000:    y <= a & b;       // AND
            4'b0001:    y <= ~(a | b);    // NOR
            4'b1110:    y <= a | b;       // OR
            4'b1001:    y <= ~(a ^ b);    // XNOR
            4'b0110:    y <= a ^ b;       // XOR
            default:    y <= a;           // A 
        endcase
    end

endmodule
