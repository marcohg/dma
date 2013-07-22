/* ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MK20DX256VLL7
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2013-07-11, 12:59, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file ProcessorExpert.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup ProcessorExpert_module ProcessorExpert module documentation
**  @{
*/         
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "AS1.h"
#include "GPIO1.h"
#include "AS3.h"
#include "DMA1.h"
#include "DMAT_UART0.h"
#include "DMAT_UART1.h"
#include "TINT1.h"
#include "TU1.h"
#include "AS2.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#define USE_DMA_FOR_TX  /*  How Tx isr are handled: DMA or isr one-by-one */
#define TWO_DMA_UARTS       /*  define if using UART0 and UART1*/
#define TX_MAX 200
LDD_TError Error;
volatile byte As1RxBuffer[20], As1TxBuffer[TX_MAX];
volatile byte As2RxBuffer[20], As2TxBuffer[TX_MAX];
volatile byte As3RxBuffer[10], As3TxBuffer[20];


volatile bool As1OnRecByte = FALSE, As1BlockSent;
volatile bool As2OnRecByte = FALSE, As2BlockSent;
volatile bool As3OnRecByte = FALSE, As3BlockSent;
volatile word As1TxCompleteCtr;

extern volatile bool bSystemTick = FALSE;
LDD_TDeviceData *GPIO1_Ptr;

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  LDD_TDeviceData *As1testDevData, *As2testDevData, *As3testDevData;
  byte RecChar =0;
  volatile word TxCount =0;
  word xferSize = 16;
  bool trigger =TRUE;
  word As3Counter =0;
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  // Init here most of PEX components (set to NO Auto-initialization feature) 

  (void)TINT1_Init(NULL);
  As1testDevData = AS1_Init(NULL);
  As2testDevData = AS2_Init(NULL);
  As3testDevData = AS3_Init(NULL);
  GPIO1_Ptr = GPIO1_Init(NULL);        // Init GPIO for testing
      
  Error = AS1_ReceiveBlock(As1testDevData, (LDD_TData*) &As1RxBuffer, 1); // link component to As1RxBuffer, and event happens with one byte
  Error = AS2_ReceiveBlock(As2testDevData, (LDD_TData*) &As2RxBuffer, 1); 
  Error = AS3_ReceiveBlock(As3testDevData, (LDD_TData*) &As3RxBuffer, 1);
  
  // Fill serial buffer with dummy data
  uint16_t m;
  for(m=0;m<TX_MAX;m++)
  {
    As2TxBuffer[m] = As1TxBuffer[m] = (m & 0x1F) + '0'; // only ASCII visible
  }
  
#ifdef USE_DMA_FOR_TX
  // Allocate and enable DMA channel for UART transfers - only once
  Error = DMAT_UART0_AllocateChannel(DMAT_UART0_DeviceData);
  Error = DMAT_UART0_EnableChannel(DMAT_UART0_DeviceData);
  // Config UART0
  UART0_C5 |= UART_C5_TDMAS_MASK;
  
#ifdef TWO_DMA_UARTS
  // Allocate and enable DMA channel for UART transfers - only once
  Error = DMAT_UART1_AllocateChannel(DMAT_UART1_DeviceData);
  Error = DMAT_UART1_EnableChannel(DMAT_UART1_DeviceData);
  UART1_C5 |= UART_C5_TDMAS_MASK;
#endif
   
#else
  // Additional Initializarion when not using DMA
