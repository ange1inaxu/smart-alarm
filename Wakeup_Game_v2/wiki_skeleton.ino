//used to get x,y values from IMU accelerometer!
void get_angle(float* x, float* y) {
  imu.readAccelData(imu.accelCount);
  *x = imu.accelCount[0] * imu.aRes;
  *y = imu.accelCount[1] * imu.aRes;
}

void set_time(float angle, char* alarm_time) {
  tft.setTextSize(1);
  tft.setCursor(5, 10, 1);
  tft.println("Please set the time: ");
  strcpy(alarm_time,"");    
  for(int i = 0; i < 2; i ++) {
    int char_index = 0;
    while(true) {
      get_angle(&a, &angle);
      int bv1 = button1.update();
    
      if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
        char_index ++;
        if (char_index > 9) {
          char_index -= 10;
        }
        scroll_timer = millis();
      } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
        char_index --;
        if (char_index < 0) {
          char_index += 10;
        }
        scroll_timer = millis();
      }
      
      if (bv1 == 1) {
        char current_num[5] = "";
        current_num[0] = numbers[char_index];
        strcat(alarm_time, current_num);
        strcpy(output, alarm_time);
        char_index = 0;
        break;      
      }

      sprintf(output, "%s%c", alarm_time, numbers[char_index]);
      tft.setCursor(10, 30, 1);
      tft.println(output);
    }
    
  }

  strcat(alarm_time, ":");

  for(int i = 0; i < 2; i ++) {
    int char_index = 0;
    while (true) {
      get_angle(&a, &angle);
      int bv1 = button1.update();
      
      if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
        char_index ++;
        if (char_index > 9) {
          char_index -= 10;
        }
        scroll_timer = millis();
      } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
        char_index --;
        if (char_index < 0) {
          char_index += 10;
        }
        scroll_timer = millis();
      }
      
      if (bv1 == 1) {
        char current_num[5] = "";
        current_num[0] = numbers[char_index];
        strcat(alarm_time, current_num);
        strcpy(output, alarm_time);
        char_index = 0;
        break;      
      }

      sprintf(output, "%s%c", alarm_time, numbers[char_index]);
      tft.setCursor(10, 30, 1);
      tft.println(output);
    }
    
  }
  
  
}

