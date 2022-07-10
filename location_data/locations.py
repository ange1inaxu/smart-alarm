import sqlite3
import time
import datetime

def bounding_box(point_coord, box):
    p1, p2, p3, p4 = box
    x_min = min(p1[0], p2[0], p3[0], p4[0])
    x_max = max(p1[0], p2[0], p3[0], p4[0])
    y_min = min(p1[1], p2[1], p3[1], p4[1])
    y_max = max(p1[1], p2[1], p3[1], p4[1])
    return x_min <= point_coord[0] <= x_max and y_min <= point_coord[1] <= y_max

def translate(point, origin):
    return (point[0] - origin[0], point[1] - origin[1])

def within_area(point_coord, poly):
    # translate the polygon and the point
    for i in range(len(poly)):
        poly[i] = (poly[i][0]-point_coord[0], poly[i][1]-point_coord[1])
    point_coord = (0,0)
    
    # calculate if the point is in the polygon
    num_cross = 0
    for i in range(len(poly)):
        p1 = poly[i-1]
        p2 = poly[i]
        if sign(p1[1]) != sign(p2[1]):
            if sign(p1[0]) == 1 and sign(p2[0]) == 1:
                num_cross += 1
            elif sign(p1[0]) == -1 and sign(p2[0]) == -1:
                pass
            elif sign(p1[0]) != sign(p2[0]):
                x_inters = (p1[0]*p2[1]-p2[0]*p1[1]) / (p2[1]-p1[1])
                if sign(x_inters) == 1:
                    num_cross += 1
    return num_cross%2 == 1

# (lon, lat)
locations={
    "Student Center":[(-71.095863,42.357307),(-71.097730,42.359075),(-71.095102,42.360295),(-71.093900,42.359340),(-71.093289,42.358306)],
    "Dorm Row":[(-71.093117,42.358147),(-71.092559,42.357069),(-71.102987,42.353866),(-71.106292,42.353517)],
    "Simmons/Briggs":[(-71.097859,42.359035),(-71.095928,42.357243),(-71.106356,42.353580),(-71.108159,42.354468)],
    "Boston FSILG (West)":[(-71.124664,42.353342),(-71.125737,42.344906),(-71.092478,42.348014),(-71.092607,42.350266)],
    "Boston FSILG (East)":[(-71.092409,42.351392),(-71.090842,42.343589),(-71.080478,42.350900),(-71.081766,42.353771)],
    "Stata/North Court":[(-71.091636,42.361802),(-71.090950,42.360811),(-71.088353,42.361112),(-71.088267,42.362476),(-71.089769,42.362618)],
    "East Campus":[(-71.089426,42.358306),(-71.090885,42.360716),(-71.088310,42.361017),(-71.087130,42.359162)],
    "Vassar Academic Buildings":[(-71.094973,42.360359),(-71.091776,42.361770),(-71.090928,42.360636),(-71.094040,42.359574)],
    "Infinite Corridor/Killian":[(-71.093932,42.359542),(-71.092259,42.357180),(-71.089619,42.358274),(-71.090928,42.360541)],
    "Kendall Square":[(-71.088117,42.364188),(-71.088225,42.361112),(-71.082774,42.362032)],
    "Sloan/Media Lab":[(-71.088203,42.361017),(-71.087044,42.359178),(-71.080071,42.361619),(-71.082796,42.361905)],
    "North Campus":[(-71.11022,42.355325),(-71.101280,42.363934),(-71.089950,42.362666),(-71.108361,42.354484)],
    "Technology Square":[(-71.093610,42.363157),(-71.092130,42.365837),(-71.088182,42.364188),(-71.088267,42.362650)]
}

def sign(x):
    if x > 0: return 1
    elif x == 0: return 0
    else: return -1

def get_area(point_coord,locations):
    for key in locations:
        if within_area(point_coord, locations[key]):
            return key
    return "Off Campus"

def get_mdpt(location):
    x, y = 0, 0
    for pt in location:
        x += pt[0]
        y += pt[1]
    return (x/len(location), y/len(location))

def request_handler(request):
    if request["method"] == "GET":
        # parse user and location name from request
        user = ""
        loc_name = ""
        try:
            user = request['values']['username']
        except:
            return "Error: user is missing or not a string"
        try:
            loc_name = request['values']['location']
        except:
            return "Error: location is missing or not a string"
        
        # create location database
        loc_db = "/var/jail/home/team16/location_info/location_info.db"
        conn = sqlite3.connect(loc_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS location_info (user text, location text, lon real, lat real);''')

        # check if database is empty or not
        not_empty = c.execute('''SELECT lon, lat FROM location_info WHERE user==? AND location==?;''',(user, loc_name,)).fetchall()
        
        # if database is empty, add default locations using the locations dictionary
        if not not_empty:
            for loc in locations:
                lon_mid, lat_mid = get_mdpt(locations[loc])
                c.execute('''INSERT into location_info VALUES (?,?,?,?);''',(user, loc, lon_mid, lat_mid))
        
        # get location info from database
        loc_info = c.execute('''SELECT lon, lat FROM location_info WHERE user==? AND location==?;''',(user, loc_name,)).fetchall()
        
        conn.commit()
        conn.close()
        # return midpoint
        if not loc_info:
            return "Error: location not in database"
        else:
            return loc_info

    elif request["method"] == "POST":
        user = ""
        loc_name = ""
        lon = 0
        lat = 0
        response = request['form']
        try:
            user = response['username']
        except:
            return "Error: user is missing or not a string"
        try:
            loc_name = response['location']
        except:
            return "Error: location is missing or not a string"
        try:
            lon = float(response['lon'])
            lat = float(response['lat'])
        except Exception as e:
            # return e here or use your own custom return message for error catch
            # be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
            return "Error: lat, lon are missing or not numbers"
        
        # create location database
        loc_db = "/var/jail/home/team16/location_info/location_info.db"
        conn = sqlite3.connect(loc_db)
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS location_info (user text, location text, lon real, lat real);''')

        # insert location information into location database
        c.execute('''INSERT into location_info VALUES (?,?,?,?);''',(user, loc_name, lon, lat))

        conn.commit()
        conn.close()
        return "location info entered"
