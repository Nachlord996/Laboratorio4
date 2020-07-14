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
#include "FreeRTOS.h"
#include "task.h"

#include <time.h>
#include <stdbool.h>
#include "freeRTOS/include/semphr.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "show_ui.h"
#include "LEDs_RGB/WS2812.h"
#include "calendar.h"
#include "tasks.h"

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
static uint32_t tiempoOn = 400;
static uint32_t tiempoOff = 800;



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
void blinkLED(void *p_param) {
    for (;;) {
        LEDA_SetHigh();
        vTaskDelay(pdMS_TO_TICKS(tiempoOn));
        LEDA_SetLow();
        vTaskDelay(pdMS_TO_TICKS(tiempoOff));
    }
}

void checkUSBStatus(void *p_param) {
    for (;;) {
        CDCTxService();
        if ((USBGetDeviceState() >= CONFIGURED_STATE) && !USBIsDeviceSuspended()) {
            xSemaphoreGive(semaphore);
        }
    }
}

void showMenu(void *p_param) {
    for (;;) {
        xSemaphoreTake(semaphore,pdMS_TO_TICKS(400));
        showInterface(&time_holder);
    }
}

void updateRTCC (void *p_param){
    for(;;){
        RTCC_TimeGet(&time_holder);
    }
}

void executeEvents(void *p_param){
    for(;;){
        checkEvents(&time_holder,&RGB_LED_eventHandler);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void initializeCalendar(void *p_param){
    for(;;){
        initialize_C();
        vTaskDelete(xTaskInitHandle);
    }
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

void load_Tasks(){
    // Para proteger el recurso compartido calendario, se podría crear un semáforo MUTEX donde las distintas 
    // sesiones críticas sean: Leer del Calendario, Agregar o Quitar del Calendario. Leer de array de LEDS
    // Modificar array de LEDS
    task_constructor task1 = (task_constructor){.function = &initializeCalendar, .name = "Initialize", .handler = xTaskInitHandle, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 2, .stack_memory = configMINIMAL_STACK_SIZE };
    task_constructor task2 = (task_constructor){.function = &blinkLED, .name = "Blink_LED_Task", .handler = NULL, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 1, .stack_memory = configMINIMAL_STACK_SIZE };
    task_constructor task3 = (task_constructor){.function = &checkUSBStatus, .name = "Check_USB_Task", .handler = NULL, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 1, .stack_memory = configMINIMAL_STACK_SIZE };
    task_constructor task4 =  (task_constructor){.function = &showMenu, .name = "Show_Interface_Task", .handler = NULL, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 1, .stack_memory = configMINIMAL_STACK_SIZE };
    task_constructor task5 =  (task_constructor){.function = &updateRTCC, .name = "Update_Time_Task", .handler = NULL, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 1, .stack_memory = configMINIMAL_STACK_SIZE };
    task_constructor task6 = (task_constructor){.function = &executeEvents, .name = "Execute_Events", .handler = NULL, 
            .parameters = NULL, .priority = tskIDLE_PRIORITY + 2, .stack_memory = configMINIMAL_STACK_SIZE };
            
    task_constructor tasks[TOTAL_TASKS] = {task1,task2,task3,task4,task5,task6};
    memcpy(project_tasks, tasks, sizeof(tasks)); 
}


/* *****************************************************************************
 End of File
 */
