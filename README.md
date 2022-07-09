# Smart Alarm Clock (Big Benji)

Big Benji will be catered towards the user’s daily schedule and music preferences, making the daunting task of “waking up” less sufferable. To protect the user’s privacy, the user will input their password by typing into a keypad to unlock Big Benji. From here, there will be several different alarm clock wakeup modes:

1. User Input
2. Rise & Shine (Photoresistor)
3. Location Input
4. Calendar Syncing
5. Voice Control
6. Weekly Scoreboard

Mode 1 allows the user to input their alarm time. Mode 2 uses the photoresistor to detect natural sunlight. Above a certain brightness threshold, the alarm will go off. Mode 3 has a database containing MIT Campus locations (along with their longitude and latitude values). The user can select a location, mode of transportation (ex. walking, driving), and input the arrival time. Using this information, we use the Google Maps API to calculate the optimal alarm time. Mode 4 parses one’s calendar to find the first event of the next day and its associated time and location. From here, we use a similar procedure as in Mode 3 to calculate the alarm time. Mode 5 uses voice control, which integrates with the previous modes: instead of manually inputting a time, the user can press a button and verbalize the desired time.

To ensure that the user does wake up, we will NOT be incorporating a snooze button :,). Instead, we will have a wakeup Trivia game that lasts for 2 minutes. Only after 2 minutes will the alarm clock be silenced. As for Mode 6, the user’s daily scores from the week leading up to the current day will be displayed.

