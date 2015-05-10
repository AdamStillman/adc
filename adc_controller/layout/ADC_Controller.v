`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:39:57 04/13/2015 
// Design Name: 
// Module Name:    ADC_Controller 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module ADC_Controller( input start,
								input Comp,
								input Clock,
								output reg sample,
								output reg datardy,
								output reg [5:0] B
    );
	
		parameter [2:0] State0 = 3'd0, 
							 State1 = 3'd1, 
							 State2 = 3'd2, 
							 State3 = 3'd3,
							 State4 = 3'd4;
							 
		reg [2:0] currentState, nextState;
		reg beginCount;
		reg counterRst;
		
		initial
			begin
				sample <= 1'b0;
				datardy <= 1'b0;
				B <= 6'b000000;
				currentState <= 3'b000;
				nextState <= 3'b000;
				beginCount <= 0;
				counterRst <= 0;
			end
	
		always @ (currentState, start, Comp, B)
			begin
				case(currentState)
					State0: begin
							sample <= 1'b0;
							datardy <= 1'b0;
							beginCount <= 0;
							counterRst <= 0;
						if(start && ~Comp)
							begin
								nextState <= State0;
							end
						else
							begin
								nextState <= State1;
							end
					end
					State1: begin
						sample <= 1'b1;
						datardy <= 1'b0;
						beginCount <=1;
						counterRst <= 0;
						if(B == 6'b111111)
							nextState <= State2;
						else
							nextState <= State1;
					end
					State2: begin
							sample <= 1'b0;
							datardy <= 1'b0;
							beginCount <= 0;
							counterRst <= 1;
						if(~start && ~Comp)
							begin
								nextState <= State3;
							end
						else
							begin
								nextState <= State0;
							end
					end
					State3: begin
							sample <= 1'b0;
							datardy <= 1'b0;
							beginCount <= 1;
							counterRst <= 0;
						if(~start && ~Comp)
							begin
								nextState <= State3;
							end
						else if(~start && Comp)
							begin
								nextState <= State4;
							end
					end
					State4: begin
							sample <= 1'b0;
							datardy <= 1'b1;
							beginCount <= 0;
							counterRst <= 0;
						if(start && ~Comp)
							begin
								nextState <= State0;
							end
						else if(~start && Comp)
							begin
								nextState <= State4;
							end
					end
				endcase
			end
		
		always @ (posedge Clock)
			begin
				//Transitions from current state to next state
				currentState <= nextState;
					
				if(beginCount)
					begin
						if(B == 6'b111111 || counterRst)
							B <= 6'b000000;
						else
							B <= B + 1'b1;
					end
			end

endmodule
