package com.example.appengine.lab2.models;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown = true)
public class MessageContent {
    private long id;
    private String text;

    public MessageContent() {
    }

    public MessageContent(long id, String text) {
        this.id = id;
        this.text = text;
    }

    public long getId() {
        return id;
    }

    public String getText() {
        return text;
    }

    @Override
    public String toString() {
        return "MessageContent[" +
                "id=" + id + ", " +
                "text=" + text + ']';
    }

}
