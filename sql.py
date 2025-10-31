import mysql.connector
from mysql.connector import Error

db = mysql.connector.connect(
    host="localhost",
    user="type",
    password="random@123",
    database="smart_lock"
)
cursor = db.cursor()

def insert(uid, key, file_name):
    try:
        sql = "INSERT INTO lock_system (UID, KEYPAD, FILE) VALUES (%s, %s, %s)"
        values = (uid, key, file_name)
        cursor.execute(sql, values)
        db.commit()
    except Error as e:
        if e.errno == 1062:
            print("Duplicate entry detected")
        else:
            print("Error while inserting:", e)

def search(uid, key):
    try:
        sql = "SELECT FILE FROM lock_system WHERE UID = %s AND KEYPAD = %s"
        values = (uid, key)
        cursor.execute(sql, values)
        result = cursor.fetchone()
        return result
    except Error as e:
        print("Error while searching:", e)
        return None
