# Air Traffic Scheduling System

## Running

Clone the repo, and run
```
make && ./bin/atss atss
Ideal Test Cases
```
If atss.db doesn't exist,
```
chmod +x data_ref/db_from_csv.py scripts/create_atss_db.sh
./scripts/create_atss_db.sh
./data_ref/db_from_csv.py data/flights_small.csv data/crew_small.csv data/atss.db
```
```
rm data/atss.db
./scripts/create_atss_db.sh
./data_ref/db_from_csv.py data/flights_big.csv data/crew_big.csv data/atss.db
```
To reset the db to busy test case.
```
rm data/atss.db
./scripts/create_atss_db.sh
./data_ref/db_from_csv.py data/flights_busy.csv data/crew_small.csv data/atss.db
```
or
```
rm data/atss.db
./scripts/create_atss_db.sh
./data_ref/db_from_csv.py data/flights_busy_big.csv data/crew_small.csv data/atss.db
```

## Project Directory Structure:

~~~
ATSS/
│── src/                 # Source Files
│   │── main.c
│── lib/                 # 3rd Party Libraries
│   │── sqlite3.c
│   │── sqlite3.h
│── data/                # .csv and db files
│── scripts/             # scripts to generate db files
│── include/             # Project specific header files
│── bin/                 # Compiled binaries
│── obj/                 # Compiled object files
│── Makefile
~~~

> Note: bin/ obj/ are created on your pc temporarily while compiling.

## TODO
- [x] Make the allot button work
- [ ] Add delay handling GUI
- [ ] Add status field to the allot table.


## Useful commands

To see the status of the flights table
```
sqlite3 -table data/atss.db "select * from flights;"
```

## Algorithm for the alloter
```
We have 3 tables in the db, `flights`, `crew`, `alloted`.
Flights are sorted according to their `runway_time` and priority.
Iterate through `flights` table:
    if diff of `runway_time`of b/w current flight and last flight in `alloted` table < 15:
        - change runway and try again
        - all runways are blocked? then if the last flight in `alloted` is less priority, kick it into the `delay_pile`
        - otherwise put this current clight into the `delay_pile`
    Now if there is a valid allot spot:
        - loop through sorted `crew` table and assign to the flight

Allotment finished.

Ask the user for clarifications for the `delay_pile`:

    - Loop throught the `alloted` table again for any gaps after the "given" flight time:
        if such a gap exists, then ask user if they want to delay to that spot
    - Else say that there is no option but to cancel the flight.
Generate runway utilization and delay reports.
Done.
```


## Agorithm for crew

we have 3 tables `alloted`, `crew` & `crew_allot`
iterate through each row in `alloted`
    using a forloop, select 4 people (two pilots two attendants)
        if hoursworked > LIMIT choose next person,
        else
            add an entry to the `crew_allot` table with the flight id
            add the hours_worked to `crew`
Done.


## Finding time slots for delay

Params are `flight id` and current `runway_time`
Iterate through each runway
    Iterate through each flight after current `runway_time`,
        If a slot is found, display it, ask for confirmation.
        yes => break;
        no slot found, switch runway. and check again.
No runways are free, then say that the only option is to cancel.


## Instructions for Using GUI

Make sure gtk3 development packages are installed in your computer:
For ubuntu based systems:
```
sudo apt install libgtk-3-dev
```
For Arch:
```
sudo pacman -S gtk3
sudo pacman -S base-devel
```

Now run the make command, you could optionally do a "make clean" before this:
```
make
```
> It is suggested to reset the DB once before the transition to start in a clean state

## Resources for development

### Official Docs
- Overall Docs: https://docs.gtk.org/gtk3/
- Getting Started Guide (Very Useful !!) : https://docs.gtk.org/gtk3/getting_started.html

### VS Code Integration

> Note: Please make sure that your configuration is not automatically taken from the makefile. If VSCode asks you to do so just say no. If it reads the configuration from makefile, it will ignore the Include Paths completely thus, gtk won't be recognised by vscode.

To get Intellisense support for gtk in VSCode follow this discussion:
https://www.reddit.com/r/GTK/comments/wc0xw7/setup_gtk3_in_vs_code/

### To get a demo application:
Install the gtk3 examples and demo package.
In ubuntu:
```
sudo apt install gtk-3-examples
```
In Arch (not tested, hopefully will work):
```
sudo pacman -S gtk-3-demo
sudo pacman -S gtk-3-examples
```

Then run the demo application:
```
gtk3-demo
```

You can press the "Run" button on top left after selecting the demo that you need. You can also view the source code !!