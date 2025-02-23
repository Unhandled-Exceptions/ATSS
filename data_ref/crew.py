import csv
import random
from faker import Faker

# Initialize Faker instances for different name styles
faker = Faker()
Faker.seed(42)  # Ensure reproducibility

airlines = ["Air India", "IndiGo", "Vistara", "SpiceJet", "Emirates", "Lufthansa", "British Airways"]

def generate_crew_data(crew_per_airline):
    crew_data = []
    
    for airline in airlines:
        for _ in range(crew_per_airline):
            designation = "attendant" if random.random() < 0.7 else "pilot"
            
            # Assign names based on airline region
            if airline in ["Air India", "IndiGo", "Vistara", "SpiceJet"]:
                name = Faker('en_IN').name()
            elif airline == "Emirates":
                name = Faker('ar_AE').name()
            elif airline == "Lufthansa":
                name = Faker('de_DE').name()
            else:
                name = Faker('en_GB').name()
            
            crew_data.append([name, designation, airline, 0])
    
    return crew_data

def save_to_csv(data, filename="crew.csv"):
    with open(filename, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["name", "designation", "airline", "hours_worked"])
        writer.writerows(data)

def main():
    crew_per_airline = int(input("Enter number of crew members per airline: "))
    crew_data = generate_crew_data(crew_per_airline)
    save_to_csv(crew_data)
    print(f"CSV file '{'crew.csv'}' generated successfully with {crew_per_airline * len(airlines)} entries.")

if __name__ == "__main__":
    main()