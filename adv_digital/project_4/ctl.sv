module ctl(
    input logic reset,
    input logic [5:0] opCode,
    input logic [5:0] funct,
    input logic pc31,
    input logic irq,
    output logic [1:0] RegDst,
    output logic [1:0] ALUSrc,
    output logic RegWrite,
    output logic MemWrite,
    output logic MemRead,
    output logic MemToReg,
    output logic ASel,
    output logic Branch,
    output logic [1:0] Jump,
    output logic Exception,
    output logic [4:0] ALUOp
);

    // RegDst
    always_comb begin
        if (reset) begin
            RegDst <= 2'b00;
        end else if (irq && ~pc31) begin
            RegDst <= 2'b11;
        end else if (Exception) begin
            RegDst <= 2'b11;
        end else begin
            case (opCode)
                6'b001000, // addi 11010
                6'b001100, // adi
                6'b001101, // ori
                6'b001110, // xori
                6'b100011: // lw
                    RegDst <= 2'b01;
                6'b000011: // jal
                    RegDst <= 2'b10;
                default:
                    RegDst <= 2'b00;
            endcase
        end
    end
    
    // ALUSrc
    always_comb begin
        if (reset) begin
            ALUSrc <= 2'b00;
        end else if (irq && ~pc31) begin
            ALUSrc <= 2'b00;
        end else if (Exception) begin
            ALUSrc <= 2'b00;
        end else begin
            case (opCode)
                6'b001000: // addi
                    ALUSrc <= 2'b11;
                6'b001100, // andi
                6'b001101, // ori
                6'b001110, // xori
                6'b100011, // lw
                6'b101011: // sw
                    ALUSrc <= 2'b10;
                6'b000000:
                    case (funct)
                        6'b000000, // sll
                        6'b000010, // srl
                        6'b000011: // sra
                            ALUSrc <= 2'b01;
                        default:
                            ALUSrc <= 2'b00;
                    endcase
                default:
                    ALUSrc <= 2'b00;
            endcase
        end
    end

    // RegWrite
    always_comb begin
        if (reset) begin
            RegWrite <= 1'b0;
        end else if (irq && ~pc31) begin
            RegWrite <= 1'b1;
        end else if (Exception) begin
            RegWrite <= 1'b1;
        end else begin
            case (opCode)
                6'b001000, // addi
                6'b001100, // andi
                6'b001101, // ori
                6'b001110, // xori
                6'b100011, // lw
                6'b000011: // jal
                    RegWrite <= 1'b1;
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
                            RegWrite <= 1'b1;
                        default:
                            RegWrite <= 1'b0;
                    endcase
                default:
                    RegWrite <= 1'b0;
            endcase
        end
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
    always_comb begin // defaults -> exceptions
        if (reset) begin
            ALUOp <= 6'b11010;
        end else if (irq && ~pc31) begin
            ALUOp <= 6'b11010;
        end else if (Exception) begin
            ALUOp <= 6'b11010;
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
                        3'b001:
                            case (funct[2:0])
                                3'b001:     ALUOp <= 5'b11010; // jr
                                default:    ALUOp <= 5'b11010; // exception
                            endcase
                        3'b100:
                            case (funct[2:0])
                                3'b000:     ALUOp <= 5'b00000; // add
                                3'b010:     ALUOp <= 5'b00001; // sub
                                3'b100:     ALUOp <= 5'b11000; // and
                                3'b101:     ALUOp <= 5'b11110; // or
                                3'b110:     ALUOp <= 5'b10110; // xor
                                3'b111:     ALUOp <= 5'b10001; // nor
                                default:    ALUOp <= 5'b11010; // exception
                            endcase
                        3'b101:
                            case (funct[2:0])
                                3'b010:     ALUOp <= 5'b00111; // slt
                                default:    ALUOp <= 5'b11010; // exception
                            endcase
                        default:
                            ALUOp <= 5'b11010; // nop
                    endcase
                6'b001000:  ALUOp <= 5'b00000;  // addi
                6'b001100:  ALUOp <= 5'b11000;  // andi
                6'b001101:  ALUOp <= 5'b11110;  // ori
                6'b001110:  ALUOp <= 5'b10110;  // xori
                6'b100011:  ALUOp <= 5'b11010;  // lw
                6'b101011:  ALUOp <= 5'b11010;  // sw
                6'b000010:  ALUOp <= 5'b11010;  // j
                6'b000011:  ALUOp <= 5'b11010;  // jal
                6'b000100:  ALUOp <= 5'b00001;  // beq
                6'b000101:  ALUOp <= 5'b00001;  // bne
                default:    ALUOp <= 5'b11010;  // exception
            endcase
        end
    end

    // ASel
    always_comb begin
        if (reset) begin
            ASel <= 1'b0;
        end else if (irq && ~pc31) begin
            ASel <= 1'b1;
        end else if (Exception) begin
            ASel <= 1'b1;
        end else begin
            case (opCode)
                6'b000011:
                    ASel <= 1'b1;
                default:
                    ASel <= 1'b0;
            endcase
        end
    end

    // exceptions
    always_comb begin
        case (opCode)
            6'b001000, // addi
            6'b001100, // andi
            6'b001101, // ori
            6'b001110, // xori
            6'b100011, // lw
            6'b101011, // sw
            6'b000010, // j
            6'b000011, // jal
            6'b000100, // beq
            6'b000101: // bne
                Exception <= 1'b0;
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
                    6'b000011, // sra
                    6'b001000: // jr
                        Exception <= 1'b0;
                    default:
                        Exception <= 1'b1;
                endcase
            default:
                Exception <= 1'b1;
        endcase
    end

    // Branch
    always_comb begin
        case (opCode)
            6'b000100, // beq
            6'b000101: // bne
                Branch <= 1'b1;
            default:
                Branch <= 1'b0;
        endcase
    end

    // Jump
    always_comb begin
        case(opCode)
            6'b000010, // j
            6'b000011: // jal
                Jump <= 2'b10;
            6'b000000:
                if (funct == 6'b001000) begin
                    Jump <= 2'b11; // jr
                end else begin
                    Jump <= 2'b00;
                end
            6'b000100, // beq
            6'b000101: // bne
                Jump <= 2'b01;
            default:
                Jump <= 2'b00;
        endcase
    end

endmodule