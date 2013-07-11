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
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#define TX_MAX 10
LDD_TError Error;
byte As1RxBuffer[20], As1TxBuffer[TX_MAX];
volatile bool As1OnRecByte = FALSE;
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  LDD_TDeviceData *As1testDevData;
  byte RecChar =0;
  word TxCount =0;
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  As1testDevData = AS1_Init(NULL);
  Error = AS1_ReceiveBlock(As1testDevData, (LDD_TData*) &As1RxBuffer, 1); // Receive only one byte
  while(1)
  {
    // Test if Received char
    if(As1OnRecByte)
    {
      word fillBuffer=0;
      As1OnRecByte = FALSE;
      RecChar = As1RxBuffer[0];
      Error = AS1_ReceiveBlock(As1testDevData, (LDD_TData*) &As1RxBuffer, 1); // prepare to rec next byte
      switch(RecChar)
      {
        case '\r': // Send the Block usimg DMA
          if(TxCount)
            Error = AS1_SendBlock(As1testDevData, (LDD_TData*) &As1TxBuffer, TxCount);
          TxCount =0;
          break;
        // Fill and send the TxBuffer   
        case '~':
          fillBuffer =0;
          while(fillBuffer < TX_MAX)
            As1TxBuffer[fillBuffer++] = fillBuffer+1;
          Error = AS1_SendBlock(As1testDevData, (LDD_TData*) &As1TxBuffer, TX_MAX);
          break;
        default:
          if(TxCount<TX_MAX)
            As1TxBuffer[TxCount++] = RecChar; // Fill the buffer
      }
        
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
