module arith(
    input logic [1:0] op,
    input logic [31:0] a,
    input logic [31:0] b,
    output logic [31:0] y,
    output logic z,
    output logic v,
    output logic n
);

    // sign
    assign n = y[31];

    // add/sub
    always_comb begin
        if (op[0]) begin
            y <= a + ~b + 1'b1;
        end else begin
            y <= a + b;
        end
    end

    // overflow
    always_comb begin
        if (op[0]) begin
            v <= (a[31] ^ b[31]) & (y[31] ^ a[31]);
        end else begin
            v <= ~(a[31] ^ b[31]) & (y[31] ^ a[31]);
        end
    end

    // zero
    always_comb begin
        if (y == 32'd0) begin
            z <= 1'b1;
        end else begin
            z <= 1'b0;
        end
    end

endmodule
