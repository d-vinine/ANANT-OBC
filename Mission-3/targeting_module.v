`timescale 1ns/1ps

module targeting_system (
    input wire clk,
    input wire rst_n, 
    input wire [2:0] sensor_in,
    output reg proton_fire 
);

    localparam IDLE         = 4'd0,
               ONE_111      = 4'd1,
               TWO_111      = 4'd2,
               LEFT_GATE    = 4'd3,
               RIGHT_GATE   = 4'd4,
               HEMI_0       = 4'd5,
               HEMI_1       = 4'd6;

    reg [3:0] state;
    reg [4:0] cnt; 

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state       <= IDLE;
            cnt         <= 5'd0;
            proton_fire <= 1'b0;
        end else if (sensor_in == 3'b101 && state != IDLE) begin
            state       <= IDLE;
            cnt         <= 5'd0;
            proton_fire <= 1'b0;
        end else begin
            proton_fire <= 1'b0;

            case (state)
                IDLE: begin
                    if (sensor_in == 3'b111) state <= ONE_111;
                end

                ONE_111: begin
                    if (sensor_in == 3'b111) state <= TWO_111;
                    else state <= IDLE;
                end

                TWO_111: begin
                    if (sensor_in == 3'b001) state <= LEFT_GATE;
                    else state <= IDLE;
                end

                LEFT_GATE: begin
                    if (sensor_in == 3'b010) begin
                        state <= RIGHT_GATE;
                        cnt   <= 5'd0; 
                    end else begin
                        state <= IDLE;
                    end
                end

                RIGHT_GATE: begin
                    cnt <= 5'd1; 
                    if (sensor_in == 3'b100) state <= HEMI_1;
                    else state <= HEMI_0;
                end

                HEMI_0: begin
                    if (sensor_in == 3'b100) begin
                        state <= HEMI_1;
                        cnt   <= cnt + 1'b1;
                    end 
                    else if (cnt >= 5'd16) begin
                        state <= IDLE;
                    end 
                    else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                HEMI_1: begin
                    if (sensor_in == 3'b100) begin
                        proton_fire <= 1'b1; 
                        state <= IDLE;     
                    end 
                    else if (cnt >= 5'd16) begin
                        state <= IDLE;
                    end 
                    else begin
                        cnt <= cnt + 1'b1;
                    end
                end

                default: state <= IDLE;
            endcase
        end
    end
endmodule
