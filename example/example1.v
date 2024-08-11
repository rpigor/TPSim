// example1.v
// 2-input multiplexer implemented using AND2 and inverter gates
// This netlist is accompanied by a stimuli file (example1.sti)

module MUX2(A, B, S, Y);
    input A;
    input B;
    input S;
    output Y;

    wire S_n;
    wire term1;
    wire term2;
    wire term1_n;
    wire term2_n;
    wire and_n;

    // gate-level instances
    INVX1 inst1(.A(S), .Y(S_n));
    AND2X1 inst2(.A(A), .B(S_n), .Y(term1));
    AND2X1 inst3(.A(B), .B(S), .Y(term2));

    INVX1 inst4(.A(term1), .Y(term1_n));
    INVX1 inst5(.A(term2), .Y(term2_n));

    AND2X1 inst6(.A(term1_n), .B(term2_n), .Y(and_n));
    INVX1 inst7(.A(and_n), .Y(Y));
endmodule
