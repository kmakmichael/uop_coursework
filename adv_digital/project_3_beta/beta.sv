module beta(
    input logic clk,
    input logic reset,
    input logic [31:0] id,
    input logic [31:0] memReadData,
    output logic [31:0] ia,
    output logic [31:0] memAddr,
    output logic [31:0] memWriteData,
    output logic MemRead,
    output logic MemWrite
);
    // signals
    logic RegDst, ALUSrc, RegWrite, MemToReg, z, v, n;
    logic [4:0] ALUOp;
    logic [31:0] A, B, Y, rbdata, wdata, imm;
    logic [5:0] rc;

    // modules
    alu xalu(A, B, ALUOp, Y, z, v, n);
    ctl xctl(reset, id[31:26], id[5:0], RegDst, ALUSrc, RegWrite, MemWrite, MemRead, MemToReg, ALUOp);
    pc xpc(clk, reset, ia);
    regfile xregfile(clk, RegWrite, RegDst, id[25:21], id[20:16], id[15:11], wdata, A, rbdata);

    // assign
    assign memWriteData = rbdata;
    assign memAddr = Y;

    // B
    always_comb begin
        if (ALUSrc)
            B <= imm;
        else
           B <= rbdata;
    end

    // wdata
    always_comb begin
        if (MemToReg)
            wdata <= memReadData;
        else
            wdata <= Y;
    end

    // pad or extend
    always_comb begin
        case(id[31:26])
            6'b000000:
                case (id[5:0])
                    6'b000000,
                    6'b000010,
                    6'b000011:
                        imm <= {26'd0, id[11:6]};
                    default:
                        imm <= 32'd0;
                endcase
            6'b001000: // addi
                if (id[15])
                    imm <= {17'h1FFFF, id[14:0]};
                else
                    imm <= {17'd0, id[14:0]};
             6'b001100, // andi
            6'b001101, // ori
            6'b001110, // xori
            6'b100011,  // lw
            6'b101011:  // sw
                imm <= {16'd0, id[15:0]};
            default:
                imm <= 32'd0;
        endcase
    end
endmodule