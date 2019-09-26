package br.ufsc.montanharussa;

import static br.ufsc.montanharussa.Car.logger;
import java.util.concurrent.Semaphore;
import java.util.logging.Logger;

public class Passenger extends Thread {

    final static Logger logger = Logger.getLogger(Passenger.class.getName());
    
    Semaphore mutex;
    Semaphore mutex2;
    Integer boarders;
    Integer unboarders;
    Semaphore boardQueue;
    Semaphore unboardQueue;
    Semaphore allAboard;
    Semaphore allAshore;
    Integer howManySeats;

    Passenger(String name, Semaphore mutex, Semaphore mutex2, Integer boarders, Integer unboarders,
            Semaphore boardQueue, Semaphore unboardQueue, Semaphore allAboard, Semaphore allAshore,
            Integer howManySeats) {

        super(name);

        this.mutex = mutex;
        this.mutex2 = mutex2;
        this.boarders = boarders;
        this.unboarders = unboarders;
        this.boardQueue = boardQueue;
        this.unboardQueue = unboardQueue;
        this.allAboard = allAboard;
        this.allAshore = allAshore;
        this.howManySeats = howManySeats;
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(2000);
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }

            synchronized (boardQueue) {
                try {
                    boardQueue.acquire();
                } catch (InterruptedException ex) {
                    logger.info(ex.getMessage());
                }
            }

            try {
                board();
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }

            synchronized (unboardQueue) {
                try {
                    unboardQueue.acquire();
                } catch (InterruptedException ex) {
                    logger.info(ex.getMessage());
                }
            }

            try {
                unboard();
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }

        }
    }

    private void board() throws InterruptedException {
        System.out.println("Passenger [" + this.getName() + "] boarding...");

        synchronized (mutex) {
            mutex.acquire();
            boarders += 1;
        }

        if (boarders.equals(howManySeats)) {
            allAboard.release();
            boarders = 0;
        }

        mutex.release();
    }

    private void unboard() throws InterruptedException {
        System.out.println("Passenger [" + this.getName() + "] unboarding...");

        synchronized (mutex2) {
            mutex2.acquire();
            unboarders += 1;
        }

        if (unboarders.equals(howManySeats)) {
            allAshore.release();
            unboarders = 0;
        }

        mutex2.release();
    }
}
