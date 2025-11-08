import mysql.connector
from mysql.connector import Error

db = mysql.connector.connect(
    host="localhost",
    user="type",
    password="random@123",
    database="locksystem"
)
cursor = db.cursor()

def search(encrypted_uid, hashed_pin):
    try:
        sql = "SELECT name FROM lockit WHERE encrypted_uid = %s AND hashed_pin = %s"
        values = (encrypted_uid, hashed_pin)
        cursor.execute(sql, values)
        result = cursor.fetchall()
        return result
    except Error as e:
        print("Error while searching:", e)
        return None
