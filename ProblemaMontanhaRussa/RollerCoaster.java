package br.ufsc.montanharussa;

import java.util.*;
import java.util.concurrent.*;

// Introduction to Semaphore https://youtu.be/e2ujg5K310s
public class RollerCoaster {

    final int NUMBER_OF_PASSENGERS = 30;

    Car car;

    // mutex protects passengers, which counts the
    // number of passengers that have invoked boardCar.
    final Semaphore mutex;
    final Semaphore mutex2;

    volatile Integer boarders;
    volatile Integer unboarders;

    // passengers wait on boardQueue before boarding
    final Semaphore boardQueue;

    // passengers wait on unboardQueue before unboarding.
    final Semaphore unboardQueue;

    // allAboard indicates that the car is full
    final Semaphore allAboard;

    final Semaphore allAshore;

    // Fila de passageiros
    public List<Passenger> passengers = new ArrayList<>();

    public Integer howManySeats;

    public RollerCoaster() {
        howManySeats = 10;

        boarders = 0;
        unboarders = 0;

        mutex = new Semaphore(1);
        mutex2 = new Semaphore(1);

        boardQueue = new Semaphore(howManySeats);
        unboardQueue = new Semaphore(howManySeats);

        allAboard = new Semaphore(howManySeats);
        allAshore = new Semaphore(howManySeats);
    }

    public void init() throws InterruptedException {
        for (int i = 0; i < NUMBER_OF_PASSENGERS; i++) {
            Passenger passenger = new Passenger("Person " + i, mutex,
                    mutex2, boarders, unboarders, boardQueue, unboardQueue,
                    allAboard, allAshore, howManySeats);
            passengers.add(passenger);
        }

        car = new Car("Carro", howManySeats, boardQueue, unboardQueue,
                allAboard, allAshore, boarders, unboarders);
        car.start();

        for (Passenger passenger : passengers)
            passenger.start();
    }
}
