char year[5] = "";
char month[4] = "";
char day[4] = "";
char date_string[30] = "";
char date_string_raw[30] = "";

void display_recap() {
  int bv4 = button4.update();
  Serial.println(bv4);
  get_current_date();
  get_weekly_scores(date);
  display(date);
  if (bv4 == 1) {
    gotten_mode_state = 0;
    tft.fillScreen(TFT_BLACK);
    Serial.println("display_recap: bv4 update, recap to choose modes");
    return;
  }
}

void display(int date) {
  tft.setCursor(5, 10, 1);
  tft.println("Weekly Recap");
  tft.println("                     ");
  // tft.setCursor(2,20,2);
  tft.println(" Date           Score");
  tft.println();
  // Serial.println("done with displaying titles");
  for (int i=0; i<7; i++) {
    format_date(date-i*10000, date_string);
    tft.printf(" %s      %d  ", date_string, scores[i]);
    tft.println();
    tft.println();
  }
  // if (slide == 0) {
  //   for (int i=0; i<4; i++) {
  //     tft.printf("%d      %d  ", (date-i*10000), scores[i]);
  //     tft.println();
  //   }
  // }
  // else if (slide == 1) {
  //   for (int i=4; i<7; i++) {
  //     tft.printf("%d       %d", (date-i*10000), scores[i]);
  //     tft.println();
  //   }
  //   tft.println("                   ");
  // }
}

void get_current_date() {
  char host[] = "iesc-s3.mit.edu";
  memset(time_request_buffer, 0, TIME_IN_BUFFER_SIZE);
  memset(time_response_buffer, 0, TIME_OUT_BUFFER_SIZE);
  sprintf(time_request_buffer,"GET http://iesc-s3.mit.edu/esp32test/currenttime HTTP/1.1\r\n");
  strcat(time_request_buffer,"Host: iesc-s3.mit.edu\r\n"); //add more to the end
  strcat(time_request_buffer,"\r\n"); //add blank line!
  do_http_request(host, time_request_buffer, time_response_buffer, TIME_OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);
  // Serial.println(time_response_buffer);

  // convert response to date integer
  memset(year, 0, sizeof(year));
  slice(time_response_buffer, year, 0, 4);
  date = atoi(year); // date = 2022
  memset(month, 0, sizeof(month));
  slice(time_response_buffer, month, 5, 7);
  date += atoi(month)*1000000; // date = 05002022
  memset(day, 0, sizeof(day));
  slice(time_response_buffer, day, 8, 10);
  date += atoi(day)*10000; // date = 05012022
  // Serial.printf("get_current_date: %d\n", date);
}

void format_date(int date, char* date_string) {
  memset(date_string, 0, sizeof(date_string));
  memset(date_string_raw, 0, sizeof(date_string_raw));
  // Serial.println("done memsetting");
  sprintf(date_string_raw, "%08d", date);
  // Serial.printf("date_string_raw: %s\n", date_string_raw);
  memset(month, 0, sizeof(month));
  slice(date_string_raw, month, 0, 2);
  char_append(month, '/', sizeof(month));
  // Serial.printf("month: %s\n", month);
  // month[2] = '/';
  memset(day, 0, sizeof(day));
  slice(date_string_raw, day, 2, 4);
  char_append(day, '/', sizeof(day));
  // Serial.printf("day: %s\n", day);
  // day[2] = '/';
  memset(year, 0, sizeof(year));
  slice(date_string_raw, year, 4, 8);
  // Serial.printf("year: %s\n", year);  
  sprintf(date_string, month);
  strcat(date_string, day);
  strcat(date_string, year);
  // Serial.printf("format_date: %s\n", date_string);
}

void get_weekly_scores(int start_date) {
  int next_date = start_date;
  for (int i=0; i<7; i++) {
    // Serial.printf("get_weekly_scores: %d\n", next_date);
    get_score(next_date);
    next_date -= 10000;
    scores[i] = atoi(response);
  }
  // Serial.println("done with get_weekly_scores");
}

void get_score(int date){
  memset(request, 0, IN_BUFFER_SIZE);
  memset(response, 0, OUT_BUFFER_SIZE);  
  sprintf(request, "GET http://608dev-2.net/sandbox/sc/team16/weekly_database/weekly_database.py?username=%s&date=%08d HTTP/1.1\r\n", username, date);
  strcat(request, "Host: 608dev-2.net\r\n"); //add more to the end
  strcat(request, "\r\n"); //add blank line!
  //submit to function that performs GET.  It will return output using response_buffer char array
  do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  // Serial.println(response); //print to serial monitor
  // tft.println(response);
}