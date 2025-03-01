import sqlite3
import csv

# Define the database file
DATABASE_FILE = "data/crew_big.db"
CSV_FILE = "data/crew_big.csv"

# Connect to SQLite database (or create it if it doesn't exist)
conn = sqlite3.connect(DATABASE_FILE)
cursor = conn.cursor()

# Create the flights table
cursor.execute('''
CREATE TABLE IF NOT EXISTS crew (
    name TEXT PRIMARY KEY,
    designation TEXT,
    airline TEXT,
    hours_worked INTEGER
)
''')

# Read and insert data from CSV
with open(CSV_FILE, "r") as file:
    reader = csv.reader(file)
    next(reader)  # Skip header row
    for row in reader:
        cursor.execute("INSERT INTO crew VALUES (?, ?, ?, ?)", row)

# Commit changes and close the connection
conn.commit()
conn.close()

print("Data successfully inserted into crew db")
