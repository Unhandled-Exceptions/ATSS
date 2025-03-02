# ATC
## Air Traffic Control

Download, compile run!

## Project Directory Structure:

~~~
ATSS/
│── src/                 # Source Files
│   │── main.c
│── lib/                 # 3rd Party Libraries
│   │── sqlite3.c
│   │── sqlite3.h
│── include/             # Project specific header files
│── bin/                 # Compiled binaries
│── obj/                 # Compiled object files
│── Makefile
~~~

> Note: bin/ obj/ are created on your pc temporarily while compiling.

## TODO
- [ ] Better makefile
- [ ] Make db names as arguments instead of inputing inside the program
- [ ] Setup proper formatting.

## Useful commands

To see the status of the flights table
```
sqlite3 -table flights.db "select * from flights;"
```

## Algorithm for the alloter

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
