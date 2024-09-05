module ctl(
    input logic reset,
    input logic [5:0] opCode,
    input logic [5:0] funct,
    output logic RegDst,
    output logic ALUSrc,
    output logic RegWrite,
    output logic MemWrite,
    output logic MemRead,
    output logic MemToReg,
    output logic [4:0] ALUOp
);

    // RegDst
    always_comb begin
        case (opCode)
            6'b001000, // addi
            6'b001100, // adi
            6'b001101, // ori
            6'b001110, // xori
            6'b100011: // lw
                RegDst <= 1'b1 & ~reset;
            default:
                RegDst <= 1'b0 & ~reset;
        endcase
    end
    
    // ALUSrc
    always_comb begin
        case (opCode)
            6'b001000, // addi
            6'b001100, // adi
            6'b001101, // ori
            6'b001110, // xori
            6'b100011, // lw
            6'b101011: // sw
                ALUSrc <= 1'b1 & ~reset;
            6'b000000:
                case (funct)
                    6'b000000, // sll
                    6'b000010, // srl
                    6'b000011: // sra
                        ALUSrc <= 1'b1 & ~reset;
                    default:
                        ALUSrc <= 1'b0;
                endcase
            default:
                ALUSrc <= 1'b0;
        endcase
    end

    // RegWrite
    always_comb begin
        case (opCode)
            6'b001000, // addi
            6'b001100, // andi
            6'b001101, // ori
            6'b001110, // xori
            6'b100011: // lw
                RegWrite <= 1'b1 & ~reset;
            6'b000000:
                case (funct)
                    6'b100000, // add
                    6'b100010, // sub
                    6'b100100, // and
                    6'b100101, // or
                    6'b100110, // xor
                    6'b100111, // nor
                    6'b101010, // slt
                    6'b000000, // sll
                    6'b000010, // srl
                    6'b000011: // sra
                        RegWrite <= 1'b1 & ~reset;
                    default:
                        RegWrite <= 1'b0;
                endcase
            default:
                RegWrite <= 1'b0;
        endcase
    end

    // MemWrite
    always_comb begin
        if (opCode == 6'b101011) // sw
            MemWrite <= 1'b1 & ~reset;
        else
            MemWrite <= 1'b0;
    end

    // MemRead
    always_comb begin
        if (opCode == 6'b100011) // lw
            MemRead <= 1'b1 & ~reset;
        else
            MemRead <= 1'b0;
    end

    // MemToReg
    always_comb begin
        case (opCode)
            6'b100011: // lw
                MemToReg <= 1'b1 & ~reset;
            default:
                MemToReg <= 1'b0;
        endcase
    end

    // ALUOp
    always_comb begin
        if (reset) begin
            ALUOp <= 6'd0;
        end else begin
            case (opCode)
                6'd0:
                    case (funct[5:3])
                    3'b000:
                        case (funct[2:0])
                            3'b000:     ALUOp <= 5'b01000; // sll
                            3'b010:     ALUOp <= 5'b01001; // srl
                            3'b011:     ALUOp <= 5'b01011; // sra
                            default:    ALUOp <= 5'b00000; // nop
                        endcase
                    3'b100:
                        case (funct[2:0])
                            3'b000:     ALUOp <= 5'b00000; // add
                            3'b010:     ALUOp <= 5'b00001; // sub
                            3'b100:     ALUOp <= 5'b11000; // and
                            3'b101:     ALUOp <= 5'b11110; // or
                            3'b110:     ALUOp <= 5'b10110; // xor
                            3'b111:     ALUOp <= 5'b10001; // nor
                            default:    ALUOp <= 5'b00000; // nop
                        endcase
                    3'b101:
                        case (funct[2:0])
                            3'b010:     ALUOp <= 5'b00111; // slt
                            default:    ALUOp <= 5'b00000; // nop
                        endcase
                    default:
                        ALUOp <= 5'b00000; // nop
                    endcase
                6'b001000:  ALUOp <= 5'b00000;  // addi
                6'b001100:  ALUOp <= 5'b11000;  // andi
                6'b001101:  ALUOp <= 5'b11110;  // ori
                6'b001110:  ALUOp <= 5'b10110;  // xori
                6'b100011:  ALUOp <= 5'b00000;  // lw
                6'b101011:  ALUOp <= 5'b00000;  // sw
                default:    ALUOp <= 5'b00000;  // nop
            endcase
        end
    end

endmodule