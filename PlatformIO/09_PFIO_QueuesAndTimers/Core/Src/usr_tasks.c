/*##############################################################################################################################################*/
/*INCLUDES______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

#include "main.h"

/*##############################################################################################################################################*/
/*GLOBALS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

const char *invalidMsg = "\n/-----------Invalid Option-----------/\n";
const char* msg_menu = "\n\n========================\r\n"
							"|         Menu         |\r\n"
							"========================\r\n"
								"LED effect    ----> 0\r\n"
								"Date and time ----> 1\r\n"
								"Exit          ----> 2\r\n"
								"Enter your choice here : ";
const char* msg_led = "\n\n========================\r\n"
						  "|      LED Effect       |\r\n"
						  "========================\r\n"
						  "(none,e1,e2,e3,e4)\r\n"
						  "Enter your choice here : ";
const char* msg_rtc1 = "\n\n========================\r\n"
							"|         RTC          |\r\n"
							"========================\r\n";

const char* msg_rtc2 = "Configure Time            ----> 0\r\n"
                        "Configure Date            ----> 1\r\n"
                        "Enable reporting          ----> 2\r\n"
                        "Exit                      ----> 3\r\n"
                        "Enter your choice here : ";


const char *msg_rtc_hh = "Enter hour(1-12):";
const char *msg_rtc_mm = "Enter minutes(0-59):";
const char *msg_rtc_ss = "Enter seconds(0-59):";

const char *msg_rtc_dd  = "Enter date(1-31):";
const char *msg_rtc_mo  ="Enter month(1-12):";
const char *msg_rtc_dow  = "Enter day(1-7 sun:1):";
const char *msg_rtc_yr  = "Enter year(0-99):";

const char *msg_conf = "Configuration successful\n";
const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";                         

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
void USRTSK_MenuTask( void *param )
{
    input_zInputCommand_t *cmd;
    uint8_t usrChoice;
    currAppState = sMainMenu;
    while( 1 )
    {
        xQueueSend( queueHdl_zOptionsPrint, &msg_menu, portMAX_DELAY );
        xTaskNotifyWait( 0, 0, ( uint32_t * )&cmd, portMAX_DELAY );

        if( cmd->len == 1 )
        {
            usrChoice = cmd->payload[ 0 ] - 48; //ASCII to Decimal
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
            continue;
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
void USRTSK_InputHandleTask( void *param )
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
void USRTSK_PrintGUITask( void *param )
{
    uint32_t msg;
    while( 1 )
    {  
        xQueueReceive( queueHdl_zOptionsPrint, &msg, portMAX_DELAY );
        HAL_UART_Transmit( &huart2, ( uint8_t * )msg, strlen( ( char * )msg ), HAL_MAX_DELAY );
    }
}

/**
 * @brief Function to create the LED GUI and wait for User input
 * 
 * @param param 
 */
void USRTSK_LEDTask( void *param )
{
    char *usrChoice;
    input_zInputCommand_t *cmd;
    
    while( 1 )
    {
        /* Wait for Activation Notification From Main Task */
        xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY );

        /*Print LED Menu and wait for input*/
        xQueueSend( queueHdl_zOptionsPrint, &msg_led, portMAX_DELAY );
        xTaskNotifyWait( 0, 0, ( uint32_t * )&cmd, portMAX_DELAY );
        usrChoice = (char *)cmd->payload;

        if( cmd->len <= 4 )
        {
            if( strcmp( usrChoice, "none" ) == 0 )
            {
                LED_LEDEffectStop( );
            }
            else if( strcmp( usrChoice, "e1" ) == 0 )
            {
                LED_LEDEffect( 1 );
            }
            else if( strcmp( usrChoice, "e2" ) == 0 )
            {
                LED_LEDEffect( 2 );
            }
            else if( strcmp( usrChoice, "e3" ) == 0 )
            {
                LED_LEDEffect( 3 );
            }
            else if( strcmp( usrChoice, "e4" ) == 0 )
            {
                LED_LEDEffect( 4 );
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
void USRTSK_RTCTask( void *param )
{
    input_zInputCommand_t *cmd;
    while( 1 )
    {
        /*Wait for notification*/
        xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY );

        /*Print RTC Menu and wait for input*/
        xQueueSend( queueHdl_zOptionsPrint, &msg_rtc1, portMAX_DELAY );
        RTC_ShowTimeDate( );
        xQueueSend( queueHdl_zOptionsPrint, &msg_rtc2, portMAX_DELAY );

        while( currAppState != sMainMenu )
        {
            xTaskNotifyWait( 0, 0, ( uint32_t * )&cmd, portMAX_DELAY );
        }

    }
}

/**
 * @brief Function to process the User Command and send it to the respective Task
 * 
 * @param cmd Pointer to User Command
 */
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
 * @param cmd Pointer to User Command
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