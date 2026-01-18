
//////////////////////////////////////////////////////////////////////////////////
// Company: Rebel Alliance Engg. Corps.
// Engineer: Recruit_ID ANANT_123 
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module tb_targeting_system();

    // Inputs
    reg clk;
    reg rst_n;
    reg [2:0] sensor_in;

    // Output
    wire proton_fire;

    // Instantiate the Unit Under Test (UUT)
    targeting_system uut (
        .clk(clk),
        .rst_n(rst_n),
        .sensor_in(sensor_in),
        .proton_fire(proton_fire)
    );

    // Clock generation: 100MHz (10ns period)
    always begin
        #5 clk = ~clk;
    end

    // Task to apply sensor data synchronized to the clock
    task send_sensor;
        input [2:0] val;
        begin
            sensor_in = val;
            @(posedge clk);
            #1; // Offset to allow observation of signals after the clock edge
        end
    endtask

    initial begin
        // --- Initialization ---
        clk = 0;
        rst_n = 0;
        sensor_in = 0;

        // --- Async Reset ---
        #20 rst_n = 1;
        @(posedge clk);

        // --- SCENARIO 1: The Successful Trench Run ---
        // Expected: proton_fire pulses HIGH at Cycle 8
        $display("--- TRENCH RUN START: SCENARIO 1 ---");
        send_sensor(3'd0); // Cycle 0: Idle
        send_sensor(3'd7); // Cycle 1: Sync 1
        send_sensor(3'd7); // Cycle 2: Sync 2 (Valid Calibration)
        send_sensor(3'd1); // Cycle 3: Left Gate
        send_sensor(3'd2); // Cycle 4: Right Gate
        send_sensor(3'd0); // Cycle 5: Window T=1
        send_sensor(3'd4); // Cycle 6: Window T=2 (Core 1)
        send_sensor(3'd0); // Cycle 7: Window T=3
        send_sensor(3'd4); // Cycle 8: Window T=4 (Core 2) -> TARGET LOCKED
        
        if (proton_fire === 1'b1) 
            $display("[SUCCESS] Cycle 8: Proton Torpedoes Fired! Direct Hit!");
        else 
            $display("[FAILURE] Cycle 8: No fire signal. The Rebellion is lost.");

        send_sensor(3'd0); // Cycle 9: Post-fire IDLE
        
        // --- SCENARIO 2: Vader uses the Force (Reset) ---
        $display("\n--- ENEMY INTERCEPT: SCENARIO 2 ---");
        
        send_sensor(3'd7); send_sensor(3'd7); // Valid Sync
        send_sensor(3'd1); send_sensor(3'd2); // Valid Gates
        send_sensor(3'd4);                    // Core 1
        send_sensor(3'd5);                    // VADER'S LOCK (Logic must reset)
        send_sensor(3'd4);                    // This would have been Core Signature 2
        
        if (proton_fire === 1'b0)
            $display("[SUCCESS] Aborted run. Vader's lock correctly reset the FSM.");
        else
            $display("[FAILURE] Fire signal triggered despite Vader's lock!");

        // --- SCENARIO 3: Noise Filtering (Exactly Two Rule) ---
        $display("\n--- SENSOR NOISE: SCENARIO 3 ---");
        
        send_sensor(3'd7); send_sensor(3'd7); send_sensor(3'd7); // Noise (3 cycles of 7)
        send_sensor(3'd1); send_sensor(3'd2);                    // Gates (Should ignore)
        send_sensor(3'd4); send_sensor(3'd4);                    // Cores (Should ignore)
        
        if (proton_fire === 1'b0)
            $display("[SUCCESS] Noise correctly filtered. Exactly-Two-Cycles rule followed.");
        else
            $display("[FAILURE] Logic triggered on faulty calibration!");

        $display("\n--- MISSION SIMULATION COMPLETE ---");
        $finish;
    end

 	initial begin
      $dumpfile("dump.vcd");
      $dumpvars(0, tb_targeting_system);
	end
endmodule

