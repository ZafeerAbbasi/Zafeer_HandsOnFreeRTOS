/*##############################################################################################################################################*/
/*INCLUDES______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

#include "main.h"

/*##############################################################################################################################################*/
/*GLOBALS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

GPIO_GPIOPinPortPair_t ledPortPinPair[ 4 ] = 
{
    { LD3_GPIO_Port, LD3_Pin },
    { LD5_GPIO_Port, LD5_Pin },
    { LD6_GPIO_Port, LD6_Pin },
    { LD4_GPIO_Port, LD4_Pin }
};

/*##############################################################################################################################################*/
/*DEFINES_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*EXTERNS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*TYPEDEFS/STRUCTS/ENUMS________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*FUNCTION DECLARATIONS_________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*FUNCTIONS_____________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

void LED_LEDEffectStop( void )
{
    LED_TurnOffAllLEDs( );
    for( int i = 0; i < 4; i++ )
    {
        xTimerStop( timerHdl_zLEDTimer[ i ], portMAX_DELAY );
    }
}

void LED_LEDEffect( int LedNum )
{
    LED_LEDEffectStop( );
    xTimerStart( timerHdl_zLEDTimer[ LedNum - 1 ], portMAX_DELAY );
}

void LED_TurnOffAllLEDs( void )
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_RESET);
}

void LED_TurnOnAllLEDs( void )
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_SET);
}

void LED_TurnOnOddLEDs( void ) 
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD6_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD5_Pin,GPIO_PIN_RESET);
}

void LED_TurnOnEvenLEDs( void )
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD6_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD5_Pin,GPIO_PIN_SET);
}

void LED_LEDEffect1( void )
{
    static uint8_t ledState = 0;
    ledState = !ledState;
    if( ledState )
    {
        LED_TurnOnAllLEDs( );
    }
    else
    {
        LED_TurnOffAllLEDs( );
    }
}

void LED_LEDEffect2( void )
{
    static uint8_t ledState = 0;
    ledState = !ledState;
    if( ledState )
    {
        LED_TurnOnOddLEDs( );
    }
    else
    {
        LED_TurnOnEvenLEDs( );
    
    }
}

void LED_LEDEffect3( void )
{
    static int ledState = 0;
    for( int i = 0; i < 4; i++ )
    {
        if( i == ledState )
        {
            HAL_GPIO_WritePin( ledPortPinPair[ i ].port, ledPortPinPair[ i ].pin, GPIO_PIN_SET );
        }
        else
        {
            HAL_GPIO_WritePin( ledPortPinPair[ i ].port, ledPortPinPair[ i ].pin, GPIO_PIN_RESET );
        }
    }
    
    if( ledState == 3 )
    {
        ledState = 0;
    }
    else
    {
        ledState++;
    }
}

void LED_LEDEffect4( void )
{
    static int ledState = 3;
    for( int i = 0; i < 4; i++ )
    {
        if( i == ledState )
        {
            HAL_GPIO_WritePin( ledPortPinPair[ i ].port, ledPortPinPair[ i ].pin, GPIO_PIN_SET );
        }
        else
        {
            HAL_GPIO_WritePin( ledPortPinPair[ i ].port, ledPortPinPair[ i ].pin, GPIO_PIN_RESET );
        }
    }
    
    if( ledState == 0 )
    {
        ledState = 3;
    }
    else
    {
        ledState--;
    }
}