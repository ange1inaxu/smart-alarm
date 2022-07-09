void set_voice_time() {
  if (!time_set) {
    // tft.setCursor(0, 0, 1);
    tft.setCursor(5, 10, 1);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Please set your alarm time:  \n");
    tft.println("  Hold Button 34 and say the time and a.m./p.m. (e.g. 7 o'clock a.m. or 7:30 p.m.)\n");
    tft.println("  Press Button 45 to confirm. \n");

    int bv1 = button1.update();
    if (bv1 == 1 && !(!strcmp(transcript,"") || !strcmp(transcript,"\""))) {
      // set alarm time
      char TIME[20] = "";
      for (int i=1; i<strlen(transcript)-1; i++) {
        TIME[i-1] = transcript[i];
      }
      TIME[strlen(transcript)-2] = '\0';
      formatVoiceTime(TIME, alarm_time);
      // Serial.println(alarm_time);
      if (millis()-timer < DISPLAY_TIME) {
        tft.setCursor(0, 60, 1);
        tft.printf("%s             ", transcript);
      } else {
        time_set = true;
        return;
        // memset(transcript, 0, sizeof(transcript));
        // timer = millis();
        // tft.fillScreen(TFT_BLACK);
      }      
    } else {
      listen();
    }
    // if (!strcmp(transcript,"") || !strcmp(transcript,"\"")) {
    //   listen();
    //   // tft.println("I'm listening...\n");
    // } else {
    //   char TIME[20] = "";
    //   for (int i=1; i<strlen(transcript)-1; i++) {
    //     TIME[i-1] = transcript[i];
    //   }
    //   TIME[strlen(transcript)-2] = '\0';
    //   formatVoiceTime(TIME, alarm_time);
    //   // Serial.println(alarm_time);
    //   if (millis()-timer < DISPLAY_TIME) {
    //     tft.setCursor(0, 60, 1);
    //     tft.printf("%s             ", transcript);
    //   } else {
    //     time_set = true;
    //     // memset(transcript, 0, sizeof(transcript));
    //     // timer = millis();
    //     // tft.fillScreen(TFT_BLACK);
    //   }
    // }
  }
}

void listen() {
  button_state_voice = digitalRead(PIN);
  if (!button_state_voice && button_state_voice != old_button_state) {
    Serial.println("listening...");

    // set display cursor
    if (state == 0) {
      tft.setCursor(5, 100, 1);
    } else if (state >= 1 && state <= 4) {
      tft.setCursor(5, 70, 1);
    } else if (state == 5) {
      tft.setCursor(5, 80, 2);
    }
    tft.println("I'm listening... \n");

    record_audio();
    Serial.println("sending...");

    // set display cursor
    if (state == 0) {
      tft.setCursor(5, 100, 1);
    } else if (state >= 1 && state <= 4) {
      tft.setCursor(5, 70, 1);
    } else if (state == 5) {
      tft.setCursor(5, 80, 2);
    }
    tft.println("Processing...    \n");
    
    Serial.print("\nStarting connection to server...");
    delay(100);
    bool conn = false;
    for (int i = 0; i < 10; i++) {
      int val = (int)client.connect(VOICE_SERVER, 443, 4000);
      Serial.print(i); Serial.print(": "); Serial.println(val);
      if (val != 0) {
        conn = true;
        break;
      }
      Serial.print(".");
      delay(100);
    }
    if (!conn) {
      Serial.println("Connection failed!");
      return;
    } else {
      Serial.println("Connected to server!");
      Serial.println(client.connected());
      int len = strlen(speech_data);
      // Make a HTTP request:
      client.print("POST /v1/speech:recognize?key="); client.print(API_KEY); client.print(" HTTP/1.1\r\n");
      client.print("Host: speech.googleapis.com\r\n");
      client.print("Content-Type: application/json\r\n");
      client.print("cache-control: no-cache\r\n");
      client.print("Content-Length: "); client.print(len);
      client.print("\r\n\r\n");
      int ind = 0;
      int jump_size = 1000;
      char temp_holder[jump_size + 10] = {0};
      Serial.println("sending data");
      while (ind < len) {
        delay(50);//experiment with this number!
        //if (ind + jump_size < len) client.print(speech_data.substring(ind, ind + jump_size));
        strncat(temp_holder, speech_data + ind, jump_size);
        client.print(temp_holder);
        ind += jump_size;
        memset(temp_holder, 0, sizeof(temp_holder));
      }
      client.print("\r\n");
      //Serial.print("\r\n\r\n");
      Serial.println("Through send...");
      unsigned long count = millis();
      while (client.connected()) {
        Serial.println("IN!");
        String line = client.readStringUntil('\n');
        Serial.print(line);
        if (line == "\r") { //got header of response
          Serial.println("headers received");
          break;
        }
        if (millis() - count > VOICE_RESPONSE_TIMEOUT) break;
      }
      Serial.println("");
      Serial.println("Response...");
      count = millis();
      while (!client.available()) {
        delay(100);
        Serial.print(".");
        if (millis() - count > VOICE_RESPONSE_TIMEOUT) break;
      }
      Serial.println();
      Serial.println("-----------");
      memset(response, 0, sizeof(response));
      while (client.available()) {
        char_append(response, client.read(), VOICE_OUT_BUFFER_SIZE);
      }
      //Serial.println(response); //comment this out if needed for debugging
      char* trans_id = strstr(response, "transcript");
      if (trans_id != NULL) {
        char* foll_coll = strstr(trans_id, ":");
        char* starto = foll_coll + 2; //starting index
        char* endo = strstr(starto + 1, "\""); //ending index
        int transcript_len = endo - starto + 1;
        // char transcript[100] = {0};
        memset(transcript, 0, sizeof(transcript));
        strncat(transcript, starto, transcript_len);
        Serial.println(transcript);
        // tft.println(transcript);
      }
      Serial.println("-----------");
      client.stop();
      Serial.println("done");
      if (state == 0) {
        tft.setCursor(5, 100, 1);
      } else if (state >= 1 && state <= 4) {
        tft.setCursor(5, 70, 1);
      } else if (state == 5) {
        tft.setCursor(5, 80, 2);
      }
      // tft.println("                     ");
      tft.printf("%s             ", transcript);
      timer = millis();
    }
  }
  old_button_state = button_state_voice;
}

