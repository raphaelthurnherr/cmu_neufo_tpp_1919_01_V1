#include <U8x8lib.h>                    // U8x8 library for OLED display
#include "tpp_neufo_1919_01_V1_board.h"
#include "math.h"


// TEXT DEFINITON FOR GFX DISPLAY MENU
#define TXT_TEST_AOUT_1 "AOUT1  - CN7 "
#define TXT_TEST_AOUT_2 "AOUT2  - CN8 "
#define TXT_TEST_LED_RED "PWM LED RED  "
#define TXT_TEST_LED_GREEN "PWM LED GREEN"
#define TXT_TEST_LED_BLUE "PWM LED BLUE "
#define TXT_TEST_PWM_SERVO "PWM SERVO CN9"


#define ARDUINO_ADC_RESOLUTION 4.8828  // => 5000mV / 1024
//#define ARDUINO_ADC_RESOLUTION 3.2226  // => 3300mV / 1024

// THE DEFINITION OF THE OUTPUTS NAME CORRESPONDING TO THE ARDUINO
// PIN CAN BE FOUND IN THE "tpp_neufo_1919_01_V1_board.h" HEADER FILE.

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
board_1919_01_V01 tpp_1912_board;

char get_actual_menu_on_display_from_pot_RV1(int pot_RV1_value);
void gfx_display_selected_menu(char menu);
void gfx_display_menu0_status_data(void);
float convert_Deg_To_Rad(float x);

int DAC_value_for_sinus = 0;
float ADCinputVoltage_mV = 0;
float I_R13_R14_mA ;
int sin_wave_counter, triangle_wave_counter, pwm_led_counter=255;
char menu_selected_by_pot_RV1 = 0;
float battery_BT1_voltage_mV = 0;
int potentiometer_RV1_ADC_value = 0;
int potentiometer_RV1_level = 0;
int lickport_CN2_state = 0;
int trigger_CN3_State = 0;
int button_SW1_State = 0;
char actual_menu_on_display=0;
int displayRefreshTime=0;

/**
 * @brief Initial setup function for Arduino
 *  - Configuring the Serial port for monitoring
 *  - Initializing the TPP board DAC integrated circuit on I2C bus
 *  - Initializing the OLED Display and display the "welcome message"
 *  - Turn-off all the LED drived by PWM (LED are active on low level, see schematics)
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // TPP board initialization
  tpp_1912_board.begin();

  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.drawString(0, 0, "TPP 2020 Demo ");
  u8x8.drawString(0, 2, "RTH 06.02.2020");

  delay(3000);
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, "LCK:   TRG:  ");
  u8x8.drawString(0, 2, "BTN:   BAT:  ");

  analogReference(DEFAULT);             // default voltage reference for ADC = 5V

  analogWrite(RGB_LED_RED, 255);
  analogWrite(RGB_LED_GREEN, 255);
  analogWrite(RGB_LED_BLUE, 255);
}


/**
 * @brief MAIN LOOP PROGRAM
 * 
 *  Get the DIN and AIN arduino value and convert it to an usable unit:
 *  - Potentiometer is converted to 0..100% for output test selection
 *  - Battery voltage is calculate with the divider bridge (R13 = 56k, R14 = 43K)
 *  - The input trigger is not use, only display on the screen
 *  - The push button input is use for a sychronized test of all de output.
 *  - The Lickport  input is not use, only display on the screen
 * 
 *  With the potentiometer test selection, the program put out the signal on the corresponding output:
 *   - Channel 1 generate a sinus waveform
 *   - Channel 2 generate a triangle waveform
 *   - LED RED is drived with PWM
 *   - LED GREEN is drived with PWM
 *   - LED BLUE is drived with PWM
 *   - SERVO MOTOR output is tested with PWM
 */ 

