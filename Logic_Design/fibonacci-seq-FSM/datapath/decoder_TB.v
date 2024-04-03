module decoder_TB ();

reg [1:0]Y;

wire [3:0]D;

two_to_four_line_decoder DUT(Y,D);

always begin 

Y=0; #100;
Y=1; #100;
Y=2; #100;
Y=3; #100;

end

endmodule
