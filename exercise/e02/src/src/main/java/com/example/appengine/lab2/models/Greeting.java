package com.example.appengine.lab2.models;

import com.google.cloud.Timestamp;

public class Greeting {
    private final long id;
    private final String name;

    private final Timestamp time;

    public Greeting(long id, String name, Timestamp time) {
        this.id = id;
        this.name = name;
        this.time = time;
    }

    public long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Timestamp getTime() {
        return time;
    }
}
