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
	uint32_t cmdAddr;
    input_zInputCommand_t *cmd;
	const char* msg_menu = "========================\r\n"
							"|         Menu         |\r\n"
							"========================\r\n"
								"LED effect    ----> 0\r\n"
								"Date and time ----> 1\r\n"
								"Exit          ----> 2\r\n"
								"Enter your choice here : \n";
    currAppState = sMainMenu;
    HAL_UART_Transmit( &huart2, ( uint8_t * )msg_menu, strlen( msg_menu ), HAL_MAX_DELAY );
    while( 1 )
    {
        //xQueueSend( queueHdl_zOptionsPrint, &msg_menu, portMAX_DELAY );
        xTaskNotifyWait( 0, 0, &cmdAddr, portMAX_DELAY );
        cmd = ( input_zInputCommand_t * )cmdAddr;
        if( cmd->len == 1 )
        {
            printf( "Hello There\r\n" );
        }

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
    while( 1 )
    {
        vTaskDelay( portMAX_DELAY );
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
            xTaskNotify( tskHdl_LEDTask, ( uint32_t )cmd, eSetValueWithOverwrite );
            break;
        }
        case sRTCMenu:
        case sRTCTimeConfig:
        case sRTCDateConfig:
        case sRTCReport:
        {
            xTaskNotify( tskHdl_RTCTask, ( uint32_t )cmd, eSetValueWithOverwrite );
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