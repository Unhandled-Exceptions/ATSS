#include <stdio.h>
#include <stdlib.h>

#define KCYN  "\x1B[36m"

void view_flight_schedules();
void add_flight_schedules();
void update_flight_schedules();
void delete_flight_schedules();
void view_crew_info();

int main() {

    // Display the title and copyright
    printf("%s    ___  ________________\n   /   |/_  __/ ___/ ___/\n  / /| | / /  \\__ \\\\__ \\ \n / ___ |/ /  ___/ /__/ / \n/_/  |_/_/  /____/____/  \n\x1B[0m",KCYN);
    printf("\nCopyright (c) 2025  by R Uthaya Murthy, Varghese K James, Tarun S\n");
    
    printf("\n\nMenu:\n1.View Flight Schedules\n2.Add Flight Schedules\n3.Update Flight Schedules\n4.Delete Flight Schedules\n5.View Flight Crew Information\n6.Exit\n");
    
    int choice;
    while (1) {
        printf("\n> ");
        scanf("%d",&choice);
        
        switch(choice){
            case 1:
                view_crew_info();
                break;
            case 2:
                add_flight_schedules();
                break;
            case 3:
                update_flight_schedules();
                break;
            case 4:
                delete_flight_schedules();
                break;
            case 5:
                view_flight_schedules();
                break;
            case 6:
                printf("Bye !\n");
                exit(0);
            default:
                printf("Invalid choice !\n");
        }
    }
    return 0;  
}

void view_flight_schedules(){
    printf("View Flight Schedule : Not yet implemented !!!\n");
}

void add_flight_schedules(){
    printf("Add Flight Schedule : Not yet implemented !!!\n");
}

void update_flight_schedules(){
    printf("Update Flight Schedule : Not yet implemented !!!\n");
}

void delete_flight_schedules(){
    printf("Delete Flight Schedule : Not yet implemented !!!\n");
} 

void view_crew_info(){
    printf("View Flight Crew Information : Not yet implemented !!!\n");
}
