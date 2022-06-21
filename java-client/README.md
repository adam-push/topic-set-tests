# Setting topics with Java

This test will send 200 byte updates across 100 topics. The topic type is "binary" and by default delta calculations are disabled.

There are three methods of updating topics in this test:

1. `TopicUpdate.set()` for basic performance figures. Topics are pre-created prior to running this test.
2. `TopicUpdate.addAndSet()` which creates topics as required, but has additional overhead sending and parsing a `TopicSpecification` each time.
3. `UpdateStream.set()` creates the topic on the initial call and subsequently streams deltas (deltas are not sent to subscribers, just into Diffusion).

## Building

`mvn clean package`

## Running

The program takes three arguments:
1. The URL of the Diffusion server
2. The number of topic updates to send
3. The test to run (as above: 1, 2 or 3)

e.g.
```
java -jar target/topic-set-tests-0.1.0-0.DEVELOPER-SNAPSHOT-jar-with-dependencies.jar \
     ws://localhost:8090 \
     10000000 \
     1
```

