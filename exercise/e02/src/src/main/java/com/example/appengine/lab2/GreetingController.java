package com.example.appengine.lab2;

import com.google.cloud.Timestamp;
import com.google.cloud.datastore.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.List;

@RestController
public class GreetingController {

    private final Datastore datastore = DatastoreOptions.getDefaultInstance().getService();
    private final KeyFactory keyFactory = datastore.newKeyFactory().setKind("greeting");
    Logger logger = LoggerFactory.getLogger(GreetingController.class);

    @GetMapping("/greeting")
    public Greeting getGreeting(@RequestParam(value = "name", defaultValue = "World") String name) {

        Key key = datastore.allocateId(keyFactory.newKey());

        Greeting greeting = new Greeting(key.getId(), name, Timestamp.now());

        Entity greetingEntity = Entity.newBuilder(key)
                .set("name", name)
                .set("time", Timestamp.now())
                .build();
        datastore.put(greetingEntity);

        return greeting;
    }

    @GetMapping("/list")
    public List<Greeting> listGreetings() {
        Query<Entity> q = Query.newGqlQueryBuilder(Query.ResultType.ENTITY, "SELECT * FROM greeting ORDER BY time DESC LIMIT @limit")
                .setBinding("limit", 10)
                .build();
        QueryResults<Entity> results = datastore.run(q);
        ArrayList<Greeting> greetings = new ArrayList<>(10);
        while (results.hasNext()) {
            Entity e = results.next();
            greetings.add(new Greeting(
                    e.getKey().getId(),
                    e.getString("name"),
                    e.getTimestamp("time")
            ));
        }
        return greetings;
    }

    @GetMapping("/cleanup")
    public ResponseEntity<String> cleanup() {
        Query<Entity> q = Query.newGqlQueryBuilder(Query.ResultType.ENTITY, "SELECT * FROM greeting order by time asc").build();

        QueryResults<Entity> results = datastore.run(q);

        List<Long> ids = new ArrayList<>();

        while (results.hasNext()) {
            Entity e = results.next();
            var id = e.getKey();
            var timestamp = e.getTimestamp("time");

            if (timestamp.getSeconds() > Timestamp.now().getSeconds() - 300) {
                break;
            }
            datastore.delete(id);
            ids.add(id.getId());
        }

        if (ids.size() > 0) {
            logger.info("Removed greetings with following ids: {}", String.join(", ", ids.stream().map(Object::toString).toList()));
        }
        return ResponseEntity.ok().build();
    }
}