void loop() {
  // ------ READING OF ANALOG INPUTS POTENTIOMETER AND BATTERY VOLTAGE -------
  // -------------------------------------------------------------------------
  // Get the register ADC value for potentiometer measuring voltage (No meaning)
  potentiometer_RV1_ADC_value = analogRead(RV1_POT_PIN);
  // Convert the potentiometer ADC value to level in %
  potentiometer_RV1_level = map(potentiometer_RV1_ADC_value, 0, 750, 100, 0);

  // Convert th ADC input register to voltage (5V/1024 = 4.8828 mV of resolution)
  ADCinputVoltage_mV = analogRead(BT1_BATTERY_PIN) * ARDUINO_ADC_RESOLUTION;
  // Divider bridge current calculation (I=U_R14/R14)
  I_R13_R14_mA = ADCinputVoltage_mV / (float)56000;
  // Make the sum of the voltage for U_R14 and U_R13(=R13 * IR13) to know the battery voltage;
  battery_BT1_voltage_mV = ADCinputVoltage_mV + ((float)43000 * I_R13_R14_mA);

  // ------ READING OF DIGITAL INPUTS -------
  // -------------------------------------------------------------------------
  // Get the digital input state for lickport, trigger and push-button
  lickport_CN2_state=!digitalRead(CN2_LICKPORT_PIN);
  trigger_CN3_State=digitalRead(CN3_TRIG_PIN);
  button_SW1_State=digitalRead(SW1_BUTTTON_PIN);

  // ------ DISPLAY MANAGEMENT AND OUTPUT TEST SELECTION FROM POTENTIOMETER-------
  // ----------------------------------------------------------------------------
  // Get the test menu corresponding to the potentiometer value
  menu_selected_by_pot_RV1 = get_actual_menu_on_display_from_pot_RV1(potentiometer_RV1_level);

  // Refresh the display only when the pototiometer was moving
  if(actual_menu_on_display != menu_selected_by_pot_RV1){
    // Display the new selected menu
    gfx_display_selected_menu(menu_selected_by_pot_RV1);
    actual_menu_on_display = menu_selected_by_pot_RV1;

    // Reset LED to OFF (LOW level LED driver) when menu is changing
    analogWrite(RGB_LED_RED, 255);
    analogWrite(RGB_LED_GREEN, 255);
    analogWrite(RGB_LED_BLUE, 255);
  }

  // ------ MAKE THE TEST OF ALL THE OUTPUT WHEN THE BUTTON IS PRESSED-------
  // ------ IF NOT, ONLY THE SELECTED OUTPUT BY THE MENU IS MADE ------------
  if(button_SW1_State){
    /*
    DAC_value_for_sinus = round(((sin(convert_Deg_To_Rad((float)sin_wave_counter))+1)*2300));
    tpp_1912_board.setAnalogOutput_mV(CHANNEL_1, DAC_value_for_sinus);
    */
    tpp_1912_board.setAnalogOutput_value(CHANNEL_2,triangle_wave_counter );
    

    analogWrite(RGB_LED_RED, pwm_led_counter);
    analogWrite(RGB_LED_GREEN, pwm_led_counter);
    analogWrite(RGB_LED_BLUE, pwm_led_counter);
  }else
  {
    switch(actual_menu_on_display){
      case 0 : if(displayRefreshTime>=100){
                  gfx_display_menu0_status_data();
                  displayRefreshTime=0;
                }
                  break;
      case 1 : DAC_value_for_sinus = round(((sin(convert_Deg_To_Rad((float)sin_wave_counter))+1)*2300));
               tpp_1912_board.setAnalogOutput_mV(CHANNEL_1, DAC_value_for_sinus); break;
      case 2 : tpp_1912_board.setAnalogOutput_value(CHANNEL_2, triangle_wave_counter); break;
      case 3 : analogWrite(RGB_LED_RED, pwm_led_counter); break;
      case 4 : analogWrite(RGB_LED_GREEN, pwm_led_counter); break;
      case 5 : analogWrite(RGB_LED_BLUE, pwm_led_counter); break;
      //case 6 : CN9_servomotor.writeMicroseconds(1500); break;
      default: break;
    }
  }


// COUNTER USE FOR SINUS WAVEFORM 
// COUNTING FROM 0 TO 360 DEGRES
// THE VOLTAGE VALUE FOR EACH DEGREE IS CALCULATED DURING THE TEST OUTPUT
  sin_wave_counter+=5;
  if(sin_wave_counter>360){
    sin_wave_counter=0;
  }

// LINEAR COUNTER USE FOR THE TRIANGLE WAVEFORM
// COUNTING FROM 0 TO 4095 CORRESPONDING TO THE 12bit ADC CONVERTER
  triangle_wave_counter+=100;
  if(triangle_wave_counter>=4095){
    triangle_wave_counter=0;
  }

// COUNTER USE FOR THE PWM GENERATION
// COUNTING FROM 0 TO 255 DEGRES CORRESPONDING TO THE 8bit ARDUINO TIMER 
  pwm_led_counter--;
  if(pwm_led_counter<=0){
    pwm_led_counter=255;
  }

  // COUNTER USED FOR DISPLAY REFRESH (CLEAR TO 0 BY TEST)
  displayRefreshTime++;

  // LOOP DELAY IN [mS] FOR THE MAIN PROGRAM
  // ALSO DEFINE THE FREQUENCY FOR THE WAVEFORM ON THE OUTPUT
  delay(1);
}

