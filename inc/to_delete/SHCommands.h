#include "SG_LCD.h"
SG_LCD lcd;


void Command_ExpandedCommandsList() {
#ifdef INCLUDE_SPEEDOGAUGE
  FlowSerialPrintLn("speedo");
#endif
#ifdef INCLUDE_TACHOMETER
  FlowSerialPrintLn("tachometer");
#endif
#ifdef INCLUDE_BOOSTGAUGE
  FlowSerialPrintLn("boostgauge");
#endif
#ifdef INCLUDE_TEMPGAUGE
  FlowSerialPrintLn("tempgauge");
#endif
#ifdef INCLUDE_FUELGAUGE
  FlowSerialPrintLn("fuelgauge");
#endif
#ifdef INCLUDE_CONSGAUGE
  FlowSerialPrintLn("consumptiongauge");
#endif
#ifdef INCLUDE_ENCODERS
  FlowSerialPrintLn("encoders");
#endif
  FlowSerialPrintLn("mcutype");
  FlowSerialPrintLn();
  FlowSerialFlush();
}

void Command_Features() {
  delay(10);

  // Matrix
  if (MAX7221_MATRIX_ENABLED == 1 || ENABLE_ADA_HT16K33_BiColorMatrix == 1 || ENABLE_ADA_HT16K33_SingleColorMatrix == 1) {
    FlowSerialPrint("M");
  }

  // LCD
  if (I2CLCD_enabled == 1) {
#ifdef INCLUDE_I2CLCD
    FlowSerialPrint("L");
#endif
  }

  if (ENABLED_NOKIALCD > 0 || ENABLED_OLEDLCD > 0) {
    FlowSerialPrint("K");
  }

  // Gear
  //FlowSerialPrint("G");

  // Name
  FlowSerialPrint("N");

  // Additional buttons
  //FlowSerialPrint("J");

  // Custom Protocol Support
  FlowSerialPrint("P");

  // Xpanded support
  FlowSerialPrint("X");

  // RGB MATRIX
  if (WS2812B_MATRIX_ENABLED > 0) {
    FlowSerialPrint("R");
  }

#if defined(INCLUDE_SHAKEITADASHIELD) || defined(INCLUDE_SHAKEITDKSHIELD) || defined(INCLUDE_SHAKEITL298N) || defined(INCLUDE_SHAKEITMOTOMONSTER) || defined(INCLUDE_SHAKEITPWM)  || defined(INCLUDE_SHAKEITPWMFANS)
  // Afafuit motorshields
  FlowSerialPrint("V");
#endif // INCLUDE_SHAKEITADASHIELD

  FlowSerialPrint("\n");
  FlowSerialFlush();
}

void Command_Motors() {
#if defined(INCLUDE_SHAKEITADASHIELD) || defined(INCLUDE_SHAKEITDKSHIELD) || defined(INCLUDE_SHAKEITL298N) || defined(INCLUDE_SHAKEITMOTOMONSTER) || defined(INCLUDE_SHAKEITPWM) || defined(INCLUDE_SHAKEITPWMFANS)

  char action = FlowSerialTimedRead();
  // Count
  if (action == 'C') {
    FlowSerialWrite(255);
    FlowSerialWrite(ADAMOTORS_SHIELDSCOUNT * 4 + min(1, DKMOTOR_SHIELDSCOUNT) * 4 + min(1, L98NMOTORS_ENABLED) * 2 + min(1, MOTOMONSTER_ENABLED) * 2 + min(4, SHAKEITPWM_ENABLED_MOTORS) + min(4, SHAKEITPWMFANS_ENABLED_MOTORS));

#ifdef  INCLUDE_SHAKEITADASHIELD
    FlowSerialPrint(shShakeitAdaMotorShieldV2.providerName() + ";");
#endif
#ifdef  INCLUDE_SHAKEITDKSHIELD
    FlowSerialPrint(shShakeitDKMotorShield.providerName() + ";");
#endif
#ifdef  INCLUDE_SHAKEITL298N
    FlowSerialPrint(shShakeitL298N.providerName() + ";");
#endif
#ifdef INCLUDE_SHAKEITMOTOMONSTER
    FlowSerialPrint(shShakeitMotoMonster.providerName() + ";");
#endif
#ifdef INCLUDE_SHAKEITPWM
    FlowSerialPrint(shShakeitPWM.providerName() + ";");
#endif
#ifdef INCLUDE_SHAKEITPWMFANS
    FlowSerialPrint(shShakeitPWMFans.providerName() + ";");
#endif
    FlowSerialPrintLn();
  }
  // Set motors
  else if (action == 'S') {
#ifdef  INCLUDE_SHAKEITADASHIELD
    shShakeitAdaMotorShieldV2.read();
#endif
#ifdef  INCLUDE_SHAKEITDKSHIELD
    shShakeitDKMotorShield.read();
#endif
#ifdef  INCLUDE_SHAKEITL298N
    shShakeitL298N.read();
#endif
#ifdef INCLUDE_SHAKEITMOTOMONSTER
    shShakeitMotoMonster.read();
#endif
#ifdef INCLUDE_SHAKEITPWM
    shShakeitPWM.read();
#endif
#ifdef INCLUDE_SHAKEITPWMFANS
    shShakeitPWMFans.read();
#endif
  }
#endif
}

void Command_CustomProtocolData() {
    if(lcd.get_i() == 1)
    {
      lcd.drawHUBCON();
      return;
    }
    FlowSerialReadStringUntil(lcdd.gear, 2, ';');
    FlowSerialReadStringUntil(lcdd.speed, 10, ';');
    FlowSerialReadStringUntil(lcdd.position, 3, ';');
    FlowSerialReadStringUntil(lcdd.lap, 4, ';');
    FlowSerialReadStringUntil(lcdd.my_best, 10, ';');
    FlowSerialReadStringUntil(lcdd.best_lap, 10, ';');
    FlowSerialReadStringUntil(lcdd.delta, 10, ';');
    lcdd.drs_av = FlowSerialReadStringUntil(';').toInt();
    lcdd.drs_en = FlowSerialReadStringUntil(';').toInt();
    FlowSerialReadStringUntil(lcdd.fuel, 5, ';');
    FlowSerialReadStringUntil(lcdd.tyreTempFL, 4, ';');
    FlowSerialReadStringUntil(lcdd.tyreTempFR, 4, ';');
    FlowSerialReadStringUntil(lcdd.tyreTempRL, 4, ';');
    FlowSerialReadStringUntil(lcdd.tyreTempRR, 4, ';');
    FlowSerialReadStringUntil(lcdd.ERS, 5, ';');
    FlowSerialReadStringUntil(lcdd.brake_bias, 5, ';');    
    lcdd.rpm = FlowSerialReadStringUntil(';').toInt();
    lcdd.flag = FlowSerialReadStringUntil(';');
    lcdd.running = FlowSerialReadStringUntil('\n').toInt();
  FlowSerialWrite(0x15);
}
