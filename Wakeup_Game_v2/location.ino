void getLocations(char* location_request, char* location_response) {
  //sprintf(location_request, "");
  char host[] = "608dev-2.net";
  
  memset(location_request, 0, IN_BUFFER_SIZE);
  memset(location_response, 0, OUT_BUFFER_SIZE);
  
  sprintf(location_request,"GET http://608dev-2.net/sandbox/sc/team16/location_info/all_locations.py?username=%s HTTP/1.1\r\n", "ahan");
  strcat(location_request,"Host: 608dev-2.net\r\n"); //add more to the end
  strcat(location_request,"\r\n"); //add blank line!

  do_http_request(host, location_request, location_response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);
  Serial.println(location_response);

  char delim[2] = ",";
  char *pch = NULL;
  int index = 0;
  //location_length = 0;
  pch = strtok(location_response, delim);
  while (pch != NULL) {
    Serial.println(pch);
    strcpy(locations[index], pch);
    //location_length ++;
    pch = strtok (NULL, delim);
    index++;
  }
  location_length = index;
}

void set_start(float angle) {
  Serial.printf("%d", location_length);
  char location[50] = "";
  char prev_location[50] = "";
  int char_index = 0;
  while(true) {
    get_angle(&a, &angle);
    int bv1 = button1.update();
  
    if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index ++;
      if (char_index > location_length - 1) {
        char_index -= location_length;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index --;
      if (char_index < 0) {
        char_index += location_length;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    }
    
    if (bv1 == 1) {
      strcpy(start_loc, locations[char_index]);
      return;
    }

    strcpy(start_loc, locations[char_index]);
    strcpy(prev_location, location);
    strcpy(location, start_loc);

    tft.setCursor(10, 30, 1);
    tft.println(location);
  }
}

void set_end(float angle) {
  char location[50] = "";
  char prev_location[50] = "";
  int char_index = 0;
  while(true) {
    get_angle(&a, &angle);
    int bv1 = button1.update();
  
    if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index ++;
      if (char_index > location_length - 1) {
        char_index -= location_length;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index --;
      if (char_index < 0) {
        char_index += location_length;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    }
    
    if (bv1 == 1) {
      strcpy(end_loc, locations[char_index]);
      break;
   
    }

    strcpy(end_loc, locations[char_index]);
    strcpy(prev_location, location);
    strcpy(location, end_loc);

    tft.setCursor(10, 30, 1);
    tft.println(location);
    
  }
}

int set_mode(float angle) {
  int modeNum = 0;
  char title[50] = "";
  char prev_title[50] = "";
  char current_num[5] = "";
  int char_index = 0;
  while(true) {
    get_angle(&a, &angle);
    int bv1 = button1.update();
  
    if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index ++;
      if (char_index > 3) {
        char_index -= 4;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index --;
      if (char_index < 0) {
        char_index += 4;
      }
      tft.setCursor(10, 30, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    }
    
    if (bv1 == 1) {
      
      current_num[0] = mode_numbers[char_index];
      modeNum = atoi(current_num);
      return modeNum;
   
    }

    current_num[0] = mode_numbers[char_index];
    strcpy(prev_title, title);
    numToMode(atoi(current_num), title);

    tft.setCursor(10, 30, 1);
    tft.println(title);
    
  }
}

int set_ready(float angle) {
  char duration[50] = "";
  char output[400] = {0}; 
  //char current_num[5] = "";
  int char_index = 0;
  while(true) {
    get_angle(&a, &angle);
    int bv1 = button1.update();
  
    if (angle >= angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index ++;
      if (char_index > 9) {
        char_index -= 10;
      }
      tft.setCursor(10, 35, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    } else if (angle <= -angle_threshold && millis() - scroll_timer >= scroll_threshold) {
      char_index --;
      if (char_index < 0) {
        char_index += 10;
      }
      tft.setCursor(10, 35, 1);
      tft.println("                                                 ");
      scroll_timer = millis();
    }
    
    if (bv1 == 1) {
      
      char current_num[5] = "";
      current_num[0] = numbers[char_index];
      strcat(duration, current_num);
      strcpy(output, duration);
      char_index = 0;  
   
    }

    if (bv1 == 2) {
      return atoi(duration);
    }

    sprintf(output, "%s%c", duration, numbers[char_index]);
    tft.setCursor(10, 35, 1);
    tft.println(output);
    
  }
}

void numToMode(int modeNum, char* title) {
  switch (modeNum) {
    case 0:
      strcpy(title, "Driving");
      break;
    case 1:
      strcpy(title, "Walking");
      break;
    case 2:
      strcpy(title, "Biking ");
      break;
    case 3:
      strcpy(title, "Transit");
      break;
  }
}

void calculateWakeupTime(char* goal_time, int hours, int minutes, char* wakeup_time) {
  char hourString[3] = "";
  slice(goal_time, hourString, 0, 2); 

  char minuteString[3] = "";
  slice(goal_time, minuteString, 3, 5); 

  int hour = atoi(hourString) - hours;
  int minute = atoi(minuteString) - minutes;

  if (minute < 0) {
    minute += 60;
    hour --;
  }

  char AMorPM[] = " AM";

  if(hour > 12){
    hour -= 12;
    strcpy(AMorPM, " PM");
  } else if(hour == 12) {
    strcpy(AMorPM, " PM");
  } else if (hour == 0) {
    hour += 12;    
  } else if (hour < 0) {
    hour += 12;
    strcpy(AMorPM, " PM");
  }
  sprintf(wakeup_time,"%d:%d%s", hour, minute, AMorPM); 

}

void getDistanceTime(float angle) {
  // getLocations(location_request, location_response);
  getLocations(request, response);
  for (int i = 0; i < location_length; i++) {
    Serial.println(locations[i]);    
  }
  
  tft.setTextSize(1);
  tft.setCursor(5, 10, 1);
  tft.println("Please set your starting location: ");
  set_start(angle);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 10, 1);
  tft.println("Please set your ending location: ");
  set_end(angle);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 10, 1);
  tft.println("Please set the time of arrival: ");
  set_time(angle, goal_time);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 10, 1);
  tft.println("Please set your mode of transport: ");
  transport_mode = set_mode(angle);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 10, 1);
  tft.println("Please set how long it takes you to get ready in minutes: ");
  ready_duration = set_ready(angle);
  
  tft.fillScreen(TFT_BLACK);
  char body[300]; //for body
  sprintf(body,"user=%s&start=%s&end=%s&mode=%d", "ahan", start_loc, end_loc, transport_mode);//generate body, posting to User, 1 step
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(distance_request,"POST http://608dev-2.net/sandbox/sc/team16/location_info/schedule.py HTTP/1.1\r\n");
  strcat(distance_request,"Host: 608dev-2.net\r\n");
  strcat(distance_request,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(distance_request+strlen(distance_request),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(distance_request,"\r\n"); //new line from header to body
  strcat(distance_request,body); //body
  strcat(distance_request,"\r\n"); //new line
  Serial.println(distance_request);

  do_http_request("608dev-2.net", distance_request, distance_response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  Serial.println("requested");
  
  char delim[2] = ",";
  char *distString = NULL;
  distString = strtok(distance_response, delim);
  distance = atof(distString);
  distString = strtok (NULL, delim);
  travel_time = atoi(distString);

  int total_minutes = travel_time + ready_duration;
  int hours = total_minutes/60;
  int leftover_mins = total_minutes%60;

  char wakeup_time[10] = "";
  calculateWakeupTime(goal_time, hours, leftover_mins, wakeup_time);
  Serial.println(wakeup_time);
  strcpy(alarm_time, wakeup_time);
  return;
}

//main body of code
/*void loop() {
  button_state = digitalRead(BUTTON);
  if (!button_state && button_state != old_button_state) {
    int offset = sprintf(json_body, "%s", PREFIX);
    int n = WiFi.scanNetworks(); //run a new scan. could also modify to use original scan from setup so quicker (though older info)
    Serial.println("scan done");
    if (n == 0) {
      Serial.println("no networks found");
    } else {
      int max_aps = max(min(MAX_APS, n), 1);
      for (int i = 0; i < max_aps; ++i) { //for each valid access point
        uint8_t* mac = WiFi.BSSID(i); //get the MAC Address
        offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE-offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); //generate the query
        if(i!=max_aps-1){
          offset +=sprintf(json_body+offset,",");//add comma between entries except trailing.
        }
      }
      sprintf(json_body + offset, "%s", SUFFIX);
      Serial.println(json_body);
      int len = strlen(json_body);
      // Make a HTTP request:
      Serial.println("SENDING REQUEST");
      request[0] = '\0'; //set 0th byte to null
      offset = 0; //reset offset variable for sprintf-ing
      offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
      offset += sprintf(request + offset, "Host: googleapis.com\r\n");
      offset += sprintf(request + offset, "Content-Type: application/json\r\n");
      offset += sprintf(request + offset, "cache-control: no-cache\r\n");
      offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
      offset += sprintf(request + offset, "%s\r\n", json_body);
      do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      Serial.println("-----------");
      Serial.println(response);
      Serial.println("-----------");
      //For Part Two of Lab04B, you should start working here. Create a DynamicJsonDoc of a reasonable size (few hundred bytes at least...)
      DynamicJsonDocument doc(1024);
      char* startBracket = strchr(response, '{');
      char* endBracket = strrchr(response, '}');      

      /*char json[strlen(response)] = "";
      strncpy(json, startBracket, endBracket - startBracket + 1);*/

      /**(endBracket+1) = '\0';

      DeserializationError error = deserializeJson(doc, startBracket);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      double latitude = doc["location"]["lat"];
      double longitude = doc["location"]["lng"];
      
      tft.setCursor(0, 0, 2);
      tft.println("Current Location:");
      tft.printf("Lat: %lf \n", latitude);
      tft.printf("Lon: %lf \n", longitude);  
      
      /*char host[] = "608dev-2.net";
      sprintf(request_buffer,"GET http://608dev-2.net/sandbox/sc/ahan/lab5a.py?lat=%f&lon=%f HTTP/1.1\r\n", latitude, longitude);
      strcat(request_buffer,"Host: 608dev-2.net\r\n"); //add more to the end
      strcat(request_buffer,"\r\n"); //add blank line!

      do_http_request(host, request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);      */
      /*char body[300]; //for body
      sprintf(body,"user=%s&lat=%f&lon=%f", "ah", latitude, longitude);//generate body, posting to User, 1 step
      int body_len = strlen(body); //calculate body length (for header reporting)
      sprintf(request_buffer,"POST http://608dev-2.net/sandbox/sc/ahan/lab5b.py HTTP/1.1\r\n");
      strcat(request_buffer,"Host: 608dev-2.net\r\n");
      strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer,"\r\n"); //new line from header to body
      strcat(request_buffer,body); //body
      strcat(request_buffer,"\r\n"); //new line
      //Serial.println(request_buffer);
      do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
    
          
      tft.println(response_buffer);
    }
  }
  old_button_state = button_state;
}*/
