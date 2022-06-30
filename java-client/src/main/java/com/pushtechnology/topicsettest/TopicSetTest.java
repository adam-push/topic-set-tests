package com.pushtechnology.topicsettest;

import static java.lang.String.format;

import com.pushtechnology.diffusion.client.Diffusion;
import com.pushtechnology.diffusion.client.features.TopicUpdate;
import com.pushtechnology.diffusion.client.features.UpdateStream;
import com.pushtechnology.diffusion.client.features.control.topics.TopicControl;
import com.pushtechnology.diffusion.client.features.control.topics.TopicControl.AddTopicResult;
import com.pushtechnology.diffusion.client.session.Session;
import com.pushtechnology.diffusion.client.topics.details.TopicSpecification;
import com.pushtechnology.diffusion.client.topics.details.TopicType;
import com.pushtechnology.diffusion.datatype.binary.Binary;
import com.pushtechnology.diffusion.datatype.binary.impl.BinaryDataTypeImpl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.atomic.AtomicInteger;

public class TopicSetTest {
    private static final BinaryDataTypeImpl binary = new BinaryDataTypeImpl();

    public static void main(String[] args)
            throws Exception {

        if (args.length < 3) {
            System.out.println("Supply Diffusion server URL, the number of iterations and the test to be run [1-3], e.g. ws://localhost:8080 100000 1");
            System.exit(1);
        }

        final String url = args[0];
        final int iterations = Integer.parseInt(args[1]);
        final int testID = Integer.parseInt(args[2]);

        Session session = Diffusion.sessions()
                .principal("admin")
                .password("password")
                .open(url);

        if (session != null && session.getState() == Session.State.CONNECTED_ACTIVE) {
            onConnected(session, iterations, testID);
            System.out.println("Done.");
            session.close();
        }
        else {
            System.out.println("Failed to establish session");
        }
    }

    protected static void onConnected(Session session,
                                      int iterations,
                                      int testID)
            throws Exception {

        TopicUpdate topicUpdate = session.feature(TopicUpdate.class);
        TopicControl topicControl = session.feature(TopicControl.class);

        final int numTopics = 100;
        TopicSpecification spec = Diffusion.newTopicSpecification(TopicType.BINARY)
                .withProperty(TopicSpecification.PUBLISH_VALUES_ONLY, "true");
        String topicPath = "test/set/java"; // + "/" + System.currentTimeMillis();

        System.out.println(format("Topic name: %s", topicPath));
        System.out.println(format("Test will run for %s iterations", iterations));

        long startTime = System.currentTimeMillis();

        switch(testID) {
            case 1:
                System.out.println("Test 1 - Add topics, then time how long it takes to set() them");
                addAllTopics(topicControl, spec, topicPath, numTopics).get();
                // Reset the clock once the topics have been created
                startTime = System.currentTimeMillis();
                setAllTopics(topicUpdate, topicPath, numTopics, iterations).get();
                break;
            case 2:
                System.out.println("Test 2 - Do addAndSet()");
                addAndSetAllTopics(topicUpdate, topicControl, spec, topicPath, numTopics, iterations).get();
                break;
            case 3:
                System.out.println("Test 3 - Use an UpdateStream to add the topic if it doesn't exist and subsequently stream them.");
                streamAllTopics(topicUpdate, spec, topicPath, numTopics, iterations).get();
                break;
            case 4:
                System.out.println("Test 4 - Reuse existing topics, timing how long it takes to set() them");
                setAllTopics(topicUpdate, topicPath, numTopics, iterations).get();
                break;
            default:
                System.out.println("Unknown test " + testID);
                return;
        }

        final long totalTime = System.currentTimeMillis();
        System.out.println("Published " + iterations + " messages to 100 topics in " + (totalTime - startTime) + "ms");
        System.out.println("Average rate was " + ((double)iterations / (totalTime - startTime) * 1000) + " msgs/s");
    }

