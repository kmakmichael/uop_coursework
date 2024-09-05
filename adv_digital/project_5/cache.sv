typedef struct packed {
    logic valid;
    logic [25:0] tag;
    logic [31:0] data;
} entry;

module cache(
    input logic clk, 
    input logic [31:0] addr,
    input logic rden,
    input logic wren,
    input logic [31:0] wrData,
    output logic MemHit = 1'b0,
    output logic [31:0] q
);
    entry [63:0] c_mem;
    // ModelSim init
    initial begin
        for (int i = 0; i < 64; i++)
            c_mem[i] = {1'b0, 26'd0, 32'd0};
    end

    logic [25:0] tag;
    logic [5:0] indx;
    assign tag = addr[31:6];
    assign indx = addr[5:0];
    assign q = c_mem[indx].data;

    always_comb begin
        if (rden) begin
            if (c_mem[indx].valid == 1'b1) begin
                if (c_mem[indx].tag == tag) begin
                    MemHit <= 1'b1;
                end else begin
                    MemHit <= 1'b0;
                end
            end else begin
                MemHit <= 1'b0;
            end
        end else
            MemHit <= 1'b0;
    end

    always_ff @(negedge clk) begin
        if (wren) begin
            c_mem[indx].valid <= 1'b1;
            c_mem[indx].tag <= tag;
            c_mem[indx].data <= wrData;
        end
    end

endmodule