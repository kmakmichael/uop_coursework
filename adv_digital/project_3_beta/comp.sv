module comp(
    input logic o3,
    input logic o1,
    input logic z,
    input logic v,
    input logic n,
    output logic [31:0] y
);

    always_comb begin
        case({o3, o1})
            2'b00:  // CMPEQ
                y <= {31'd0, z};
            2'b01:  // CMPLT
                y <= {31'd0, (v ^ n) & ~z};
            2'b10:  // CMPLE
                y <= {31'd0, (v ^ n) | z};  
            default:// 2b11 not implemented
                y <= {32'd0};
        endcase
    end

endmodule