![Full System Video](https://youtu.be/1d8xW6Z0Muc)

Functionality
===============================================================================

Unlocking Mechanism
-------------------------------------------------------------------------------
**Server-side**<br>
Within the team server, there is a table in the user information database called user_info that looks like the one drawn below. The purpose of this table is to hold usernames and their corresponding passwords. This table is searchable (able to return whether or not matching username and password exist within the table) and is mutable (new username and corresponding passwords are able to be inserted).

|  User            |  Password         |
|------------------|:-----------------:|
| username_example |  password_example |

There is another table in the user information database called pw_attempts that keeps track of incorrect password attempts by storing the username and the timestamps as shown below. The purpose of this table is to disable the device after too many incorrect tries within a certain timeframe. This table is searchable (able to return the usernames and timestamps of incorrect attempts) and mutable (new usernames and timestamps of incorrect password attempts can be inserted).

|  User            |  Timing           |
|------------------|:-----------------:|
| username_example |  timing_example   |

<br><br>
**ESP32-side**<br>
The user is presented with a keypad and prompted to enter their username and password. Once entered, we verify them through requests to the server-side and display to the user the appropriate response. If the login is verified, we then transition to the user set-up detailed below.


Select Mode and Alarm Song
-------------------------------------------------------------------------------
Upon ESP startup, the user is prompted to select one of six modes for the alarm clock (detailed below). A short button click wired by I/O 34 is associated with a change in mode (ranges from 0 to 5), and a long button click confirms user selection of the mode. From here, the user will be prompted to select one of three songs for an alarm (“Feel Special” by Twice, “22” by Taylor Swift, and the Silk ringtone). The IMU will sense acceleration, and the user can rotate the breadboard to alternate between alarm songs. A short button click wired to I/O 45 confirms user selection of the song. From here, the corresponding mode is entered.


Mode 1: User input 
-------------------------------------------------------------------------------
In the user input mode, the user is asked to enter the time (in military time) at which they want the alarm to go off. The mechanism for entering is the same IMU mechanism with which the user selects an alarm song.


Mode 2: Rise & Shine
-------------------------------------------------------------------------------
We use the analog readings of the brightness increase from the photoresistor. Using the resistance extractor and brightness extractor equations, the lux can be calculated. Above a certain lux threshold, the alarm goes off and the wakeup game begins.


Mode 3: Location input
-------------------------------------------------------------------------------
**Google Maps API**<br>
We utilize the Google Distance Matrix API to calculate the distance and estimated time travel from the user’s origin to destination. We allow the user to specify a travel mode and provide it to the API as well so that we can better calculate the ETA.

**Integration with ESP**<br>
The user is asked to enter in their origin and their destination, as well as the time at which they must arrive at their destination. Then, the user is asked to enter in their mode of transport and how long it takes for them to get ready in the morning. All of these mechanisms use the same IMU mechanism described above where the user rotates the device and performs a short button click wired to I/O 45 to confirm their selection of a number/location, except in the entering of how long it takes the user to get ready, a long button click wired to I/O 45 confirms that the user has fully entered in the duration. Based on the information that the user has entered, the time at which the user must wake up to get to their destination on time is calculated, and the alarm time is set at that time.


Mode 4: Calendar Syncing
-------------------------------------------------------------------------------
**Nylas Calendar API**<br>
The “calendar mode” automatically syncs with the user’s Google Calendar via a third party library, Nylas. We use Nylas to read the user’s first event of tomorrow and use its location combined with the Google Distance Matrix API to get information about estimated time travel. That way, we can then use this information to calculate when to wake the user up tomorrow morning. 

**Integration with ESP**<br>
The user is asked to enter their origin location. Then, the user is asked to enter in their mode of transport and how long it takes for them to get ready in the morning. All of these mechanisms use the same IMU mechanism described above where the user rotates the device and performs a short button click wired to I/O 45 to confirm their selection of a number/location, except in the entering of how long it takes the user to get ready, a long button click wired to I/O 45 confirms that the user has fully entered in the duration. Then, the location and start time of the user’s first event of the next day is extracted from their calendar, and based on this and the information that the user has entered, the time at which the user must wake up to get to their destination on time is calculated, and the alarm time is set at that time.



Mode 5: Voice Control
-------------------------------------------------------------------------------
In the voice control mode, the user can use voice commands to set their alarm time. While holding a button, the user can say their alarm time and then confirm it by pressing another button. Once the alarm time is reached, the alarm goes off and the wakeup game starts.

The system handles this using a function that transcribes audio messages. While the user holds a button, audio is recorded, processed via base64-encoding (required by the Google Speech API) and mu-law encoding (rescales the 16-bit sample to 8 bits), and then stored in memory. Once the audio message is finished recording, the program sends the audio to the Google Speech API, awaits the JSON object response, and pulls the transcript from it.


Wakeup Trivia Game 
-------------------------------------------------------------------------------
After parsing the questions and answers (True/False) from the Trivia API, the question statements are displayed to the LCD screen. The user answers with True (short press button I/O 39) and False (short press button I/O 38). The counts of incorrect, correct, and total scores are updated continuously until the game ends in 2 minutes. At this point, we do a quick check to see if the number of incorrect and correct answers are not both 0. If so, the game would end and we would store the user’s score in the database on the server for later use in our weekly scoreboard. Otherwise, if both values are 0, this would denote that the user slept through their alarm. Therefore, the alarm clock and trivia game would just repeat the 2 minute cycle again.


Weekly Recap
-------------------------------------------------------------------------------
Once the trivia game ends, a unique weekly recap for the user is shown on the LCD screen. This weekly recap shows the user’s scores on the wakeup game over the past seven days. 

Motion Sensing
-------------------------------------------------------------------------------
Using the IMU, the magnitude of acceleration is calculated. When no acceleration is sensed, after 10 seconds, the screen blacks out, with the exception of when the alarm is going off and the game is in progress. Immediately when motion is sensed again, the screen will brighten back up, displaying the time.



Detailed Code Layout
===============================================================================

ESP32 Side
-------------------------------------------------------------------------------

### alarm_clock.ino

This is the main file of our project. It contains all of the global constants, as well as the setup and loop functions that run on the ESP. Inside this file, we also have a few helper functions for retrieving, displaying, and formatting the current local time that serve as the backbone for our clock. In the loop function, we first handle the user login; then, once the user has properly logged in, we handle which mode the user wants to set the alarm with, and the song that the user wants to play as the alarm sound. Based on the user’s selection of mode, a different helper function (in another file described below) is called to handle the setting of the alarm. Finally, once the alarm has been set, the alarm time will be compared to the current local time, and if they are the same, the alarm will be triggered, the trivia game will begin, and the user’s selected song will play.

### calendar.ino

This file contains helper functions that handle the calendar mode of our alarm clock. First, a list of the user’s locations are retrieved using a GET request to a file on our server, location_info/all_locations.py. Then, the user is prompted to enter their starting location for the next morning, as well as their mode of transport, and how many minutes it takes for them to get ready in the morning. Based on this information, a POST request to location_info/calendar.py is made to get back the duration that it will take the user to travel from their starting location to the location of their first event on their calendar for the next morning, as well as the time of the user’s first event, and the time that the user needs to wake up is then calculated and set as the alarm time.

### location.ino

This file contains helper functions that handle the location mode of our alarm clock. First, a list of the user’s locations are retrieved using a GET request to a file on our server, location_info/all_locations.py. Then, the user is prompted to enter in their starting location and ending location for the next morning, as well as the time they must arrive at their ending location, their mode of transport, and how many minutes it takes for them to get ready in the morning. Based on this information, a POST request to location_info/schedule.py is made to get back the duration that it will take the user to travel from their starting to their ending location, and the time that the user needs to wake up is then calculated and set as the alarm time.
### photoresistor.ino

This file contains helper functions that contains physics computations, such as finding the resistance and brightness for our Rise & Shine Mode.

### voice_recognition.ino

This file handles transcribing audio messages and allowing the user to record audio messages until the user is satisfied. While the user holds a button, audio is recorded, processed via base64-encoding (required by the Google Speech API) and mu-law encoding (rescales the 16-bit sample to 8 bits), and then stored in memory. Once the audio message is finished recording, the program sends the audio to the Google Speech API, awaits the JSON object response, and pulls the transcript from it.

### play_song.ino

This file contains a helper function that allows the user to select which song they want the alarm sound to be, and the song which the user selects is stored for playing when the alarm goes off. This helper function is called right after the user selects the mode in which they want to set the time of the alarm.

### support_functions.ino

This file contains helper functions, such as HTTP and HTTPS Requests, Wifi Object Builders, array slicing, and array appending.

### trivia.ino

This file handles the trivia game that will begin once the alarm time has been reached. First, the user’s selected song will begin playing on the MP3 player. Then, the Trivia game will begin by retrieving questions from the Trivia API. A state machine is used to retrieve the user’s answer (True or False); check the answer; and store the number of correct answers, incorrect answers, and total score. Once two minutes have passed, check that the user actually played (ie. the number of correct and incorrect answers are non-zero). If the user did play, then the MP3 is silenced the score is stored to the database on the server.

### set_time.ino

This file contains a helper function for allowing the user to enter in a time (in military format). This helper function is used in both the User Input and the Location modes of our alarm clock.

### keypad_password.ino

This file handles the login feature of our product with a simple state machine: USERNAME → PASSWORD → LOGIN. We prompt the user to enter their username, password and then verify those by making a request to the server-side and displaying the appropriate results. The username is built up by listening to key presses on the keypad. When the user hits the asterisk key, we transition to building up the password. The final asterisk key press leads us into the login state as described. 


Server Side
-------------------------------------------------------------------------------

### /user_info
This folder holds scripts and databases for the login feature that we describe below. 

#### pw_timeout.py
This is a Python script that handles GET/POST requests to user_info.db in order to create a timeout mechanism that locks the device for two minutes after six incorrect password attempts within two minutes from each other. The program handles a get request with the inputted username and password as parameters to check if the device should be unlocked, remain locked, or be disabled. It then creates two tables to store (1) the timestamps of the incorrect attempts and (2) the user’s information if these tables don’t already exist in the database. If the username isn’t found in the user_info table, then “Error: username not found” is returned. Otherwise, the program extracts the most recent incorrect attempt’s timestamp from the pw_attempts table. It also retrieves all the most recent incorrect attempts that are within two minutes from the most recent attempt. If less than or equal to two minutes has passed since the most recent incorrect attempt, the code checks the following. If the amount of most recent incorrect attempts is greater than or equal to 5, then "Smart Alarm Clock is disabled. Try again in 2 minutes." is returned. Otherwise, if the inputted password matches the user’s actual password, then “Access granted” is returned, and otherwise, the timestamp is inserted into the database and “Access denied” is returned. If more than two minutes has passed since the most recent incorrect attempt, then if the password is correct, then “Access granted” is returned and otherwise, the timestamp is inserted into the database and “Access denied” is returned. 

#### user_password.py
This is a Python script that handles user_info.db. If a database called user_info does not exist when a GET or POST request is made to this Python script, a database called user_info with columns of user (text) and password (text) are created before the following is executed. 
When a GET request is made to this Python script, the code will check that a username and password is specified. Otherwise, at this point it will return an error message. If both the username and password are specified, then the Python script will fetch all the usernames and passwords pairs that are currently in user_info. If a pair of username and password both match the specified username and password from the GET request, True is returned. Otherwise, False is returned.
When a POST request is made, a username and password must be specified with it. Then, this pair of username and password are inserted into the user_info database. Finally, a message is returned to indicate the success of entering the information into user_info.

#### user_info.db
This is a SQL database that holds information regarding the username and password of the user using the Smart Alarm clock as well as the username and timestamps of the user’s incorrect password attempts using two tables: user_info and pw_attempts.
The table called user_info is created when the first GET or POST request is made to user_password.py and pw_attempts.py. user_info contains two columns of text: user and password. The number of rows is not constrained and is mutable based on the number of POST requests made to user_password.py. Below is what user_info looks like:

| user     |            password       | 
|:--------:|:-------------------------:|
| my_user  |     		123      	   |


### /trivia
This folder holds scripts and databases for the trivia game feature that we describe below. 

#### trivia.py
This Python script contains the request handler to retrieve the Trivia question and answer from the API.
First we check if the request method is “GET.” From here, we check if request values contain a scoreboard parameter. If the scoreboard is False, we send a get request to the Trivia API for a medium level True or False question. We parse and return the result for the question and answer. However, if scoreboard is True, we connect to visits_db on our server and return the top scores in descending order. 

#### trivia_db.db
This is a SQL database that holds information regarding the username and trivia score of the user using the Smart Alarm clock. The table called dated_table is created when the first GET or POST request is made to trivia.py. dated_table contains three columns of text: user, their trivia score, and the date. The number of rows is not constrained and is mutable based on the number of POST requests made to trivia.py. Below is what dated_table looks like:


| user     |            score           | timing          |
|:--------:|:--------------------------:|:---------------:|
| my_user  |     123     			|     my_timestamp    	| 

### /location_info
This folder holds scripts and databases for the location and scheduling features that we describe below.

#### all_locations.py
This is a Python program that handles get and post requests to retrieve all the locations of a user.
The program returns all of the locations stored for a user by handling a get request with the username and location name as parameters. To do this, the program first creates a table called location_info in location_info.db to store the location information if the table doesn’t already exist in the database. If the table is empty, the default locations from Lab 05b are inserted into the database, specifically the username, location name, longitude of the midpoint, and latitude of the midpoint. Then, all of the locations for the user are retrieved from the database and returned as a list. This list of locations is then used for display on the ESP32 to allow the user to choose their desired starting and ending locations. 

#### locations.py
This is a Python program that handles get and post requests to retrieve and insert location data specific to the user.
The program returns the location’s coordinates if the location is in the user’s stored list of locations by handling a get request with the username and location name as parameters. To do this, the program first creates a table called location_info in location_info.db to store the location information if the table doesn’t already exist in the database. If the table is empty, the default locations from Lab 05b are inserted into the database, specifically the username, location name, longitude of the midpoint, and latitude of the midpoint. Then, the location’s coordinates are retrieved from the database and returned as a tuple containing the longitude and latitude, respectively. These coordinates are then used to calculate how long it will take for the user to travel from their starting location to their next location to help calculate the optimal time to set the alarm for.
The program inserts new locations into the database by handling a post request with the username, location name, longitude, and latitude as parameters. Similar to the get request, the program first creates a table called location_info in location_info.db to store the location information if the table doesn’t already exist in the database. Then, the location information from the request is inserted into the database, and “location info entered” is returned.

#### schedule.py
This file records the user’s daily schedule in a database table by keeping track of their first event of the day and the estimated time it takes to get there. 
The POST request uses parameters “user”, “start”, “end”, and “mode”. “User” specifies the person, and “start” and “end” tell us the origin and destination location. Lastly, “mode” specifies how they’re planning to travel to class the next day, either by 1) walking, 2) biking, or 3) public transit. 

