#include <AverageValue.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Fonts/FreeSans9pt7b.h>


AverageValue<float> avgVin(10);
AverageValue<float> avgCout(10);

Adafruit_SSD1306 display(-1);

static OPAMP_HandleTypeDef hopamp1;

const int PIN_H_1A = PB15; //TIM1_1
const int PIN_L_1A = PC11; //TIM1_1N
const int PIN_H_1B = PC1;  //TIM1_3
const int PIN_L_1B = PB9;  //TIM1_3N
const int PIN_H_1C = PC2;  //TIM1_4
const int PIN_L_1C = PC3;  //TIM1_4N
const int PIN_H_2A = PC4;  //TIM8_1
const int PIN_L_2A = PC8;  //TIM8_1N
const int PIN_H_2B = PC5;  //TIM8_2
const int PIN_L_2B = PC9;  //TIM8_2N
const int PIN_H_2C = PC6;  //TIM8_3
const int PIN_L_2C = PC10; //TIM8_3N

const int PIN_SENSE_VIN = PA9;
const int PIN_SENSE_V_1A = PC0;
const int PIN_SENSE_V_1B = PA0;
const int PIN_SENSE_V_1C = PA3;
const int PIN_SENSE_V_2A = PA4;
const int PIN_SENSE_V_2B = PA5;
const int PIN_SENSE_V_2C = PB2;

const int PIN_SENSE_A_1A = PA1;
const int PIN_SENSE_A_1B = PA7;
const int PIN_SENSE_A_1C = PB0;
//const int PIN_SENSE_A_2A = PB13; //TODO: must not use phase 2A until disable vout to PB13
const int PIN_SENSE_A_2B = PB14;
const int PIN_SENSE_A_2C = PB12;

const int PIN_OPOUT_1A = PA2;
const int PIN_OPOUT_1B = PA6;
const int PIN_OPOUT_1C = PB1;
//const int PIN_OPOUT_2A = PB12; //TODO: must not use phase 2A until disable vout to PB13
const int PIN_OPOUT_2B = PA8;
const int PIN_OPOUT_2C = PB11;

const int PIN_LED_1 = PA10;
const int PIN_LED_2 = PC7;

#define maxV 3.3
#define shuntOhms 0.2
#define analogBits 10
#define analogSteps (float)((1 << analogBits) - 1)

#define ohmsVdivHigh 15000.0
#define ohmsVdivLow 2200.0

void setup() {
  Serial.begin(115200);

  //Phase 1A is GND
  pinMode(PIN_H_1A, OUTPUT);
  digitalWrite(PIN_H_1A, LOW);
  pinMode(PIN_L_1A, OUTPUT);
  digitalWrite(PIN_L_1A, HIGH);

  //OPAMP1 init
  MX_OPAMP1_Init();
  HAL_OPAMP_MspInit(&hopamp1);
  HAL_OPAMP_Start(&hopamp1);
  //investigate HAL_OPAMP_SelfCalibrate();

  Wire.setSDA(PB7);
  Wire.setSCL(PA15);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  //display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  updateDisplay();

}

void loop() {


  avgVin.push(((float)analogRead(PIN_SENSE_VIN) / analogSteps) * maxV * (ohmsVdivHigh + ohmsVdivLow) / ohmsVdivLow);
  avgCout.push(((float)analogRead(PIN_OPOUT_1A)) * (1.3 / 26.0));

  //if (millis() % 300 == 0) digitalWrite(PIN_LED_2, LOW);
  //if (millis() % 600 == 0) digitalWrite(PIN_LED_2, HIGH);


  Serial.print("  Input Voltage: ");
  Serial.print(avgVin.average());
  Serial.print("  Output Current: ");
  Serial.print(avgCout.average());
  Serial.println(" ");

  updateDisplay();
}

void updateDisplay(){
  char InputVoltage[21];
  char OutputCurrent[21];
  char OutputPower[21];

  float power = 5.0 * avgCout.average();

  strcpy(InputVoltage, "Input Voltage:  "); // Copy the initial part of the message
  strcat(InputVoltage, String(avgVin.average(), 1).c_str()); // Append the float value as a string
  strcat(InputVoltage, "V"); // Append the "V" symbol
  
  strcpy(OutputCurrent, "Output Current: "); // Copy the initial part of the message
  strcat(OutputCurrent, String(avgCout.average(), 1).c_str()); // Append the float value as a string
  strcat(OutputCurrent, "A"); // Append the "V" symbol
  
  strcpy(OutputPower, "Output Power:   "); // Copy the initial part of the message
  strcat(OutputPower, String(power, 1).c_str()); // Append the float value as a string
  strcat(OutputPower, "W"); // Append the "V" symbol


  display.clearDisplay();
  display.setCursor(0, 0);
  writeln("IEEE Charger", 12);
  writeln(InputVoltage, 20);
  writeln(OutputCurrent, 20);
  writeln(OutputPower, 20);
  display.display();
}

void writeln(char *s, size_t len) {
    for (size_t i = 0; i < len; i++) {
      display.write(s[i]);
    }
    for (size_t i = len; i < 21; i++) {
      display.write(' ');
    }
}

void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef* hopamp) {
  GPIO_InitTypeDef GPIO_InitStruct_A = { 0 };
  if (hopamp->Instance == OPAMP1) {

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP1 GPIO Configuration
    PA2     ------>  OPAMP1_VOUT
    PC0     ------> OPAMP1_VINP
    */
    GPIO_InitStruct_A.Pin =  GPIO_PIN_1 || GPIO_PIN_2;
    GPIO_InitStruct_A.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct_A.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_A);
  }
}

static void MX_OPAMP1_Init(void) {
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
  hopamp1.Init.Mode = OPAMP_PGA_MODE;
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hopamp1.Init.InvertingInput = OPAMP_INVERTINGINPUT_IO1;  //TODO: this doesnt matter right?
  hopamp1.Init.InternalOutput = DISABLE;
  hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp1.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_NO;  //OPAMP_PGA_CONNECT_INVERTINGINPUT_NO
  hopamp1.Init.PgaGain = OPAMP_PGA_GAIN_64_OR_MINUS_63;           //OPAMP_PGA_GAIN_4_OR_MINUS_3
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK) {
    Serial.println("OPAMP1 Init fail!");
  }
}
