/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    calendar.c

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

#include "calendar.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include <stdbool.h>
#include <stdio.h>
#include "parser.h"
#include <time.h>
#include "mcc_generated_files/pin_manager.h"
#include "LEDs_RGB/WS2812.h"

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
bool empty;
bool deleted;
bool added;
bool paramsOk;
struct tm aux;
int pos_range;
int parsedPosition;
static int position;
int state;
int next_state;
uint8_t event_command;
uint8_t event_param;
uint8_t event_color;
uint8_t event_time[2];
uint8_t event_date[3];
uint8_t event_message[22];
struct tm event_tm;
app_event_t new_event;
static app_event_t calendario[CALENDAR_LENGTH];

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
int getCalendarPositionRange() {
    pos_range = 1;
    if (CALENDAR_LENGTH > 9) {
        pos_range = 2;
    } else if (CALENDAR_LENGTH > 99) {
        pos_range = 3;
    }
    return pos_range;
}

static int parsePositionId(uint8_t *buffer, int position_range) {
    int fs = buffer[2] - 0x30;
    int sd;
    int th;
    parsedPosition = -1;
    switch (position_range) {
        case 1:
            if ((fs >= 0) && (fs < CALENDAR_LENGTH)) {
                parsedPosition = fs;
            }
            break;
        case 2:
            sd = buffer[3] - 0x30;
            parsedPosition = (10 * fs) + sd;
            if ((parsedPosition < 0) || parsedPosition >= CALENDAR_LENGTH) {
                parsedPosition = -1;
            }
            break;
        case 3:
            sd = buffer[3] - 0x30;
            th = buffer[4] - 0x30;
            parsedPosition = (100 * fs)+ (10 * sd) + th;
            if ((parsedPosition < 0) || parsedPosition >= CALENDAR_LENGTH) {
                parsedPosition = -1;
            }
            break;
        default:
            break;
    }
    return parsedPosition;
}

static bool parseEventParameters(uint8_t *buffer) {
    paramsOk = false;
    event_command = buffer[2] - 0x30;
    event_param = buffer[4] - 0x30;
    event_color = buffer[6] - 0x30;

    if (event_command == 0 || event_command == 1) {
        if (event_param >= 0 && event_param <= 7) {
            if (event_color >= 0 && event_color <= 3) {
                paramsOk = true;
            }
        }
    }
    return paramsOk;
}

static void prepareNewEvent(app_event_t *ev) {
    event_tm.tm_hour = event_time[0];
    event_tm.tm_min = event_time[1];
    event_tm.tm_mday = event_date[0];
    event_tm.tm_mon = event_date[1];
    event_tm.tm_year = event_date[2];
    ev->cmd = event_command;
    ev->param = event_param;
    ev->color = event_color;
    ev->time = event_tm;
}

static void generateEventMessage(app_event_t *ev) {
    event_message[2] = ev->cmd + 0x30;
    event_message[3] = SPACE;
    event_message[4] = ev->param + 0x30;
    event_message[5] = SPACE;
    event_message[6] = ev->color + 0x30;
    event_message[7] = SPACE;
    aux = ev->time;
    codeCurrentTime(event_message, &aux, 8);
    event_message[13] = SPACE;
    codeCurrentDate(event_message, &aux, 14);

    // Add Line Feed and Carriage return at end of buffer
    event_message[0] = 0xA;
    event_message[1] = 0xD;
}

static void deleteEvent(uint8_t pos) {
    calendario[pos] = (app_event_t){.cmd = EMPTY_SLOT_VALKYRIE};
}

static bool isGreaterThan(struct tm *time1, struct tm *time2) {
    bool isGreater = false;
    if (time1->tm_year > time2->tm_year) {
        isGreater = true;
    } else if (time1->tm_year == time2->tm_year) {
        if (time1->tm_mon > time2->tm_mon) {
            isGreater = true;
        } else if (time1->tm_mon == time2->tm_mon) {
            if (time1->tm_mday > time2->tm_mday) {
                isGreater = true;
            } else if (time1->tm_mday == time2->tm_mday) {
                if (time1->tm_hour > time2->tm_hour) {
                    isGreater = true;
                } else if (time1->tm_hour == time2->tm_hour) {
                    if (time1->tm_min >= time2->tm_min) {
                        isGreater = true;
                    }
                }
            }
        }
    }
    return isGreater;
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


bool isBlank(uint8_t pos) {
    empty = false;
    if (pos >= 0 && pos < CALENDAR_LENGTH) {
        empty = calendario[pos].cmd == EMPTY_SLOT_VALKYRIE;
    }
    return empty;
}

void initialize_C() {
    int i;
    for (i = 0; i < CALENDAR_LENGTH; i++) {
        calendario[i] = (app_event_t){.cmd = EMPTY_SLOT_VALKYRIE};
    }
}

bool deleteEventInCalendar(uint8_t *buffer) {
    deleted = false;
    pos_range = getCalendarPositionRange();
    parsedPosition = parsePositionId(buffer, pos_range);
    if (parsedPosition != -1) {
        deleteEvent(parsedPosition);
        deleted = true;
    }
    return deleted;
}

bool addEventToCalendar(uint8_t *buffer) {
    added = false;
    int i;

    if (parseEventParameters(buffer)) {
        for (i = 0; i < CALENDAR_LENGTH; i++) {
            if (isBlank(i)) {
                parseTime(buffer, 8, event_time);
                parseDate(buffer, 14, event_date);
                prepareNewEvent(&new_event);
                calendario[i] = new_event;
                added = true;
                break;
            }
        }
    }
    return added;
}

bool getEventListAndSend(uint8_t length) {
    bool hasEnded = false;
    if (length <= CALENDAR_LENGTH) {
        if (position < length) {
            generateEventMessage(&calendario[position]);
            putUSBUSART(event_message, 22);
            position++;

        } else {
            position = 0;
            hasEnded = true;
        }
    }
    return hasEnded;
}

void checkEvents(struct tm *currentTime, void(*function)(void)) {
    int i;
    ws2812_t color;

    for (i = 0; i < CALENDAR_LENGTH; i++) {

        if (isBlank(i) == false) {

            if (isGreaterThan(currentTime, &calendario[i].time)) {
                event_param = calendario[i].param;
                event_color = calendario[i].color;
                color = getColor(event_color);
                if (calendario[i].cmd == 1) {
                    leds[event_param] = color;
                }
                if (calendario[i].cmd == 0) {
                    leds[event_param] = BLACK;
                }
                function();
                deleteEvent(i);
            }
        }
    }
}






/* *****************************************************************************
 End of File
 */
