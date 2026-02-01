`timescale 1ns / 1ps

module testfixture;

  // DUT I/O
  logic [22:0] A, B;
  logic [23:0] Z;

  // Instantiate DUT
  Mul_Mod dut (
    .A(A),
    .B(B),
    .Z(Z)
  );

  // Internal variables
  int infile, line = 0;
  int passed = 0, failed = 0;
  int num_tests = 0;

  int A_int, B_int, golden_Z;
  string line_str;

  initial begin
    infile = $fopen("golden.dat", "r");
    if (infile == 0) begin
      $display("Failed to open golden.dat");
      $finish;
    end

    $display("Begin testing full_modular_multiplier");

    while (!$feof(infile)) begin
      line++;
      line_str = "";
      void'($fgets(line_str, infile));
      if (line_str == "") continue;

      void'($sscanf(line_str, "%d %d %d", A_int, B_int, golden_Z));

      A = A_int[22:0];
      B = B_int[22:0];

      #1;  // Allow propagation

      if (Z === golden_Z[23:0]) begin
        $display("Test %0d: A=%0d, B=%0d => Z=%0d [PASS]", line, A, B, Z);
        passed++;
      end else begin
        $display("Test %0d: A=%0d, B=%0d => Z=%0d (expected %0d)", line, A, B, Z, golden_Z);
        failed++;
      end

      num_tests++;
    end

    $display("=== TEST SUMMARY ===");
    $display("Total tests: %0d", num_tests);
    $display("Passed     : %0d", passed);
    $display("Failed     : %0d", failed);
    $display("====================");
    $finish;
  end

endmodule