//function used to record audio at sample rate for a fixed nmber of samples
void record_audio() {
  int sample_num = 0;    // counter for samples
  int enc_index = strlen(VOICE_PREFIX) - 1;  // index counter for encoded samples
  float time_between_samples = 1000000 / SAMPLE_FREQ;
  int value = 0;
  char raw_samples[3];   // 8-bit raw sample data array
  memset(speech_data, 0, sizeof(speech_data));
  sprintf(speech_data, "%s", VOICE_PREFIX);
  char holder[5] = {0};
  Serial.println("starting");
  uint32_t text_index = enc_index;
  uint32_t start = millis();
  time_since_sample = micros();
  while (!button_state_voice && millis()-start<=5000) { //read in NUM_SAMPLES worth of audio data // sample_num < NUM_SAMPLES
    button_state_voice = digitalRead(PIN);
    value = analogRead(AUDIO_IN);  //make measurement
    raw_samples[sample_num % 3] = mulaw_encode(value - 1800); //remove 1.5ishV offset (from 12 bit reading)
    sample_num++;
    if (sample_num % 3 == 0) {
      base64_encode(holder, raw_samples, 3);
      strncat(speech_data + text_index, holder, 4);
      text_index += 4;
    }
    // wait till next time to read
    while (micros() - time_since_sample <= time_between_samples); //wait...
    time_since_sample = micros();
  }
  Serial.println(millis() - start);
  sprintf(speech_data + strlen(speech_data), "%s", VOICE_SUFFIX);
  Serial.println("out");
}

int8_t mulaw_encode(int16_t sample) {
  //paste the fast one here.
  const uint16_t MULAW_MAX = 0x1FFF;
  const uint16_t MULAW_BIAS = 33;
  uint16_t mask = 0x1000;
  uint8_t sign = 0;
  uint8_t position = 12;
  uint8_t lsb = 0;
  if (sample < 0)
  {
    sample = -sample;
    sign = 0x80;
  }
  sample += MULAW_BIAS;
  if (sample > MULAW_MAX)
  {
    sample = MULAW_MAX;
  }
  for (; ((sample & mask) != mask && position >= 5); mask >>= 1, position--)
      ;
  lsb = (sample >> (position - 4)) & 0x0f;
  return (~(sign | ((position - 5) << 4) | lsb));
}