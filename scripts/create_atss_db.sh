#! /bin/bash
SQL_QUERY="
CREATE TABLE IF NOT EXISTS crew (
    crew_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    designation TEXT,
    airline TEXT,
    hours_worked INTEGER
);
CREATE TABLE IF NOT EXISTS flights (
    flight_id TEXT PRIMARY KEY,
    airline TEXT,
    origin TEXT,
    destination TEXT,
    departure_time TEXT,
    arrival_time TEXT,
    aircraft_type TEXT,
    priority_level INTEGER,
    runway_time TEXT
);
CREATE TABLE IF NOT EXISTS alloted (
    allot_id INTEGER PRIMARY KEY,
    flight_id TEXT,
    allotted_time INTEGER,
    FOREIGN KEY (flight_id) REFERENCES flights (flight_id)
);
CREATE TABLE IF NOT EXISTS crew_allot (
    allot_id INTEGER PRIMARY KEY,
    crew_name TEXT,
    allotted_time INTEGER,
    FOREIGN KEY (crew_name) REFERENCES crew (name)
);"

sqlite3 "data/atss.db" "$SQL_QUERY" && echo "Unified database created."
