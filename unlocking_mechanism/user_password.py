import sqlite3
import time


def request_handler(request):
    if request["method"] == "GET":
        try:
            input_username = request['values']['username']
            input_password = request['values']['password']

            
            
            user_db = "/var/jail/home/team16/user_info/user_info.db"
            conn = sqlite3.connect(user_db)
            c = conn.cursor()
            c.execute('''CREATE TABLE IF NOT EXISTS user_info (user text,password text);''')
            
            #if username and password match, return True; otherwise, False
            found = False
            all_info = c.execute('''SELECT user, password FROM user_info;''').fetchall()
        
            for name, password in all_info:
                if input_username == name and input_password == password:
                    found = True
            conn.commit()
            conn.close()
            return found
            
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
        
        
        


