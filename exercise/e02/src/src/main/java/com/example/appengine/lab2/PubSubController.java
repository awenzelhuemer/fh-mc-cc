package com.example.appengine.lab2;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.MapperFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.json.JsonMapper;
import com.google.cloud.datastore.*;
import com.google.gson.JsonElement;
import com.google.gson.JsonParser;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.io.IOException;
import java.util.*;

@RestController
public class PubSubController {

    private final Datastore datastore = DatastoreOptions.getDefaultInstance().getService();
    private final KeyFactory keyFactory = datastore.newKeyFactory().setKind("message");

    Logger logger = LoggerFactory.getLogger(PubSubController.class);

    @PostMapping("/pubsub/endpoint")
    public ResponseEntity<String> handlePubSubMessage(@RequestBody String pubSubMessage) {
         logger.info("Received Pub/Sub message with payload: " + pubSubMessage);

        try {
            var messageContent = getMessageContent(pubSubMessage);

            logger.info(messageContent.toString());

            Key key = datastore.allocateId(keyFactory.newKey());
            Entity entity = Entity.newBuilder(key)
                    .set("text", messageContent.getText())
                    .build();
            datastore.put(entity);

            // Return a success response
            return ResponseEntity.ok("Message processed successfully");
        } catch (IOException e) {
            logger.error("Error parsing JSON: " + pubSubMessage, e);
            // Handle parsing errors
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error processing Pub/Sub message");
        }
    }

    private MessageContent getMessageContent(String messageContent) throws IOException {

        JsonElement jsonRoot = JsonParser.parseString(messageContent).getAsJsonObject();
        String messageStr = jsonRoot.getAsJsonObject().get("message").toString();

        logger.info("Extracted string: " + messageStr);

        var objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        objectMapper.configure(MapperFeature.ACCEPT_CASE_INSENSITIVE_PROPERTIES, true);

        Message message = objectMapper.readValue(messageStr, Message.class);

        logger.info("Message: " + message);

        String decodedMessage = new String(Base64.getDecoder().decode(message.getData()));

        logger.info("Decoded Message: " + decodedMessage);


        return objectMapper.readValue(decodedMessage, MessageContent.class);
    }

    @GetMapping("/pubsub/list")
    public ResponseEntity<List<MessageContent>> listStoredMessages() {
        Query<Entity> q = Query.newGqlQueryBuilder(Query.ResultType.ENTITY, "SELECT * FROM message").build();

        QueryResults<Entity> results = datastore.run(q);

        ArrayList<MessageContent> messages = new ArrayList<>();

        while (results.hasNext()) {
            Entity e = results.next();
            var id = e.getKey();
            var message = e.getString("text");
            messages.add(new MessageContent(id.getId(), message));
        }

        return ResponseEntity.ok(messages);
    }
}