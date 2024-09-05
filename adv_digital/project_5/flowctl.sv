module flowctl(
    input logic [31:0] pcp4, id, radata,
    input logic [1:0] Jump,
    input logic Branch, z,
    output logic [31:0] pcnext
);
    // signals
    logic [31:0] brAddr;
    logic [31:0] jAddr;

    assign jAddr = {pcp4[31:28], id[25:0], 2'b00};

    // jump mux
    always_comb begin
        case (Jump)
            2'b00,  // none
            2'b01:  // branch
                pcnext <= brAddr;
            2'b10:  // j, jal
                pcnext <= jAddr;
            2'b11:  // jr
                pcnext <= radata;
        endcase
    end

    // branch ctl
    always_comb begin
        if (Branch) begin
            if (id[26] != z) begin
                if (id[15]) //sign-extend
                    brAddr <= pcp4 + {15'h7FFF, id[14:0], 2'b00};
                else
                    brAddr <= pcp4 + {15'd0, id[14:0], 2'b00};
            end else begin
                brAddr <= pcp4;
            end
        end else begin
            brAddr <= pcp4;
        end
    end
endmodule