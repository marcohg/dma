An example of using DMA on Tx of UART for Kinetis K20
MH - Jul, 11,2013
- Jul 15, make cpu more compatible for merging (minor clock settings)
- Jul15: Added second port, both has DMA. 

MONITOR:
- Toggling TP1 every main loop 
- Toggling TP2 on DMA complete (it happen actually 2 bytes before trsnmission is completed)
	( The DMA1 component is 'freezed' as any added code will be erased every time PEX generates code)
		Something like this to toggle pin TP2:
		GPIO1_ToggleFieldBits(GPIO1_Ptr, TEST_POINTS, 0x02U);
  

Test 	port 	Monitor on TWR elevator
TP1 	PTB18	(A22)
TP2 	PTB19	(A23)
TP3 	PTB20	(B23)
TP4		PTB21	(A11)
TP5		PTB22	(A10)

UART0 TX(PTD7) RX(PTD6), 115200,8,n,1 - 
 Rx Reads char isr -one by one- 
 
 A rec commands does the following with the As1TxBuffer[]:
  	'~' Fills the As1TxBuffer block with consecutive data 1,2,3,..TX_MAX 
	'1' Sends first 10, '2' sends first 50, '3' sends all TX_MAX buffer
	Any others is stored in As1TxBufferptr++] 
	\r  Sends ptr bytes starting at As1TxBuffer[0], and resets ptr=0  
 
 /// @TODO Use the Transmit Complete flag to test TX-driver (RS485)
   