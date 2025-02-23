import csv
import random
from datetime import datetime, timedelta

# Predefined list of Indian and major international airports
indian_airports = [
    "DEL", "BOM", "MAA", "BLR", "HYD", "CCU", "AMD", "COK", "GOI", "PNQ"
]
international_airports = [
    "JFK", "LHR", "DXB", "SIN", "SYD", "CDG", "HKG", "LAX", "FRA", "NRT"
]

airlines = ["Air India", "IndiGo", "Vistara", "SpiceJet", "Emirates", "Lufthansa", "British Airways"]
aircraft_types = ["Boeing 737", "Airbus A320", "Boeing 777", "Airbus A380", "Embraer E190"]

fields = ["flight_id", "airline", "origin", "destination", "departure_time", "arrival_time", "aircraft_type", "priority_level"]
data = []

current_airport = input("Enter the current airport IATA code (or type 'random' to choose one): ")
if current_airport.lower() == "random":
    current_airport = random.choice(indian_airports)
print(f"Current airport: {current_airport}")

start = input("Enter start time [in hh:mm 24hrs format] : ")
end = input("Enter end time [in hh:mm 24hrs format] : ")
n = int(input("Enter the number of flights within the time: "))

start_time = datetime.strptime(start, "%H:%M")
end_time = datetime.strptime(end, "%H:%M")
time_now = start_time

time_range = (end_time - start_time).total_seconds() // 60
max_interval = int(time_range // n)

print(f"Current Airport: {current_airport}")
print(f"Max interval between flights: {max_interval} minutes")

for i in range(n):
    airline = random.choice(airlines)
    aircraft = random.choice(aircraft_types)
    
    if random.random() < 0.7:  # 70% chance of domestic flight
        other_airport = random.choice([ap for ap in indian_airports if ap != current_airport])
        origin, destination = (current_airport, other_airport) if random.random() < 0.5 else (other_airport, current_airport)
        flight_duration = random.randint(60, 240)  # 1 to 4 hours
        priority = 3  # Domestic
    else:  # International flight
        other_airport = random.choice(international_airports)
        origin, destination = (current_airport, other_airport) if random.random() < 0.5 else (other_airport, current_airport)
        flight_duration = random.randint(300, 900)  # 5 to 15 hours
        priority = 2  # International
    
    if random.random() < 0.05:  # 5% chance of emergency
        priority = 1
    
    departure_time = time_now.strftime("%H:%M")
    arrival_time = (time_now + timedelta(minutes=flight_duration)).strftime("%H:%M")
    
    data.append([f"FL{i+1:03d}", airline, origin, destination, departure_time, arrival_time, aircraft, priority])
    
    interval = random.randint(15, max_interval)
    time_now += timedelta(minutes=interval)
    if time_now > end_time:
        break

with open("flights.csv", 'w', newline='') as file:
    csv_writer = csv.writer(file)
    csv_writer.writerow(fields)
    csv_writer.writerows(data)

print("Flights data saved to flights.csv")
