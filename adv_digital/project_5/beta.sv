module beta(
    input logic clk, reset, irq, MemReadReady, MemWriteDone,
    input logic [31:0] id,
    input logic [31:0] memReadData,
    output logic [31:0] ia,
    output logic [31:0] memAddr,
    output logic [31:0] memWriteData,
    output logic MemRead, MemWrite, MemReadDone, MemHit, MemWriteReady
);
    // signals
    logic RegWrite, MemToReg, z, v, n, ASel, Exception, Branch, pc31, irqpc, CacheRead, stall;//, MemWriteDone, MemWriteReady;
    logic [1:0] RegDst, ALUSrc, Jump;
    logic [4:0] ALUOp;
    logic [31:0] A, B, radata, rbdata, wdata, pcnext, pcp4, cAddr, cData, cOut;
    logic [5:0] rc;

    // modules
    alu xalu(A, B, ALUOp, memAddr, z, v, n);
    ctl xctl(reset, id[31:26], id[5:0], pc31, irq, RegDst, ALUSrc, RegWrite, MemWrite, MemRead, MemToReg, ASel, Branch, Jump, Exception, ALUOp);
    pc xpc(clk, reset, irq, Exception, stall, pcnext, ia);
    regfile xregfile(clk, RegWrite, RegDst, id[25:21], id[20:16], id[15:11], wdata, radata, rbdata);
    flowctl xflowctl(pcp4, id, radata, Jump, Branch, z, pcnext);
    cache xcache(clk, memAddr, CacheRead, CacheWrite, cData, MemHit, cOut);
    cachectl xcachectl(clk, MemRead, MemReadReady, MemWrite, MemWriteDone, MemHit, MemReadDone, CacheRead, MemWriteReady, CacheWrite, stall);

    // assign
    assign memWriteData = MemWrite ? cOut : rbdata;
    assign pcp4 = ia + 32'd4;

    // A
    always_comb begin
        if (ASel)
            A <= pcp4;
        else 
            A <= radata;
    end

    // B
    always_comb begin
        case (ALUSrc)
            2'b00:  // register
                B <= rbdata;
            2'b01:  // shifts
                B <= {26'd0, id[11:6]};
            2'b10:  // I-format
                B <= {16'd0, id[15:0]};
            2'b11:  // addi
                if (id[15]) begin
                    B <= {17'h1FFFF, id[14:0]};
                end else begin
                    B <= {17'd0, id[14:0]};
                end
            default:
                B <= rbdata;
        endcase
    end

    // wdata
    always_comb begin
        if (MemToReg)
            wdata <= memReadData;
        else
            wdata <= memAddr;
    end

    // supervisor bit
    always_comb begin
        if (pc31) begin
            if (Jump == 2'b11) begin // performing JR
                pc31 <= pcnext[31];
            end else begin
                pc31 <= pcp4[31];
            end
        end else begin
            pc31 <= pcp4[31];
        end
    end

    // write from mem to cache on read miss
    always_comb begin
        if (MemRead)
            cData <= memReadData;
        else
            cData <= rbdata;
    end

endmodule