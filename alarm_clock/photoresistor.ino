float resistanceExtractor(float vin,float rb,float vout){
    return -rb/(1-vin/vout);
}

float brightnessExtractor(float vout){
    float rb = 20000;
    float vin = 3.3;
    float rs = resistanceExtractor(vin, rb, vout);
    return pow(10, (log10(rs) - 8.65) / -2.0);
}

void photoresistorAlarm() {
    //read brightness increase based off of a processed analog measurement and calculate lux (outside brightness)
    bright_inc = analogRead(pin);
    float vout = bright_inc / photoresistor_value * 3.3;
    float lux = brightnessExtractor(vout);
    
    //tft.setCursor(5, 10, 1);
    //tft.printf("Current Lux: %d   ", (int)lux);
    Serial.printf("Current Lux: %d   ", (int)lux);
     
    if ((int)lux < 100){
      on_off = 1;
      mode_state = ALARM_MODE;
    }
}
