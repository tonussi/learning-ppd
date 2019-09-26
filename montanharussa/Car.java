package br.ufsc.montanharussa;

import java.util.concurrent.Semaphore;
import java.util.logging.Logger;

public class Car extends Thread {

    final static Logger logger = Logger.getLogger(Car.class.getName());

    final Integer C;
    Integer howManySeats;
    Integer boarders;
    Integer unboarders;
    Semaphore boardQueue;
    Semaphore unboardQueue;
    Semaphore allAboard;
    Semaphore allAshore;

    Car(String name, Integer howManySeats, Semaphore boardQueue,
            Semaphore unboardQueue, Semaphore allAboard,
            Semaphore allAshore, Integer boarders, Integer unboarders) {

        super(name);

        this.C = howManySeats;
        this.boarders = boarders;
        this.unboarders = unboarders;
        this.boardQueue = boardQueue;
        this.unboardQueue = unboardQueue;
        this.allAshore = allAshore;
        this.allAboard = allAboard;
        this.howManySeats = howManySeats;
    }

    @Override
    public void run() {
        while (true) {
            try {
                load();
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }

            try {
                Thread.sleep(2000);
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }

            try {
                unload();
            } catch (InterruptedException ex) {
                logger.info(ex.getMessage());
            }
        }
    }

    private void load() throws InterruptedException {
        System.out.println("Car loading...");
        boardQueue.release(howManySeats);
        synchronized (allAboard) {
            allAboard.acquire();
        }
    }

    private void unload() throws InterruptedException {
        System.out.println("Car unloading...");
        unboardQueue.release(howManySeats);
        allAshore.acquire();
    }

}
