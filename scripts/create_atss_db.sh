#! /bin/bash

# Ensure the data directory exists
mkdir -p data

# Define the SQL statements to create tables
SQL_QUERY="
-- Crew table definition
CREATE TABLE IF NOT EXISTS crew (
    crew_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,          -- Added NOT NULL constraint
    designation TEXT NOT NULL,   -- Added NOT NULL constraint
    airline TEXT NOT NULL,       -- Added NOT NULL constraint
    hours_worked INTEGER DEFAULT 0 -- Added DEFAULT 0
);

-- Flights table definition
CREATE TABLE IF NOT EXISTS flights (
    flight_id TEXT PRIMARY KEY,
    airline TEXT NOT NULL,       -- Added NOT NULL constraint
    origin TEXT NOT NULL,        -- Added NOT NULL constraint
    destination TEXT NOT NULL,   -- Added NOT NULL constraint
    departure_time TEXT NOT NULL CHECK(length(departure_time) = 4), -- Added check
    arrival_time TEXT NOT NULL CHECK(length(arrival_time) = 4),     -- Added check
    aircraft_type TEXT,
    priority_level INTEGER NOT NULL, -- Added NOT NULL constraint
    runway_time TEXT NOT NULL CHECK(length(runway_time) = 4)      -- Added NOT NULL constraint and check
);

-- Runway allotment table definition
CREATE TABLE IF NOT EXISTS alloted (
    allot_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flight_id TEXT NOT NULL,                     -- Added NOT NULL
    allotted_time TEXT NOT NULL CHECK(length(allotted_time) = 4), -- Added NOT NULL and check
    runway INTEGER NOT NULL CHECK(runway IN (1, 2, 3)), -- Added NOT NULL and check for valid runway numbers (assuming max 3)
    FOREIGN KEY (flight_id) REFERENCES flights (flight_id) ON DELETE CASCADE -- Added ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS crew_allotments (
  allotment_id INTEGER PRIMARY KEY AUTOINCREMENT,
  flight_id TEXT NOT NULL,
  crew_id INTEGER NOT NULL,
  assignment_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- Records when the assignment was made
  FOREIGN KEY (flight_id) REFERENCES flights(flight_id) ON DELETE CASCADE,
  FOREIGN KEY (crew_id) REFERENCES crew(crew_id) ON DELETE CASCADE
);


CREATE INDEX IF NOT EXISTS idx_flights_runway_time ON flights(runway_time);
CREATE INDEX IF NOT EXISTS idx_alloted_flight_id ON alloted(flight_id);
CREATE INDEX IF NOT EXISTS idx_alloted_runway_time ON alloted(runway, allotted_time);
CREATE INDEX IF NOT EXISTS idx_crew_designation_airline ON crew(designation, airline);
CREATE INDEX IF NOT EXISTS idx_crew_allotments_flight_id ON crew_allotments(flight_id);
CREATE INDEX IF NOT EXISTS idx_crew_allotments_crew_id ON crew_allotments(crew_id);
"

# Execute the SQL query against the database file
sqlite3 "data/atss.db" "$SQL_QUERY"

# Check the exit status of sqlite3
if [ $? -eq 0 ]; then
    echo "Database 'data/atss.db' tables checked/created successfully."
else
    echo "Error executing SQL commands for database 'data/atss.db'."
    exit 1 # Exit with an error code
fi

echo "Database setup script finished."