#!/usr/bin/env python3

import sqlite3
import csv
import sys

if len(sys.argv) != 4:
    print("Usage example:\n./db_from_csv.py data/flights_small.csv data/crew_small.csv data/atss.db")
    exit()

# Define the database file
DATABASE_FILE = sys.argv[3]
FLIGHTS_FILE = sys.argv[1]
CREW_FILE = sys.argv[2]

# Connect to SQLite database (or create it if it doesn't exist)
conn = sqlite3.connect(DATABASE_FILE)
cursor = conn.cursor()

# Read and insert data from CSV
with open(FLIGHTS_FILE, "r") as file:
    reader = csv.reader(file)
    next(reader)  # Skip header row
    for row in reader:
        cursor.execute("INSERT INTO flights VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", row)

with open(CREW_FILE, "r") as file:
    reader = csv.reader(file)
    next(reader)  # Skip header row
    for row in reader:
        cursor.execute("INSERT INTO crew VALUES (?, ?, ?, ?)", row)

# Commit changes and close the connection
conn.commit()
conn.close()

print(f"Data successfully inserted into {DATABASE_FILE}")