We then query for the “start” and “end” lat/lon coordinates in “location.db”. If successful, we then pass these coordinates and the travel mode to the Google Distance Matrix API which returns to us the estimated time travel and distance. In the end, “calendar.py” will return 3 things: distance to destination (in km), estimated time travel (in minutes), and the time (military) of the user’s first class tomorrow. 

The GET request takes in no parameters and returns the last 20 entries in our schedule.db. The information recorded in schedule.db can be used in weekly summaries to display and track users’ progress.

#### calendar.py
This file works very similar to schedule.py, with the only difference being that it auto-syncs with the user’s Google Calendar through Nylas, a 3rd party library so that no “end” parameter is needed; the end location is calculated automatically by pulling the user’s first event tomorrow.

#### location_info.db
There are 2 tables within this database, location_info and schedule.
location_info: This table records the user’s different locations.  In total, there are columns “user”, “location”, “lon”, “lat”. Location is the actual name of the place (ie: “Vassar Academic Buildings”) and lon, lat are the coordinates. 
schedule: This table tracks the user’s travel information for each day. In total, there are columns “user”, “day”, “origin”, “destination”, “distance”, and “travel_time”. This information will tell us where the user starts their morning, where their first class is, and how long it’ll take to get there. 

### /weekly_database
This folder holds scripts and databases for recording users' morning stats which we describe below.

