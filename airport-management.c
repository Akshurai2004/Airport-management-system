#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_FLIGHTS 6
#define SEATS_PER_FLIGHT 60
#define ROWS 10
#define COLS 6

typedef struct Seat {
    int seatNumber;
    int isBooked;
    struct Seat* next;
} Seat;

typedef struct Flight {
    int flightNumber;
    Seat* seats;
} Flight;

typedef struct User {
    char name[50];
    int flightNumber;
    int seatNumber;
} User;


void initializeFlights(Flight* flights);
Seat* createSeatList();
void displayFlights(Flight* flights);
void displayFlightSeats(Flight* flights);
void bookTicket(Flight* flights);
void cancelTicket(Flight* flights);
void saveBooking(User user);
void removeBooking(int flightNumber, int seatNumber);
void displayBookings();
void exitSystem();
Seat* findAvailableSeat(Seat* head);
Seat* findSeat(Seat* head, int seatNumber);
void freeSeatList(Seat* head);

int main() {
    Flight flights[TOTAL_FLIGHTS];
    initializeFlights(flights);

    int choice;
    while (1) {
        printf("\n--- Airport Management System ---\n");
        printf("1. Display available flights\n");
        printf("2. Display seats for a flight\n");
        printf("3. Book a ticket\n");
        printf("4. Cancel a ticket\n");
        printf("5. Display all bookings\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayFlights(flights);
                break;
            case 2:
                displayFlightSeats(flights);
                break;
            case 3:
                bookTicket(flights);
                break;
            case 4:
                cancelTicket(flights);
                break;
            case 5:
                displayBookings();
                break;
            case 6:
                exitSystem();
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

void initializeFlights(Flight* flights) {
    for (int i = 0; i < TOTAL_FLIGHTS; i++) {
        flights[i].flightNumber = i + 1;
        flights[i].seats = createSeatList();
    }
}

Seat* createSeatList() {
    Seat* head = NULL;
    Seat* temp = NULL;

    for (int i = 1; i <= SEATS_PER_FLIGHT; i++) {
        Seat* newSeat = (Seat*)malloc(sizeof(Seat));
        newSeat->seatNumber = i;
        newSeat->isBooked = 0;
        newSeat->next = NULL;

        if (head == NULL) {
            head = newSeat;
            temp = head;
        } else {
            temp->next = newSeat;
            temp = temp->next;
        }
    }
    return head;
}

void displayFlights(Flight* flights) {
    printf("\nAvailable Flights and Seats:\n");
    for (int i = 0; i < TOTAL_FLIGHTS; i++) {
        printf("Flight %d: ", flights[i].flightNumber);
        int availableSeats = 0;
        Seat* temp = flights[i].seats;
        while (temp != NULL) {
            if (!temp->isBooked) availableSeats++;
            temp = temp->next;
        }
        printf("%d seats available\n", availableSeats);
    }
}

void displayFlightSeats(Flight* flights) {
    int flightNumber;
    printf("\nEnter flight number to view seat matrix (1-%d): ", TOTAL_FLIGHTS);
    scanf("%d", &flightNumber);

    if (flightNumber < 1 || flightNumber > TOTAL_FLIGHTS) {
        printf("Invalid flight number!\n");
        return;
    }

    printf("\nSeat Matrix for Flight %d:\n", flightNumber);
    Seat* temp = flights[flightNumber - 1].seats;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            int seatNum = i * COLS + j;
            Seat* seat = findSeat(temp, seatNum);
            if (seat->isBooked)
                printf("[X] ");
            else
                printf("[O] ");
        }
        printf("\n");
    }
    printf("Legend: [O] = Available, [X] = Booked\n");
}

void bookTicket(Flight* flights) {
    int flightNumber;
    printf("\nEnter flight number to book (1-%d): ", TOTAL_FLIGHTS);
    scanf("%d", &flightNumber);

    if (flightNumber < 1 || flightNumber > TOTAL_FLIGHTS) {
        printf("Invalid flight number!\n");
        return;
    }

    displayFlightSeats(&flights[flightNumber - 1]);

    int seatNumber;
    printf("Enter seat number to book (1-%d): ", SEATS_PER_FLIGHT);
    scanf("%d", &seatNumber);

    if (seatNumber < 1 || seatNumber > SEATS_PER_FLIGHT) {
        printf("Invalid seat number!\n");
        return;
    }

    Seat* seat = findSeat(flights[flightNumber - 1].seats, seatNumber);
    if (seat->isBooked) {
        printf("Seat already booked!\n");
        return;
    }

    User user;
    printf("Enter your name: ");
    scanf("%s", user.name);
    user.flightNumber = flightNumber;
    user.seatNumber = seatNumber;

    seat->isBooked = 1;
    saveBooking(user);

    printf("Booking confirmed for %s on Flight %d, Seat %d.\n", user.name, flightNumber, seatNumber);
}

void cancelTicket(Flight* flights) {
    int flightNumber, seatNumber;
    printf("\nEnter flight number to cancel (1-%d): ", TOTAL_FLIGHTS);
    scanf("%d", &flightNumber);
    printf("Enter seat number to cancel (1-%d): ", SEATS_PER_FLIGHT);
    scanf("%d", &seatNumber);

    if (flightNumber < 1 || flightNumber > TOTAL_FLIGHTS || seatNumber < 1 || seatNumber > SEATS_PER_FLIGHT) {
        printf("Invalid flight or seat number!\n");
        return;
    }

    Seat* seat = findSeat(flights[flightNumber - 1].seats, seatNumber);
    if (!seat->isBooked) {
        printf("Seat not booked!\n");
        return;
    }

    seat->isBooked = 0;
    removeBooking(flightNumber, seatNumber);
    printf("Ticket canceled for Flight %d, Seat %d.\n", flightNumber, seatNumber);
}

void saveBooking(User user) {
    FILE* file = fopen("bookings.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%s %d %d\n", user.name, user.flightNumber, user.seatNumber);
    fclose(file);
}

void removeBooking(int flightNumber, int seatNumber) {
    FILE* file = fopen("bookings.txt", "r");
    FILE* temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL) {
        printf("Error processing cancellation!\n");
        return;
    }

    User user;
    while (fscanf(file, "%s %d %d", user.name, &user.flightNumber, &user.seatNumber) != EOF) {
        if (!(user.flightNumber == flightNumber && user.seatNumber == seatNumber)) {
            fprintf(temp, "%s %d %d\n", user.name, user.flightNumber, user.seatNumber);
        }
    }

    fclose(file);
    fclose(temp);
    remove("bookings.txt");
    rename("temp.txt", "bookings.txt");
}

void displayBookings() {
    FILE* file = fopen("bookings.txt", "r");
    if (file == NULL) {
        printf("No bookings found!\n");
        return;
    }
    printf("\nCurrent Bookings:\n");
    User user;
    while (fscanf(file, "%s %d %d", user.name, &user.flightNumber, &user.seatNumber) != EOF) {
        printf("Name: %s, Flight: %d, Seat: %d\n", user.name, user.flightNumber, user.seatNumber);
    }
    fclose(file);
}

void exitSystem() {
    printf("\nExiting the booking system. Thank you!\n");
}

Seat* findAvailableSeat(Seat* head) {
    Seat* temp = head;
    while (temp != NULL) {
        if (!temp->isBooked) return temp;
        temp = temp->next;
    }
    return NULL;
}

Seat* findSeat(Seat* head, int seatNumber) {
    Seat* temp = head;
    while (temp != NULL) {
        if (temp->seatNumber == seatNumber) return temp;
        temp = temp->next;
    }
    return NULL;
}

void freeSeatList(Seat* head) {
    Seat* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
