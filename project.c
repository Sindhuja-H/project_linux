#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// The maximum number of customer threads.
#define MAX_PATIENTS 50

// Function prototypes…
void *patient( void *num );
void *dentist(void *);

void randwait(int secs);

// Define the semaphores.

// waitingRoom Limits the # of customers allowed
// to enter the waiting room at one time.
sem_t waitingRoom;

// barberChair ensures mutually exclusive access to
// the barber chair.
sem_t bestodentChair;

// barberPillow is used to allow the barber to sleep
// until a customer arrives.
sem_t dentistFree;

// seatBelt is used to make the customer to wait until
// the barber is done cutting his/her hair.
sem_t seatBelt;

// Flag to stop the barber thread when all customers
// have been serviced.
int allDone = 0;

int main(int argc, char *argv[]) {
pthread_t btid;
pthread_t tid[MAX_PATIENTS];
long RandSeed;
int i, numPatients, numChairs;
int Number[MAX_PATIENTS];

printf("Enter the number of Patients : ");
 scanf("%d",&numPatients) ;
printf("Enter the number of Bestodent Chairs : "); 
scanf("%d",&numChairs);

// Make sure the number of threads is less than the number of
// customers we can support.
if (numPatients > MAX_PATIENTS) {
printf("The maximum number of Patients is %d.\n", MAX_PATIENTS);
exit(-1);
}

// Initialize the numbers array.
for (i=0; i<MAX_PATIENTS; i++) {
Number[i] = i;
}

// Initialize the semaphores with initial values…
sem_init(&waitingRoom, 0, numChairs);
sem_init(&bestodentChair, 0, 1);
sem_init(&dentistFree, 0, 0);
sem_init(&seatBelt, 0, 0);

// Create the barber.
pthread_create(&btid, NULL, dentist, NULL);

// Create the customers.
for (i=0; i<numPatients; i++) {
pthread_create(&tid[i], NULL, patient, (void *)&Number[i]);
sleep(1);
}

// Join each of the threads to wait for them to finish.
for (i=0; i<numPatients; i++) {
pthread_join(tid[i],NULL);
sleep(1);
}

// When all of the customers are finished, kill the
// barber thread.
allDone = 1;
sem_post(&dentistFree); // Wake the barber so he will exit.
pthread_join(btid,NULL);
}

void *patient(void *number) {
int num = *(int *)number;

// Leave for the shop and take some random amount of
// time to arrive.
//printf("Patient %d leaving for barber shop.\n", num);
//randwait(2);
printf("Patient %d arrived at clinic\n", num);

// Wait for space to open up in the waiting room…
sem_wait(&waitingRoom);
printf("Patient %d entering waiting room.\n", num);

// Wait for the barber chair to become free.
sem_wait(&bestodentChair);

// The chair is free so give up your spot in the
// waiting room.
sem_post(&waitingRoom);

// Wake up the barber…
printf("Patient %d wants service from dentist\n", num);
sem_post(&dentistFree);

// Wait for the barber to finish cutting your hair.
sem_wait(&seatBelt);

// Give up the chair.
sem_post(&bestodentChair);
printf("Patient %d leaving clinic\n", num);
}

void *dentist(void *junk) {
// While there are still customers to be serviced…
// Our barber is omnicient and can tell if there are
// customers still on the way to his shop.
while (!allDone) {

// Sleep until someone arrives and wakes you..
printf("The dentis is free\n");
sem_wait(&dentistFree);

// Skip this stuff at the end…
if (!allDone) {

// Take a random amount of time to cut the
// customer’s hair.
printf("The patient is getting checkup from dentist\n");
randwait(2);
printf("The checkup is done\n");

// Release the customer when done cutting…
sem_post(&seatBelt);
}
else {
printf("The dentist is going home for the day.\n");
}
}
}

void randwait(int secs) {
int len;

// Generate a random number…
len = (int) ((1 * secs) + 1);
sleep(len);
}