/**
 * @brief Get the actual menu on display from pot RV1 object
 * Convert the potentiometer value in % to a 
 * menu number.
 * The max value of the potentiometer (100%) is divided by the number of menus
 * and a little portion of the percentage is used as "dead" zone to prevent 
 * the uncontrolled menu switching when the potentiometer is just between two menu. (=Hysteresis)
 * @param pot_RV1_value pot_RV1_value The potentiometer read value in %
 * @return char The number of menu selected
 */

char get_actual_menu_on_display_from_pot_RV1(int pot_RV1_value){
  static char menu = 0;

  if(pot_RV1_value >= 0  && pot_RV1_value < 10) menu = 0;
  if(pot_RV1_value >= 14 && pot_RV1_value < 24) menu =  1;
  if(pot_RV1_value >= 28 && pot_RV1_value < 38) menu =  2;
  if(pot_RV1_value >= 42 && pot_RV1_value < 52) menu =  3;
  if(pot_RV1_value >= 56 && pot_RV1_value < 66) menu =  4;
  if(pot_RV1_value >= 70 && pot_RV1_value < 80) menu =  5;
  if(pot_RV1_value >= 84 && pot_RV1_value < 94) menu =  6;
  if(pot_RV1_value >= 98) menu =  0;
  return(menu);
}

/**
 * @brief gfx display selected menu
 * Display on the screen the text menu correspondign to the menu number
 * no display modification if the menu number is not reconized
 * @param menu number of the menu to display
 */

void gfx_display_selected_menu(char menu){
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, "-OUTPUT TEST-");

  switch(menu){
    case 0:    u8x8.drawString(0, 0, "LCK:   TRG:  ");
               u8x8.drawString(0, 2, "BTN:   BAT:  ");break;
    case 1 : u8x8.drawString(0, 2, TXT_TEST_AOUT_1); break;
    case 2 : u8x8.drawString(0, 2, TXT_TEST_AOUT_2); break;;
    case 3 : u8x8.drawString(0, 2, TXT_TEST_LED_RED); break;
    case 4 : u8x8.drawString(0, 2, TXT_TEST_LED_GREEN); break;
    case 5 : u8x8.drawString(0, 2, TXT_TEST_LED_BLUE); break;
    case 6 : u8x8.drawString(0, 2, TXT_TEST_PWM_SERVO); break;
    default: break;
  }
}

/**
 * @brief gfx display menu0 status data
 * Display on the screen the updated data from
 * the inputs.
 * The text present on the screen is only affected by this function
 * where the cursor is set because no "clear screen" is made.
 */
void gfx_display_menu0_status_data(void){

  // DISPLAY THE DATA FOR INPUTS LICKPORT, TRIGGER, PUSH-BUTTON AND BATTERY VOLTAGE
    u8x8.setCursor(5, 0);
    u8x8.print(lickport_CN2_state);

    u8x8.setCursor(12, 0);
    u8x8.print(trigger_CN3_State);

    u8x8.setCursor(5, 2);
    u8x8.print(button_SW1_State);

    u8x8.setCursor(12, 2);
    u8x8.print((float)battery_BT1_voltage_mV / 1000, 1);
    u8x8.print("V");
}


/**
 * @brief convert Deg To Rad
 * This function just convert a degree value to radian
 * (for sinus waveform génération)
 * @param x in degree
 * @return float 
 */
float convert_Deg_To_Rad(float x)
{
    const float Pi = 3.141592654f;
    return x / 180 * Pi;
}