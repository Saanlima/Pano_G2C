module bscan_spi (
  SPI_MISO,
  SPI_MOSI,
  SPI_CS,
  SPI_SCK);

  input SPI_MISO;
  output SPI_MOSI;
  output SPI_CS;
  output SPI_SCK;
  
  wire reset;
  wire user_CAPTURE;
  wire user_DRCK;
  wire user_RESET;
  wire user_SEL;
  wire user_SHIFT;
  wire user_TDI;
  wire user_UPDATE;
  reg user_TDO;
  reg SPI_MOSI;
  reg SPI_SCK;
  reg SPI_CS;
  reg CS_GO;
  reg have_header;
  reg [31:0] tdi_mem;
  reg [7:0] tdo_mem;
  reg [15:0] len;

  assign reset = user_CAPTURE | user_RESET | user_UPDATE | ~user_SEL;

  always @ * begin
    user_TDO = tdo_mem[7];
    if (user_SEL) begin
      SPI_MOSI = user_SHIFT ? user_TDI : 1'b0;
      SPI_SCK = user_SHIFT ? user_DRCK : 1'b1;
      SPI_CS = ~CS_GO;
    end else begin
      SPI_MOSI = 1'b0;
      SPI_CS = 1'b1;
      SPI_SCK = 1'b0;
    end
  end

  always @ (negedge user_DRCK or posedge reset) begin
    if (reset)  begin
      have_header <= 1'b0;
      CS_GO <= 1'b0;
      len <= 16'd0;
    end else begin
      if (~have_header) begin
        if (tdi_mem[31:16] == 16'b0101100110100110) begin
          len <= tdi_mem[15:0];
          have_header <= 1'b1;
          if (tdi_mem[15:0] > 16'd0) begin
            CS_GO <= 1'b1;
          end
        end
      end else if (len > 0) begin
        len <= len - 1'b1;
        if (len == 16'd1)
          CS_GO <= 1'b0;
      end
    end
  end

  always @ (posedge user_DRCK or posedge reset) begin
    if (reset) begin
      tdo_mem <= 8'd0;
      tdi_mem <= 32'd0;
    end else begin
      tdi_mem <= {tdi_mem[30:0], user_TDI};
      tdo_mem <= {tdo_mem[6:0], SPI_MISO};
    end
  end

  BSCAN_SPARTAN6 BS (
    .CAPTURE(user_CAPTURE),
		.DRCK(user_DRCK),
		.RESET(user_RESET),
		.SEL(user_SEL),
		.SHIFT(user_SHIFT),
		.TDI(user_TDI),
		.UPDATE(user_UPDATE),
		.TDO(user_TDO)
  );

endmodule

