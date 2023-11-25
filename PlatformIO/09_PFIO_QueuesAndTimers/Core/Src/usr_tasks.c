/*##############################################################################################################################################*/
/*INCLUDES______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

#include "main.h"

/*##############################################################################################################################################*/
/*GLOBALS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

const char *invalidMsg = "/-----------Invalid Option-----------/\n";

/*##############################################################################################################################################*/
/*DEFINES_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*EXTERNS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

extern input_zInputCommand_t inputCmd;
extern UART_HandleTypeDef huart2;

/*##############################################################################################################################################*/
/*TYPEDEFS/STRUCTS/ENUMS________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*FUNCTION DECLARATIONS_________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

void usrtsk_ProcessCmd( input_zInputCommand_t *cmd );
void usrtsk_ExtractCmd( input_zInputCommand_t *cmd );

/*##############################################################################################################################################*/
/*FUNCTIONS_____________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

/**
 * @brief Function to Create Menu GUI String and wait for User Input
 * 
 * @param param 
 */
void usrtsk_MenuTask( void *param )
{
    input_zInputCommand_t *cmd;
    uint8_t usrChoice;
	const char* msg_menu = "========================\r\n"
							"|         Menu         |\r\n"
							"========================\r\n"
								"LED effect    ----> 0\r\n"
								"Date and time ----> 1\r\n"
								"Exit          ----> 2\r\n"
								"Enter your choice here : ";
    currAppState = sMainMenu;
    while( 1 )
    {
        xQueueSend( queueHdl_zOptionsPrint, &msg_menu, portMAX_DELAY );
        xTaskNotifyWait( 0, 0, ( uint32_t * )&cmd, portMAX_DELAY );

        if( cmd->len == 1 )
        {
            usrChoice = cmd->payload - 48; //ASCII to Decimal
            switch( usrChoice )
            {
                case 0: //LED Effect
                {
                    currAppState = sLEDEffect;
                    xTaskNotify( tskHdl_zLEDTask, 0, eNoAction );
                    break;
                }
                case 1: //RTC Menu
                {
                    currAppState = sRTCMenu;
                    xTaskNotify( tskHdl_zRTCTask, 0, eNoAction );
                    break;
                }
                case 2: //Exit
                {
                    break;
                }
                default:
                {
                    xQueueSend( queueHdl_zOptionsPrint, &invalidMsg, portMAX_DELAY );
                    continue; //Jump to next iteration
                }
            }
        }
        else
        {
            //Invalid Input
            xQueueSend( queueHdl_zOptionsPrint, &invalidMsg, portMAX_DELAY );
        }

        /* Wait to run again until it has been notified */
        xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY );
    }
}

/**
 * @brief Function to handle User Input
 * 
 * @param param 
 */
void usrtsk_InputHandleTask( void *param )
{
    BaseType_t status;
    while( 1 )
    {
        /*Wait for Notification*/
        status = xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY );
        if( status == pdTRUE )
        {
            /*Notifcation Has been received*/
            usrtsk_ProcessCmd( &inputCmd );
        }

    }
}

/**
 * @brief Function to Print the GUI of Main, LED, and RTC
 * 
 * @param param 
 */
void usrtsk_PrintGUITask( void *param )
{
    while( 1 )
    {  
        vTaskDelay( portMAX_DELAY );
        
    }
}

/**
 * @brief Function to create the LED GUI and wait for User input
 * 
 * @param param 
 */
void usrtsk_LEDTask( void *param )
{
    char *usrChoice;
    input_zInputCommand_t *cmd;
    const char* msg_led = "========================\r\n"
						  "|      LED Effect     |\r\n"
						  "========================\r\n"
						  "(none,e1,e2,e3,e4)\r\n"
						  "Enter your choice here : ";
    
    while( 1 )
    {
        /* Wait for Activation Notification From Main Task */
        xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY );

        /*Print LED Menu and wait for input*/
        xQueueSend( queueHdl_zOptionsPrint, &msg_led, portMAX_DELAY );
        xTaskNotifyWait( 0, 0, ( uint32_t * )&cmd, portMAX_DELAY );
        usrChoice = cmd->payload;

        if( cmd->len <= 4 )
        {
            if( strcmp( usrChoice, "none" ) == 0 )
            {
                led_LEDEffectStop( );
            }
            else if( strcmp( usrChoice, "e1" ) == 0 )
            {
                led_LEDEffect( 1 );
            }
            else if( strcmp( usrChoice, "e2" ) == 0 )
            {
                led_LEDEffect( 2 );
            }
            else if( strcmp( usrChoice, "e3" ) == 0 )
            {
                led_LEDEffect( 3 );
            }
            else if( strcmp( usrChoice, "e4" ) == 0 )
            {
                led_LEDEffect( 4 );
            }
            else
            {
                xQueueSend( queueHdl_zOptionsPrint, &invalidMsg, portMAX_DELAY );
            }
        }
        else
        {
            xQueueSend( queueHdl_zOptionsPrint, &invalidMsg, portMAX_DELAY );
        }

        /* Update State Variable */
        currAppState = sMainMenu;

        /*Notify Main Task*/
        xTaskNotify( tskHdl_zMenuTask, 0, eNoAction );
    }
}

/**
 * @brief Function to create the RTC GUI and wait for User Input
 * 
 * @param param 
 */
void usrtsk_RTCTask( void *param )
{
    while( 1 )
    {
        vTaskDelay( portMAX_DELAY );
    }
}

void usrtsk_ProcessCmd( input_zInputCommand_t *cmd )
{
    
    usrtsk_ExtractCmd( cmd );

    switch( currAppState )
    {
        case sMainMenu:
        {
            xTaskNotify( tskHdl_zMenuTask, ( uint32_t )cmd, eSetValueWithOverwrite );
            break;
        }
        case sLEDEffect:
        {
            xTaskNotify( tskHdl_zLEDTask, ( uint32_t )cmd, eSetValueWithOverwrite );
            break;
        }
        case sRTCMenu:
        case sRTCTimeConfig:
        case sRTCDateConfig:
        case sRTCReport:
        {
            xTaskNotify( tskHdl_zRTCTask, ( uint32_t )cmd, eSetValueWithOverwrite );
            break;
        }
    }

}

/**
 * @brief Function to extract the data bytes from the Queue and form a Command
 * 
 * @param cmd 
 */
void usrtsk_ExtractCmd( input_zInputCommand_t *cmd )
{
    uint8_t msg;
    BaseType_t numMsgs;

    numMsgs = uxQueueMessagesWaiting( queueHdl_zInputData );
    if( numMsgs == 0 )
    {
        /*No Messages in Queue*/
        return;
    }
    else
    {
        /*Messages in Queue*/
        for( uint8_t i = 0; i < numMsgs; i++ )
        {
            xQueueReceive( queueHdl_zInputData, &msg, 0 );
            taskENTER_CRITICAL( );
            cmd->payload[ i ] = msg;
            taskEXIT_CRITICAL( );
        }
        taskENTER_CRITICAL( );
        cmd->len = numMsgs - 1;
        cmd->payload[ numMsgs - 1 ] = '\0';
        taskEXIT_CRITICAL( );
    }
}