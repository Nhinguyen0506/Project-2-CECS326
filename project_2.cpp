#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

#define PHILOSOPHERS 5
#define THINKING 0
#define NEED_TO_EAT 1
#define EATING 2

int status[PHILOSOPHERS]; 
pthread_mutex_t mutex; // Global mutex
pthread_cond_t cond_var[PHILOSOPHERS]; 

// Test if a philosopher can eat
void test(int philosopher_number) {
    if (status[(philosopher_number + 4) % PHILOSOPHERS] == EATING) // check left chopstick
    {
        cout << " Forks  #" << philosopher_number << " is with Philosopher " 
        << (philosopher_number + 4) % PHILOSOPHERS << endl;
    }
    else if (status[(philosopher_number + 1) % PHILOSOPHERS] == EATING) // check right chopstick
    {
        cout << " Forks  #" << philosopher_number + 1 << " is with Philosopher " 
        << (philosopher_number + 1) % PHILOSOPHERS << endl;        
    }
    else if (status[philosopher_number] == NEED_TO_EAT)
        {
        status[philosopher_number] = EATING;
        cout << "Forks are with Philosopher " << philosopher_number << endl;
        pthread_cond_signal(&cond_var[philosopher_number]);
    }
}

void take_forks(int philosopher_number) {
    pthread_mutex_lock(&mutex);
    status[philosopher_number] = NEED_TO_EAT;
    cout << "Philosopher " << philosopher_number << " is hungry." << endl;
    test(philosopher_number); // Test if the philosopher can eat
    // If philosopher can't eat, wait
    if (status[philosopher_number] != EATING) {
        pthread_cond_wait(&cond_var[philosopher_number], &mutex);
    }
    pthread_mutex_unlock(&mutex); 
}

void return_forks(int philosopher_number) {
    pthread_mutex_lock(&mutex); 
    status[philosopher_number] = THINKING;
    cout << "Philosopher " << philosopher_number << " full and return chopsticks." << endl;
    // Check chopstick left and right to use
    test((philosopher_number + 4) % PHILOSOPHERS);
    test((philosopher_number + 1) % PHILOSOPHERS);
    pthread_mutex_unlock(&mutex);
}

void* start_dining(void* num) {
    int philosopher_number = *(int*)num;

    while (true) {
        // Thinking time
        int thinking_time = (rand() % 5) * 1000; // Random time between 0ms and 5000ms
        cout << "Philosopher " << philosopher_number << " took " << thinking_time << "ms thinking" << endl;
        usleep(thinking_time * 1000);
        // Try to pick up chopsticks (eat)
        take_forks(philosopher_number);
        // Eating time
        int eating_time = (rand() % 4) * 1000; // Random eating time between 0ms and 4000ms
        cout << "Philosopher " << philosopher_number << " took " << eating_time << "ms eating" << endl;
        usleep(eating_time * 1000);
        // Put down chopsticks
        return_forks(philosopher_number);
    }
    return nullptr;
}
int main() {
    pthread_t philosophers[PHILOSOPHERS]; // Array to hold philosopher threads
    int philosopher_numbers[PHILOSOPHERS]; // Array to hold philosopher numbers
    pthread_mutex_init(&mutex, NULL); 
    for (int i = 0; i < PHILOSOPHERS; i++) {
        pthread_cond_init(&cond_var[i], NULL); 
        status[i] = THINKING; // Initially, all philosophers are THINKING
        philosopher_numbers[i] = i; // Assign philosopher numbers 0 to 4
    }
    // Create threads
    for (int i = 0; i < PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, start_dining, &philosopher_numbers[i]);
    }

    // Join threads
    for (int i = 0; i < PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
