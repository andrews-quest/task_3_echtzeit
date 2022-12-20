
/* Standard includes. */
#include <time.h>
/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include <ncurses.h>
#include <stdlib.h>

#define N_TASKS 10
#define mainKEYBOARD_TASK_PRIORITY          ( tskIDLE_PRIORITY + 0 )

/*
 * The tasks as described in the comments at the top of this file.
 */
static void vKeyHitTask( void *pvParameters );
static void vIncrementFunktion(void *pvParameters);
static void vGlobalManager(void *pvParameters);

xSemaphore = xSemaphoreCreateCounting(10,0);

/* A software timer that is started from the tick hook. */
bool quit = false;
int TaskData[N_TASKS];
u_int32_t * pvGlobalVariable;
bool bStop = pdFALSE;


/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_rtos( void )
{
	for(int i=1;i<N_TASKS;i++){
		xTaskCreate( vIncrementFunktion,			    /* The function that implements the task. */
			"Increment Global Variable", 	
			configMINIMAL_STACK_SIZE, 		/* The size of the stack to allocate to the task. */
			(void*) i, 					  	    /* The parameter passed to the task - not used in this simple case. */
			mainKEYBOARD_TASK_PRIORITY,     /* The priority assigned to the task. */
			NULL );
		mvprintw(10, 0, "Task %d", 0);
	}

	xTaskCreate(vGlobalManager, "Global Manager", configMINIMAL_STACK_SIZE, NULL, mainKEYBOARD_TASK_PRIORITY, NULL );
	
	xTaskCreate(vKeyHitTask, "Keyboard", configMINIMAL_STACK_SIZE, NULL, mainKEYBOARD_TASK_PRIORITY, NULL );

	pvGlobalVariable = (u_int32_t*)pvPortMalloc(sizeof(u_int32_t));
	*pvGlobalVariable = 0;

	vTaskStartScheduler();


	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for( ;; ){
	};
}

/*
void vRunningIndicator(u_int32_t ulTaskID){
	for(;;)
}
*/
static void vGlobalManager(void *pvParameters){
	for( ;; ){
		taskENTER_CRITICAL();
		mvprintw(15, 0, "Global Variable: %d", *pvGlobalVariable);
		refresh();
		taskEXIT_CRITICAL();

		if(*pvGlobalVariable == 1000){
			bStop = pdTRUE;
		}
	};
}

/*-----------------------------------------------------------*/
static void vIncrementFunktion(void *pvParameters) {
	u_int32_t task_number = (u_int32_t) pvParameters;
	u_int32_t localVar;

	//mvprintw(25+task_number, task_number*6, "Task finished: %d", localVar);

	taskENTER_CRITICAL();
	mvprintw(5+task_number, 0, "Task %d", task_number);
	mvprintw(5+task_number, 15, "running...");
	refresh;
	taskEXIT_CRITICAL();

	for(;;){
		localVar = *pvGlobalVariable;
		vTaskDelay(pdMS_TO_TICKS(task_number*100));
		localVar = localVar+1;
		vTaskDelay(pdMS_TO_TICKS(task_number*100));
		*pvGlobalVariable = localVar;


	}

	if (bStop == pdTRUE){
		taskENTER_CRITICAL();
		mvprintw(5+task_number, 15, "Task finished: %d", localVar);
		refresh;
		taskEXIT_CRITICAL();
	}
}


static void vKeyHitTask(void *pvParameters) {
	int k = 0;

	//printw("Press a key to turn the backlight on.\n");
    attron(A_BOLD);
    attron(COLOR_PAIR(1));
    mvprintw(2, 20, "Press 'q' to quit application !!!\n");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
	refresh();

	/* Ideally an application would be event driven, and use an interrupt to process key
	 presses. It is not practical to use keyboard interrupts when using the FreeRTOS 
	 port, so this task is used to poll for a key press. */
	for (;;) {
		/* Has a key been pressed? */
		k = getch();
		switch(k) {
			case 113: { //'q'
				quit = true;
				endwin();
				exit(2);
			}
			case 160: { //spacebar
 				main_rtos();
			}
			default:
				break;
		}
		mvprintw(4, 0, " ");
	}
}

/*-----------------------------------------------------------*/


