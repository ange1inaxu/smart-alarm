
int run_game() {
  int bv1 = button1.update();
  int bv2 = button2.update();
  int bv3 = button3.update();
  if(played == 0) {
    Serial.printf("%d\n", alarm_sound);
    myDFPlayer.play(alarm_sound);
    played = 1;
  }

  if (millis() - game_timer >= TWO_MIN) {
    game_state = GAME_OVER;    
  }

  if (game_state == IDLE) {
    tft.setCursor(0, 2, 1);
    //tft.println("Long press to start game!");

    //if(bv1 == 2) {
    game_state = Q_DISPLAY;
    Serial.println("Q_DISP");
    char host[] = "608dev-2.net";
    memset(request, 0, IN_BUFFER_SIZE);
    memset(response, 0, OUT_BUFFER_SIZE);
    sprintf(request,"GET http://608dev-2.net/sandbox/sc/team16/trivia.py?scoreboard=false HTTP/1.1\r\n");
    strcat(request,"Host: 608dev-2.net\r\n"); //add more to the end
    strcat(request,"\r\n"); //add blank line!
    do_http_request(host, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);
    
    char delim[2] = "\n";
    char* token;
    token = strtok(response, delim);
    strcpy(question, token);
    token = strtok(NULL, delim);
    strcpy(answer, token);

    Serial.println(question);
    Serial.println(answer);
    game_timer = millis();
    tft.fillScreen(TFT_BLACK);
  //}
    
  } else if (game_state == Q_DISPLAY) {
    tft.setCursor(0, 2, 1);
    tft.setTextSize(1);
    tft.println(question);

    if (bv2 == 1) {
      game_state = USER_TRUE;
      if (compare(answer, TRUE)) {
        score ++;
        correct_num ++;
      } else {
        score --;
        incorrect_num ++;  
      }
      tft.fillScreen(TFT_BLACK);
    } else if (bv3 == 1) {
      game_state = USER_FALSE;
      if (compare(answer, FALSE)) {
        score ++;
        correct_num ++;
      } else {
        score --;
        incorrect_num ++;  
      }
      tft.fillScreen(TFT_BLACK);
    }
    
  } else if (game_state == USER_TRUE) {

    tft.setCursor(0, 2, 1);
    tft.setTextSize(1);
    tft.printf("Score: %d\nNumber Correct: %d\nNumber Incorrect: %d\n", score, correct_num, incorrect_num);      

    if(bv1 == 1) {
      game_state = Q_DISPLAY;  
      char host[] = "608dev-2.net";
      memset(request, 0, IN_BUFFER_SIZE);
      memset(response, 0, OUT_BUFFER_SIZE);
      sprintf(request,"GET http://608dev-2.net/sandbox/sc/team16/trivia.py?scoreboard=false HTTP/1.1\r\n");
      strcat(request,"Host: 608dev-2.net\r\n"); //add more to the end
      strcat(request,"\r\n"); //add blank line!
      do_http_request(host, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);

      char delim[2] = "\n";
      char* token;
      token = strtok(response, delim);
      strcpy(question, token);
      token = strtok(NULL, delim);
      strcpy(answer, token);

      Serial.println(question);
      Serial.println(answer);

      tft.fillScreen(TFT_BLACK); 
    } /*else if (bv1 == 2) {
      game_state = GAME_OVER;
      tft.fillScreen(TFT_BLACK); 
    }*/

  } else if (game_state == USER_FALSE) {

    
    tft.setCursor(0, 2, 1);
    tft.setTextSize(1);
    tft.printf("Score: %d\nNumber Correct: %d\nNumber Incorrect: %d\n", score, correct_num, incorrect_num);     
    //tft.println("Long Press to end game");   

    if(bv1 == 1) {
      game_state = Q_DISPLAY;  
      char host[] = "608dev-2.net";
      memset(request, 0, IN_BUFFER_SIZE);
      memset(response, 0, IN_BUFFER_SIZE);
      sprintf(request,"GET http://608dev-2.net/sandbox/sc/team16/trivia.py?scoreboard=false HTTP/1.1\r\n");
      strcat(request,"Host: 608dev-2.net\r\n"); //add more to the end
      strcat(request,"\r\n"); //add blank line!
      do_http_request(host, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, 0);

      char delim[2] = "\n";
      char* token;
      token = strtok(response, delim);
      strcpy(question, token);
      token = strtok(NULL, delim);
      strcpy(answer, token);

      Serial.println(question);
      Serial.println(answer);

      tft.fillScreen(TFT_BLACK); 
    } /*else if (bv1 == 2) {
      game_state = GAME_OVER;
      tft.fillScreen(TFT_BLACK); 
    }*/

  } else if (game_state == GAME_OVER) {
    if (correct_num == 0 && incorrect_num == 0) {
      game_state = Q_DISPLAY;
      game_timer = millis();
      myDFPlayer.play(alarm_sound);
    } else {
      myDFPlayer.pause();
      char body[300]; //for body
      sprintf(body,"user=%s&score=%d", USER, score);//generate body, posting to User, 1 step
      int body_len = strlen(body); //calculate body length (for header reporting)
      memset(request, 0, IN_BUFFER_SIZE);
      memset(response, 0, OUT_BUFFER_SIZE);
      sprintf(request,"POST http://608dev-2.net/sandbox/sc/team16/trivia.py HTTP/1.1\r\n");
      strcat(request,"Host: 608dev-2.net\r\n");
      strcat(request,"Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request+strlen(request),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request,"\r\n"); //new line from header to body
      strcat(request,body); //body
      strcat(request,"\r\n"); //new line
      do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
            
      //tft.println(response);
      game_state = IDLE;
      score = 0;
      correct_num = 0;
      incorrect_num = 0;
      tft.fillScreen(TFT_BLACK); 
      return 1;
    }
  }
  return 0;
}

