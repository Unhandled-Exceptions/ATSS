SQL_QUERY="
CREATE TABLE IF NOT EXISTS flights (
    flight_id TEXT PRIMARY KEY,
    airline TEXT,
    origin TEXT,
    destination TEXT,
    departure_time TEXT,
    arrival_time TEXT,
    aircraft_type TEXT,
    priority_level INTEGER
);"

sqlite3 "data/flights.db" "$SQL_QUERY"

echo "Done !!"
