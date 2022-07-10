void login(){
  // validate password
  sprintf(request_buffer_key, "GET /sandbox/sc/team16/user_info/pw_timeout.py?username=%s&password=%s HTTP/1.1\r\n", username, login_password);
  strcat(request_buffer_key, "Host: 608dev-2.net\r\n");
  strcat(request_buffer_key, "\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer_key, response_buffer_key, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  // print response
  Serial.println("The response:");
  Serial.println(response_buffer_key); //print to serial monitor
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(5,30,1);
  // tft.println(response_buffer_key);
  if (compare("Access granted", response_buffer_key)){
    loggedIn = 1;
  } else {
    tft.setCursor(5,50,1);
    tft.println("Press * to try again");
    // memset username, pw, and input_display
    // memset(username, 0, USERNAME_SIZE);
    // memset(login_password, 0, PASSWORD_SIZE);
    // memset(input_display, 0, PASSWORD_SIZE);
  }

  /*memset(username, 0, USERNAME_SIZE);
  memset(login_password, 0, PASSWORD_SIZE);
  memset(input_display, 0, PASSWORD_SIZE);*/
  return;
}


void handle_login(){
  char pressed_key = customKeypad.getKey();
  bool user_pressed_enter = pressed_key == ENTER;

  if(pressed_key){
    Serial.println(pressed_key);
  }
  
  // handle state changes
  switch(key_state){
    case USERNAME:
      if(key_state != last_state){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(5,10,1);
        tft.println("Welcome! Please enter your username and then hit the '*' key to confirm.");
        // tft.println("");
        last_state = key_state;
      }
      if (user_pressed_enter){ // user is done entering username - move on to entering password
        memset(input_display, 0, PASSWORD_SIZE);
        key_state = PASSWORD; 
      }
      else if (pressed_key){
        char_append(username, pressed_key, USERNAME_SIZE); // add letter to username
        sprintf(input_display, "%s", username);
      }
      break;

    case PASSWORD:
      if(key_state != last_state){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(5,10,1);
        tft.printf("Please enter the password for %s.", username);
        last_state = key_state;
      }
      if (user_pressed_enter){ // user is done with entering password, login now
        key_state = LOGIN;
      }
      else if (pressed_key){
        char_append(login_password, pressed_key, PASSWORD_SIZE); // add letter to password
        sprintf(input_display, "%s", login_password);
      }
      break;

    case LOGIN:
      if(key_state != last_state){
        last_state = key_state;
        login();
      }
      
      if (user_pressed_enter) {
        tft.fillScreen(TFT_BLACK);
        // memset username, pw, and input_display
        memset(username, 0, USERNAME_SIZE);
        memset(login_password, 0, PASSWORD_SIZE);
        memset(input_display, 0, PASSWORD_SIZE);
        key_state = USERNAME;
      }

      break;

  };

  tft.setCursor(5,70,1);
  tft.println(input_display);
}