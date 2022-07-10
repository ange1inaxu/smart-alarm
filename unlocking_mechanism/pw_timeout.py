import sqlite3
import time
import datetime

def make_datetime_object(string_with_datetime):
    return datetime.datetime.strptime(string_with_datetime, '%Y-%m-%d %H:%M:%S.%f')

def request_handler(request):
    if request["method"] == "GET":
        try: 
            input_username = request['values']['username']
            input_password = request['values']['password']

            output = ""

            # create user_info database and user_info and pw_attempts tables
            user_db = "/var/jail/home/team16/user_info/user_info.db"
            conn = sqlite3.connect(user_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS pw_attempts (user text, timing timestamp);''')
            c.execute('''CREATE TABLE IF NOT EXISTS user_info (user text, password text);''')
            
            # find user's info in user_info database
            info = c.execute('''SELECT password FROM user_info WHERE user==?;''',(input_username,)).fetchone()

            # if username not found in user_info db, return false
            if len(info) == 0:
                output = "Error: username not found"
            
            # else, continue with the rest of the code
            else:
                # get most recent attempts within 2 min before most recent incorrect attempt
                most_recent_time = c.execute('''SELECT timing FROM pw_attempts ORDER BY timing DESC;''').fetchone()[0]
                most_recent_time = make_datetime_object(most_recent_time)
                two_minutes_ago = most_recent_time - datetime.timedelta(minutes = 2)
                most_recent_attempts = c.execute('''SELECT * FROM pw_attempts WHERE timing > ? ORDER BY timing DESC;''',(two_minutes_ago,)).fetchall()
                now = datetime.datetime.now()
                two_minutes_from_now = now - datetime.timedelta(minutes = 2)

                # if now <= 2 min since most recent
                if two_minutes_from_now <= most_recent_time:
                    if len(most_recent_attempts) >= 5:
                        output = "Smart Alarm Clock is disabled. Try again in 2 minutes."
                    else:
                        pw = info[0]
                        if input_password == pw:
                            output = "Access granted"
                        else:
                            c.execute('''INSERT into pw_attempts VALUES (?,?);''',(input_username, now))
                            output = "Access denied"
                # else
                else:
                    pw = info[0]
                    if input_password == pw:
                        output = "Access granted"
                    else:
                        c.execute('''INSERT into pw_attempts VALUES (?,?);''',(input_username, now))
                        output = "Access denied"
            
            conn.commit()
            conn.close()
            return output
        except:
            return "Error: no username and/or password"

    elif request["method"] == "POST":
        response = request['form']
        username = response['username']
        password = response['password']
        user_db = "/var/jail/home/team16/user_info/user_info.db"
        conn = sqlite3.connect(user_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS user_info (user text,password text);''')

        c.execute('''INSERT into user_info VALUES (?,?);''',(username, password))
        conn.commit()
        conn.close()
        return "user info entered"