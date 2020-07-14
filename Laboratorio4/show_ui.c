/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */
#include <stdint.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "parser.h"
#include <time.h>
#include "calendar.h"
#include "show_ui.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */

static uint8_t numBytes;
static uint8_t buffer[30];
static uint8_t message[5];
static uint8_t okMessage[3];
static uint8_t nl[2];
static enum pendingMessage state = AVAILABLE;


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */

/** 
  @Function
    int ExampleLocalFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Description
    Full description, explaining the purpose and usage of the function.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Precondition
    List and describe any required preconditions. If there are no preconditions,
    enter "None."

  @Parameters
    @param param1 Describe the first parameter to the function.
    
    @param param2 Describe the second parameter to the function.

  @Returns
    List (if feasible) and describe the return values of the function.
    <ul>
      <li>1   Indicates an error occurred
      <li>0   Indicates an error did not occur
    </ul>

  @Remarks
    Describe any special behavior not described above.
    <p>
    Any additional remarks.

  @Example
    @code
    if(ExampleFunctionName(1, 2) == 0)
    {
        return 3;
    }
 */
void sendOkMessage(){
    memcpy(okMessage, SuccessMessage, sizeof(SuccessMessage));
    putUSBUSART(okMessage, 3);
}

void sendNewLine(){
    nl[0] = 0xA;
    nl[1] = 0xD;
    putUSBUSART(nl, 2);
}



/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */
void showInterface(struct tm *tc) {
    switch (state) {
        case STILL_SENDING:
            if (getEventListAndSend(CALENDAR_LENGTH)) {
                state = JUST_SENT;
            }
            break;
        case JUST_SENT:
            sendNewLine();
            state = AVAILABLE;
            break;
        case AVAILABLE:
            numBytes = getsUSBUSART(buffer, sizeof (buffer));
            if (numBytes > 0) {
                switch (buffer[0]) {
                    case 'L':
                        if (numBytes == 1) {
                            if (!getEventListAndSend(CALENDAR_LENGTH)) {
                                state = STILL_SENDING;
                            }
                        }
                        break;
                    case 'G':
                        // The Character 'G' (0x47) is for getting the current time
                        if (numBytes == 1) {
                            codeCurrentTime(message, tc, 0);
                            putUSBUSART(message, sizeof (message));
                        }
                        break;
                    case 'D':
                        if (numBytes == (2 + getCalendarPositionRange())) {
                            if(deleteEventInCalendar(buffer)){
                             sendOkMessage();   
                            }
                        }
                        break;
                    case 'A':
                        /*  Adding event to calendar format : 'A x y z HH:MM DD/MM/YY'; x stands for turnOn/Off;
                         *  y stands for LedNumber and z stands for LedColor */
                        if (numBytes == 22) {
                            if (addEventToCalendar(buffer)){
                                sendOkMessage();
                            }
                        }
                        break;
                    case 'S':
                        if (numBytes == 7) {
                            decodeCurrentTime(buffer, tc);
                            RTCC_TimeSet(tc);
                            sendOkMessage();
                        }
                        break;
                    default:
                        break;
                }
                if (state == AVAILABLE){
                       state = JUST_SENT;
                }
            }
            break;
        default:
            break;
    }
}





/* *****************************************************************************
 End of File
 */