#endif
  trigger = FALSE;
  
  // Enable the Low Power Timer
  // === LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;                   // Use this line if using the Init module
  // ==== Cpu_EnableInt();

  // Transmit dummy data over UART
  // UART0_C2 |= UART_C2_TIE_MASK;
    
  while(1)
  {
          
    // Poll if Received ONE char on any UART
    if(As1OnRecByte || As2OnRecByte)
    {
      word fillBuffer=0;
      if(As1OnRecByte)
      {
        RecChar = As1RxBuffer[0];
        Error = AS1_ReceiveBlock(As1testDevData, (LDD_TData*) &As1RxBuffer, 1); // prepare to rec next byte
      }
              
      else
      {
        RecChar = As2RxBuffer[0];
        Error = AS2_ReceiveBlock(As2testDevData, (LDD_TData*) &As2RxBuffer, 1); // prepare to rec next byte
      }
      As1OnRecByte = As2OnRecByte =FALSE;
      
      
      switch(RecChar)
      {
        case '\r': // Send the Block usimg DMA
          if(TxCount)
          { 
            // Send what we read 
            //GPIO1_SetFieldBits(GPIO1_Ptr, TEST_POINTS, 0x02U);
            As2BlockSent = As1BlockSent = FALSE;
            xferSize = TxCount;
            trigger = TRUE;
          }
          TxCount =0;
          break;
        // Fill the buffer with sequential    
        case '~':
          fillBuffer =0;
          while(fillBuffer < TX_MAX)
            As2TxBuffer[fillBuffer++]  = As1TxBuffer[fillBuffer] = fillBuffer+1;
          //GPIO1_SetFieldBits(GPIO1_Ptr, TEST_POINTS, 0x02U);
          As2BlockSent = As1BlockSent = FALSE;
          // Error = AS1_SendBlock(As1testDevData, (LDD_TData*) &As1TxBuffer, TX_MAX);
          break;
          
        // Set size of transmission and trigger
        case '1':
          xferSize = 10;
          trigger = TRUE;
          
          break;
          
        case '2':
          xferSize = 50;
          trigger = TRUE;
          break;
          
        case '3':
          xferSize = TX_MAX;
          trigger = TRUE;
          break;
          
          
        default:
          if(TxCount<TX_MAX)
            As1TxBuffer[TxCount++] = As2TxBuffer[TxCount] = RecChar; // Fill the buffer
      }
    }
    // Poll
    // Measure Block transmission 
    if(As1BlockSent)
    {
      As1BlockSent = FALSE;
      // useless GPIO1_ClearFieldBits(GPIO1_Ptr, TEST_POINTS, 0x02U);
    }
    if(trigger)
    {
      trigger = FALSE;
      
#ifdef USE_DMA_FOR_TX
      // These are TX using DMA
      // These are the register of the DMA that need to be changed when size changes
      // Source Address -> reset to buffer start
      Error = DMAT_UART0_SetSourceAddress(DMAT_UART0_DeviceData,(LDD_DMA_TAddress)(uint32_t)&As1TxBuffer); 
      // We will xfer this amount of bytes (MAJOR loop)
      Error = DMAT_UART0_SetTransferCount(DMAT_UART0_DeviceData, xferSize);
          
      // Request a transmit TDRE (xmit register empty), which starts the DMA on ch.0
      UART0_C2 |= UART_C2_TIE_MASK;
      
#ifdef TWO_DMA_UARTS
      // Source Address -> reset to buffer start
      Error = DMAT_UART1_SetSourceAddress(DMAT_UART1_DeviceData,(LDD_DMA_TAddress)(uint32_t)&As2TxBuffer); 
      // We will xfer this amount of bytes (MAJOR loop)
      Error = DMAT_UART1_SetTransferCount(DMAT_UART1_DeviceData, xferSize);
      // Request a transmit TDRE (xmit register empty), which starts the DMA on ch.0
      UART1_C2 |= UART_C2_TIE_MASK;
#endif
      
#else
      AS1_SendBlock(As1testDevData, (LDD_TData *)As1TxBuffer, (uint16_t)xferSize);
#endif
    }
    // Test the S3L 
    if(As3OnRecByte)  // Just echo whatever you receive
    {
      Error = AS3_ReceiveBlock(As3testDevData, (LDD_TData*) &As3RxBuffer, 1); // prepare to rec next byte
      // Echo what we received
      RecChar = As3RxBuffer[0];
      As3OnRecByte = FALSE;
      Error = AS3_SendBlock(As3testDevData, (LDD_TData *)&RecChar, (uint16_t)1);
      As3Counter =0;
    }
    else
    if(As3Counter > 100)  // Every 5 secs
    {
      As3OnRecByte = FALSE;
      As3Counter=0;
      Error = AS3_SendBlock(As3testDevData, (LDD_TData *)"This one S3L.. ", (uint16_t)15);
            
    }
    
        
    //  Monitor Main loop scan
    if(bSystemTick)
    {
      bSystemTick= FALSE;
      GPIO1_ToggleFieldBits(GPIO1_Ptr, TEST_POINTS, 0x01U);
      ++As3Counter;
    }
    if(As1TxCompleteCtr > 0)
    {
      asm( "nop");
    }
    
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.2 [05.06]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
