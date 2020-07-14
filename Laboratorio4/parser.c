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

#include <stdio.h>
#include <time.h>
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
    int rawDay;
    int rawMon;
    int rawYear;
    int rawHour;
    int rawMin;

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

void codeCurrentTime(uint8_t *loader, struct tm *tc, uint8_t start){
    rawHour = tc->tm_hour;
    rawMin = tc->tm_min;
    if (rawHour < 10) {
        loader[start] = 0x30;
        loader[start + 1] = rawHour + 0x30;
    } else {
        loader[start] = (rawHour / 10) + 0x30;
        loader[start + 1] = (rawHour % 10) + 0x30;
    }
    // 58 = ascii code for colon
    loader[start + 2] = 58;
    if (rawMin < 10) {
        loader[start + 3] = 0x30;
        loader[start + 4] = rawMin + 0x30;
    } else {
        loader[start + 3] = (rawMin / 10) + 0x30;
        loader[start + 4] = (rawMin % 10) + 0x30;
    }
}

void codeCurrentDate(uint8_t *buffer, struct tm *tc, uint8_t start){
    rawDay = tc->tm_mday;
    rawMon = tc->tm_mon;
    rawYear = tc->tm_year;
    if (rawDay < 10) {
        buffer[start] = 0x30;
        buffer[start + 1] = rawDay + 0x30;
    } else {
        buffer[start] = (rawDay / 10) + 0x30;
        buffer[start + 1] = (rawDay % 10) + 0x30;
    }
    buffer[start + 2] = '/';
    if (rawMon < 10) {
        buffer[start + 3] = 0x30;
        buffer[start + 4] = rawMon + 0x30;
    } else {
        buffer[start + 3] = (rawMon / 10) + 0x30;
        buffer[start + 4] = (rawMon % 10) + 0x30;
    }
    buffer[start + 5] = '/';
    rawYear = rawYear % 100;
    if (rawYear < 10) {
        buffer[start + 6] = 0x30;
        buffer[start + 7] = rawYear + 0x30;
    } else {
        buffer[start + 6] = (rawYear / 10) + 0x30;
        buffer[start + 7] = (rawYear % 10) + 0x30;
    }
}


 int parseTime(uint8_t *buffer, uint8_t start, uint8_t *response){
    int decenaHora = buffer[start] - 0x30;
    int unidadHora = buffer[start + 1] - 0x30;
    int decenaMin = buffer[start + 3] - 0x30;
    int unidadMin = buffer[start + 4] - 0x30;

    response[0] = (decenaHora * 10) + unidadHora;
    response[1] = (decenaMin * 10) + unidadMin;
 }
 
 int parseDate(uint8_t *buffer, uint8_t start, uint8_t *response) {
    int decenaDia = buffer[start] - 0x30;
    int unidadDia = buffer[start + 1] - 0x30;
    int decenaMes = buffer[start + 3] - 0x30;
    int unidadMes = buffer[start + 4] - 0x30;
    int decenaAnio = buffer[start + 6] - 0x30;
    int unidadAnio = buffer[start + 7] - 0x30;

    response[0] = decenaDia * 10 + unidadDia;
    response[1] = decenaMes * 10 + unidadMes;
    response[2] = decenaAnio * 10 + unidadAnio;
}
 
void decodeCurrentTime(uint8_t *buffer, struct tm *tc) {
    uint8_t response[2];
    parseTime(buffer, 2, response);
    tc->tm_hour = response[0];
    tc->tm_min = response[1];
}


/* *****************************************************************************
 End of File
 */
