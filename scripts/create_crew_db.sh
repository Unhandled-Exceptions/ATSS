SQL_QUERY="
CREATE TABLE IF NOT EXISTS crew (
    name TEXT PRIMARY KEY,
    designation TEXT,
    airline TEXT,
    hours_worked INTEGER
);"

sqlite3 "data/crew.db" "$SQL_QUERY"

echo "Done !!"
