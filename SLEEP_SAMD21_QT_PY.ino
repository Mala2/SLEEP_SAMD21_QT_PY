
// Put the SAMD21 into deep sleep and awake with a logic HIGH interrupt on pin A0.
// Make sure to pulldown A0 with a resistor and connect the other end to 3V to 
// act as logic HIGH when pressed. A3 will act as an input for the LED.

// To demonstrate, after setting up the connections. You need to press the button
// >3s to enter sleep mode. Another press will wake up the SAMD21.

const int On_Off_pin = A0;   //ON/OFF
const int ledPin =  A3;      // the LED pin

byte
currSwitch,
lastSwitch,
STATUS = 1;

unsigned long
timeStart;
bool
bCheckingSwitch;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode( On_Off_pin, INPUT );
  lastSwitch = digitalRead( On_Off_pin );
  bCheckingSwitch = false;
  digitalWrite(ledPin, HIGH);
}

void loop() {
  wake_BTN ();

    Serial.println("I am AWAKE !!!");
    
}


//---------------------------------------wake_BTN-------------------------------------

void wake_BTN () {
  //read the switch (for simplicity, no debouncing done)
  currSwitch = digitalRead( On_Off_pin );

  if ( currSwitch != lastSwitch )
  {
    //if low now, the change was unpressed to pressed
    if ( currSwitch == HIGH )
    {
      //in that case, get the time now to start the
      //5-sec delay...
      timeStart = millis();
      //...indicate we're now timing a press...
      bCheckingSwitch = true;
    }//if
    else
    {
      bCheckingSwitch = false;
    }//else

    //and save the
    lastSwitch = currSwitch;
  }
  if ( bCheckingSwitch )
  {
    // below you can adjust how long the button should be pressed
    if ((millis() - timeStart ) >= 3000) {
      Serial.println("Going to sleep");
      digitalWrite(ledPin, LOW);
      delay(3000);//sleep time
      sleep_CPU();
    }
  }
}
//---------------------------------------wake_CPU-------------------------------------

void wake_CPU (void) {
  Serial.println("I am AWAKE !!!");
  digitalWrite(ledPin, HIGH);           // Show we're awake again
  USB->DEVICE.CTRLA.bit.ENABLE = 1;     // Turn on the USB peripheral
  pinMode( On_Off_pin, INPUT );
  lastSwitch = digitalRead( On_Off_pin );
  bCheckingSwitch = false;
  Serial.println("I am AWAKE !!!");
}  // end of wake

//---------------------------------------sleep_CPU-------------------------------------

void sleep_CPU(void)
{
  // Disabled peripherals
  //  SERCOM0->USART.CTRLA.bit.ENABLE = 0;
  //  SERCOM1->USART.CTRLA.bit.ENABLE = 0;
  //  SERCOM2->USART.CTRLA.bit.ENABLE = 0;
  //  SERCOM3->USART.CTRLA.bit.ENABLE = 0;
  //
  //  I2S->CTRLA.bit.ENABLE = 0;
  //
  //  ADC->CTRLA.bit.ENABLE = 0;
  //
  //  DAC->CTRLA.bit.ENABLE = 0;
  //
  //  AC->CTRLA.bit.ENABLE = 0;
  //
  //  TCC0->CTRLA.bit.ENABLE = 0;
  //  TCC1->CTRLA.bit.ENABLE = 0;
  //  TCC2->CTRLA.bit.ENABLE = 0;
  //
  //  RTC->MODE0.CTRL.bit.ENABLE = 0;
  //  RTC->MODE1.CTRL.bit.ENABLE = 0;
  //  RTC->MODE2.CTRL.bit.ENABLE = 0;
  //
  //  USB->HOST.CTRLA.bit.ENABLE = 0;
  USB->DEVICE.CTRLA.bit.ENABLE = 0;             // Shutdown the USB peripheral
  while (USB->DEVICE.SYNCBUSY.bit.ENABLE);
  attachInterrupt(A0, wake_CPU, HIGH);          // Set up an interrupt on analog pin A1 on a logic HIGH level
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;            // Set up the SAMD21 to enter low power STANDBY mode
  digitalWrite(ledPin, LOW);                    // Set D13 output to logic LOW
  __DSB();                                      // Complete data memory operations
  __WFI();                                      // Put the SAMDx1 into deep sleep Zzzzzzz....
}
