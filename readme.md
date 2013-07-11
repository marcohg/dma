An example of using DMA on Tx of UART for Kinetis K20
MH - Jul, 11,2013
Toggling the TP2 every TX (inside's AS1 InterruptTx()

// This line will be erased every time PEX generates code
  GPIO1_ToggleFieldBits(GPIO1_Ptr, TEST_POINTS, 0x02U);
  

GPIOs at PTB
	TP1 PTB18	(A22)
	TP2 PTB19	(A23)
	TP3 PTB20	(B23)
	TP4	PTB21	(A11)
	TP5	PTB22	(A10)
	

- UART0 TX(PTD7) RX(PTD6), 115200,8,n,1 - Reads char -one by one- and send block on \r, ~
- RAM and FLASH K20 Targets
 
