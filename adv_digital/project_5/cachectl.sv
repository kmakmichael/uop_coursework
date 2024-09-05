module cachectl(
    input logic clk,
    input logic MemRead, MemReadReady,
    input logic MemWrite, MemWriteDone,
    input logic MemHit,
    output logic MemReadDone, CacheRead,
    output logic MemWriteReady, CacheWrite = 1'b0,
    output logic stall
);

    logic miss = 1'b0, write = 1'b0;;
    assign CacheRead = MemRead || MemWrite;
    assign stall = miss || write;
    assign MemReadDone = miss & MemHit;
    assign MemWriteReady = write & MemHit;

    // read miss detection
    always_ff @(negedge clk) begin
        if (MemRead & ~MemHit) begin
            miss <= 1'b1;
        end
    end
    // stop stalling for read miss
    always_ff @(negedge MemReadReady) begin
        miss <= 1'b0;
    end
    // write stall
    always_ff @(negedge clk) begin
        if (MemWrite & ~MemWriteDone) begin
            write <= 1'b1;
        end
    end
    // stop stalling for write
    always_ff @(negedge clk) begin
        if (MemWriteDone)
            write <= 1'b0;
    end

    always_comb begin
        CacheWrite <= (MemReadReady & ~MemHit) || (MemWrite & ~MemHit);
    end


    /*always_ff @(negedge clk) begin
        MemReadDone <= miss & MemHit;
    end

    always_comb begin
        CacheWrite <= miss & ~MemReadDone;
    end
   
   /* typedef enum logic [1:0] {idle, rst, miss, write} statetype;
    statetype current_state = idle;
    statetype next_state = idle;
    // state cycle
    always_ff @(posedge clk) begin
        current_state <= next_state;
    end

    // detect cache miss
    always_comb begin
        case (current_state)
            idle:
                if (MemRead & ~MemHit) begin
                    next_state <= miss;
                end else
                    next_state <= idle;
            miss:
                if (MemReadDone)
                    next_state <= rst;
                else
                    next_state <= miss;
            default:
                next_state <= idle;
        endcase
    end

    // stall
    always_comb begin
        case (next_state)
            idle:
                stall <= 1'b0;
            miss,
            write,
            rst:
                stall <= 1'b1;
            default:
                stall <= 1'b0;
        endcase
    end

    // MemReadDone
    always_comb begin
        if (current_state == miss) begin
            MemReadDone <= MemHit;
            CacheSrc <= 1'b1;
        end else
            MemReadDone <= 1'b0;
            CacheSrc <= 1'b0;
    end

    // CacheWrite
    always_comb begin
        case(current_state)
            miss:
                CacheWrite <= MemReadReady;
            default:
                CacheWrite <= 1'd0;
        endcase
    end*/
endmodule