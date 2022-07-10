void getDistanceTimeCalendar(float angle) {
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
  tft.println("Please set your mode of transport: ");
  transport_mode = set_mode(angle);

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5, 10, 1);
  tft.println("Please set how long it takes you to get ready in minutes: ");
  ready_duration = set_ready(angle);
  
  tft.fillScreen(TFT_BLACK);
  char body[300]; //for body
  sprintf(body,"user=%s&start=%s&mode=%d", username, start_loc, transport_mode);//generate body, posting to User, 1 step
  int body_len = strlen(body); //calculate body length (for header reporting)

  memset(request, 0, IN_BUFFER_SIZE);
  memset(response, 0, OUT_BUFFER_SIZE);
  
  sprintf(request,"POST http://608dev-2.net/sandbox/sc/team16/location_info/calendar.py HTTP/1.1\r\n");
  strcat(request,"Host: 608dev-2.net\r\n");
  strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request+strlen(request),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request,"\r\n"); //new line from header to body
  strcat(request,body); //body
  strcat(request,"\r\n"); //new line
  Serial.println(request);

  do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);

  // sprintf(calendar_request,"POST http://608dev-2.net/sandbox/sc/team16/location_info/calendar.py HTTP/1.1\r\n");
  // strcat(calendar_request,"Host: 608dev-2.net\r\n");
  // strcat(calendar_request,"Content-Type: application/x-www-form-urlencoded\r\n");
  // sprintf(calendar_request+strlen(calendar_request),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  // strcat(calendar_request,"\r\n"); //new line from header to body
  // strcat(calendar_request,body); //body
  // strcat(calendar_request,"\r\n"); //new line
  // Serial.println(calendar_request);

  // do_http_request("608dev-2.net", calendar_request, calendar_response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);

  Serial.println("requested");
  
  char delim[2] = ",";
  char *calString = NULL;
  //char ttt[50] = "1.0,13,02:00";
  calString = strtok(response, delim);
  distance_calendar = atof(calString);
  //Serial.printf("%f", distance_calendar);
  calString = strtok (NULL, delim);
  travel_time_calendar = atoi(calString);
  //Serial.printf("%d", travel_time_calendar);
  calString = strtok (NULL, delim);
  Serial.println(calString);
  //goal_time_calendar = distString;
  strcpy(goal_time_calendar, calString);
  Serial.println("hello");

  int total_minutes = travel_time_calendar + ready_duration;
  int hours = total_minutes/60;
  int leftover_mins = total_minutes%60;

  char wakeup_time[10] = "";
  calculateWakeupTime(goal_time_calendar, hours, leftover_mins, wakeup_time);
  Serial.println(wakeup_time);
  strcpy(alarm_time, wakeup_time);
  return;
}