    private static CompletableFuture<Boolean> addAllTopics(TopicControl topicControl, TopicSpecification spec, String topicPath, int numTopics) {
        CompletableFuture future = new CompletableFuture<Void>();

        var results = new ArrayList<CompletableFuture<AddTopicResult>>();

        for (int i = 0; i < numTopics; i++) {
            String topic = topicPath + "/" + i;
            results.add(topicControl.addTopic(topic, spec));
        }
        System.out.println("All topics added");

        CompletableFuture.allOf(results.toArray(new CompletableFuture[results.size()]))
                .whenComplete((ok, err) -> {
                    future.complete(true);
                });

        return future;
    }


    private static CompletableFuture<Boolean> setAllTopics(
            TopicUpdate topicUpdate,
            String topicPath,
            int numTopics,
            int iterations) {
        CompletableFuture future = new CompletableFuture<Boolean>();

        AtomicInteger completedPublicationTasks = new AtomicInteger();

        for (int i = 0; i < iterations; i++) {
            String topic = topicPath + "/" + (i % numTopics);
            String data = "" + System.nanoTime() + "XXXXXX" + "ABCDEFHIJ".repeat(20);

            Binary dataValue = binary.readValue(data.getBytes());
            topicUpdate.set(topic, Binary.class, dataValue)
                    .whenComplete((ok, ex) -> {
                        if (ex != null) {
                            System.out.println("Exception publishing to " + topic + " :" + ex.getMessage());
                        } else {
                            if (completedPublicationTasks.getAndIncrement() == iterations - 1) {
                                System.out.println("All publication tasks complete");
                                future.complete(true);
                            }
                        }
                    });
        }

        return future;
    }

    private static CompletableFuture<Boolean> addAndSetAllTopics(
            TopicUpdate topicUpdate,
            TopicControl topicControl,
            TopicSpecification spec,
            String topicPath,
            int numTopics,
            int iterations)
            throws Exception {

        CompletableFuture future = new CompletableFuture<Boolean>();

        AtomicInteger completedPublicationTasks = new AtomicInteger();

        for (int i = 0; i < iterations; i++) {
            String topic = topicPath + "/" + (i % numTopics);

            String data = "" + System.nanoTime() + "XXXXXX" + "ABCDEFGHI".repeat(20);
            Binary dataValue = binary.readValue(data.getBytes());

            topicUpdate.addAndSet(
                            topic,
                            spec,
                            Binary.class,
                            dataValue)
                    .whenComplete((result, ex) -> {
                        if (ex != null) {
                            System.out.println("Exception publishing to " + topic + " :" + ex.getMessage());
                        }
                        if (completedPublicationTasks.getAndIncrement() == iterations - 1) {
                            System.out.println("All publication tasks complete.");
                            future.complete(true);
                        }
                    });

        }

        return future;
    }

    private static CompletableFuture<Boolean> streamAllTopics(
            TopicUpdate topicUpdate,
            TopicSpecification spec,
            String topicPath,
            int numTopics,
            int iterations)
            throws Exception {

        CompletableFuture future = new CompletableFuture<Boolean>();

        AtomicInteger completedPublicationTasks = new AtomicInteger();

        Map<String, UpdateStream<Binary>> streams = new HashMap<>();

        // Create update streams for each topic
        for (int i = 0; i < iterations; i++) {
            String topic = topicPath + "/" + (i % numTopics);

            String data = "" + System.nanoTime() + "XXXXXX" + "ABCDEFGHI".repeat(20);
            Binary dataValue = binary.readValue(data.getBytes());

            UpdateStream stream = streams.get(topic);
            if (stream == null) {
                stream = topicUpdate.createUpdateStream(topic, spec, Binary.class);
                streams.put(topic, stream);
            }

            stream.set(dataValue)
                    .whenComplete((ok, ex) -> {
                        if (ex != null) {
                            System.out.println("Exception publishing to " + topic + " :" + ex);
                        }
                        if (completedPublicationTasks.getAndIncrement() == iterations - 1) {
                            System.out.println("All publication tasks complete.");
                            future.complete(true);
                        }
                    });
        }

        return future;
    }
}