#### weekly_database.py
This is a Python script that handles weekly_database.db. If a database called weekly_database does not exist when a GET or POST request is made to this Python script, a database called weekly_database with columns of user (text), date (int), score (int), and timing (timestamp) are created before the following is executed. The user is simply text that represents the username of the current user. The date is represented as an integer such that mm/dd/yyyy is mmddyyyy. The score is from the wakeup game. The timing is the timestamp that the POST request was made.
When a GET request is made to this Python script, the code will check that a username and date (in the form of an integer) are specified. Otherwise, at this point it will return an error message. If both the username and date are specified, then the Python script will fetch all the usernames, date, score, and timing sets that are currently in weekly_database in order of descending timings. If a set has a matching username and date, then the score from that set will be returned. Otherwise, a message indicating that no such username and date pair exist will be returned.
When a POST request is made, the Python script will first check that a username, date, and score were specified. If not, then an error message is returned to indicate that the specified values were not included. Otherwise, this set of specified values and the timestamp will be inserted into the weekly_database into their respective columns. Finally, a message is returned to indicate the success of entering the information into weekly_database.

#### weekly_database.db
This is a SQL database that holds information regarding whether or not the user successfully woke up. The table called weekly_datbase is created when the first GET or POST request is made to weekly_database.py. weekly_database contains three columns: user, date, and score. The first column is text of the user’s username, the second is the data in the formatting of a number (as explained in the section called weekly_database.py), and the score is represented as a number (as explained in the section called weekly_database.py). The number of rows is not constrained and is mutable based on the number of POST requests made to weekly_database.py. Below is what weekly_database looks like:

| user     |            date           | score          |
|:--------:|:--------------------------:|:---------------:|
| my_user  |     mmddyyyy      			|     20    	| 

