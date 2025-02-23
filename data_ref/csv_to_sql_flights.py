import sqlite3
import csv

# Define the database file
DATABASE_FILE = "flights_big.db"
CSV_FILE = "flights_big.csv"

# Connect to SQLite database (or create it if it doesn't exist)
conn = sqlite3.connect(DATABASE_FILE)
cursor = conn.cursor()

# Create the flights table
cursor.execute('''
CREATE TABLE IF NOT EXISTS flights (
    flight_id TEXT PRIMARY KEY,
    airline TEXT,
    origin TEXT,
    destination TEXT,
    departure_time TEXT,
    arrival_time TEXT,
    aircraft_type TEXT,
    priority_level INTEGER
)
''')

# Read and insert data from CSV
with open(CSV_FILE, "r") as file:
    reader = csv.reader(file)
    next(reader)  # Skip header row
    for row in reader:
        cursor.execute("INSERT INTO flights VALUES (?, ?, ?, ?, ?, ?, ?, ?)", row)

# Commit changes and close the connection
conn.commit()
conn.close()

print("Data successfully inserted into flights db")
