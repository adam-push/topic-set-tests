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
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.atomic.AtomicInteger;

public class TopicSetTest {
    private static final BinaryDataTypeImpl binary = new BinaryDataTypeImpl();

    public static void main(String[] args)
            throws Exception {

        if (args.length < 3) {
            System.out.println("<url> <iterations> <test_number> [<total_unique_topic_values> [<topic_value_size> [<message_queue_size> [<total_topics>]]]]");
            System.exit(1);
        }

        final String url = args[0];
        final int iterations = Integer.parseInt(args[1]);
        final int testID = Integer.parseInt(args[2]);

        int totalUniqueTopicValues = 1;
        if (args.length >= 4) {
            totalUniqueTopicValues = Integer.parseInt(args[3]);
        }

        int topicValueSize = 250;
        if (args.length >= 5) {
            topicValueSize = Integer.parseInt(args[4]);
        }

        int messageQueueSize = 10000;
        if (args.length >= 6) {
            messageQueueSize = Integer.parseInt(args[5]);
        }

        int totalTopics = 100;
        if (args.length >= 7) {
            totalTopics = Integer.parseInt(args[6]);
        }

        System.out.println("Parameters used in test:");
        System.out.println("- url: " + url);
        System.out.println("- iterations: " + iterations);
        System.out.println("- test number: " + testID);
        System.out.println("- total unique topic values: " + totalUniqueTopicValues);
        System.out.println("- topic value size: " + topicValueSize);
        System.out.println("- message queue size: " + messageQueueSize);
        System.out.println("- total topics: " + totalTopics);

        final String rootTopicPath = "test/set/java";

        List<Binary> topicValues = new ArrayList<Binary>();
        // generate random topic values
        Random randomGenerator = new Random();
        for(long i = 0; i < totalUniqueTopicValues; i++) {
            byte[] randomData = new byte[topicValueSize];
            randomGenerator.nextBytes(randomData);
            topicValues.add(binary.readValue(randomData));
        }

        Session session = Diffusion.sessions()
                .principal("admin")
                .password("password")
                .maximumQueueSize(messageQueueSize)
                .open(url);

        if (session != null && session.getState() == Session.State.CONNECTED_ACTIVE) {
            onConnected(
                session,
                rootTopicPath,
                iterations,
                totalTopics,
                totalUniqueTopicValues,
                topicValues,
                testID
            );
            System.out.println("Done.");
            session.close();
        }
        else {
            System.out.println("Failed to establish session");
        }
    }

    protected static void onConnected(Session session,
                                      String rootTopicPath,
                                      int iterations,
                                      int numTopics,
                                      int totalUniqueTopicValues,
                                      List<Binary> topicValues,
                                      int testID)
            throws Exception {

        TopicUpdate topicUpdate = session.feature(TopicUpdate.class);
        TopicControl topicControl = session.feature(TopicControl.class);

        TopicSpecification spec = Diffusion.newTopicSpecification(TopicType.BINARY)
                .withProperty(TopicSpecification.PUBLISH_VALUES_ONLY, "true");

        long startTime = System.currentTimeMillis();

        switch(testID) {
            case 1:
                System.out.println("Test 1 - Add topics, then time how long it takes to set() them");
                addAllTopics(topicControl, spec, rootTopicPath, numTopics).get();
                // Reset the clock once the topics have been created
                startTime = System.currentTimeMillis();
                setAllTopics(topicUpdate, rootTopicPath, numTopics, iterations, topicValues).get();
                break;
            case 2:
                System.out.println("Test 2 - Do addAndSet()");
                addAndSetAllTopics(
                    topicUpdate,
                    topicControl,
                    spec,
                    rootTopicPath,
                    numTopics,
                    iterations,
                    topicValues).get();
                break;
            case 3:
                System.out.println("Test 3 - Use an UpdateStream to add the topic if it doesn't exist and subsequently stream them.");
                streamAllTopics(topicUpdate, spec, rootTopicPath, numTopics, iterations, topicValues).get();
                break;
            case 4:
                System.out.println("Test 4 - Reuse existing topics, timing how long it takes to set() them");
                setAllTopics(topicUpdate, rootTopicPath, numTopics, iterations, topicValues).get();
                break;
            default:
                System.out.println("Unknown test " + testID);
                return;
        }

        final long elapsedTime = System.currentTimeMillis() - startTime;
        final double updatesPerSecond = ((double)iterations * 1000) / elapsedTime;
        final long roundedUpdateRate = new Double(updatesPerSecond).longValue();
        System.out.println("Test took " + elapsedTime + " ms.");
        System.out.println("Average Update Rate = " + roundedUpdateRate + " updates/s");
    }

    private static CompletableFuture<Boolean> addAllTopics(
            TopicControl topicControl,
            TopicSpecification spec,
            String topicPath, int
            numTopics)
            {
        CompletableFuture future = new CompletableFuture<Void>();

        List<CompletableFuture<AddTopicResult>> results = new ArrayList<CompletableFuture<AddTopicResult>>();

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
            String rootTopicPath,
            int numTopics,
            int iterations,
            List<Binary> topicValues) {

        CompletableFuture future = new CompletableFuture<Boolean>();
        AtomicInteger completedPublicationTasks = new AtomicInteger();

        for (int i = 0; i < iterations; i++) {
            String topicPath = rootTopicPath + "/" + (i % numTopics);
            Binary dataValue = topicValues.get(i % topicValues.size());

            topicUpdate.set(topicPath, Binary.class, dataValue)
                    .whenComplete((ok, ex) -> {
                        if (ex != null) {
                            System.out.println("Exception publishing to " + topicPath + " :" + ex.getMessage());
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
            String rootTopicPath,
            int numTopics,
            int iterations,
            List<Binary> topicValues) throws Exception {

        CompletableFuture future = new CompletableFuture<Boolean>();

        AtomicInteger completedPublicationTasks = new AtomicInteger();

        for (int i = 0; i < iterations; i++) {
            String topicPath = rootTopicPath + "/" + (i % numTopics);
            Binary dataValue = topicValues.get(i % topicValues.size());

            topicUpdate
                .addAndSet(topicPath, spec, Binary.class, dataValue)
                .whenComplete((result, ex) -> {
                    if (ex != null) {
                        System.out.println("Exception publishing to " + topicPath + " :" + ex.getMessage());
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
            String rootTopicPath,
            int numTopics,
            int iterations,
            List<Binary> topicValues) throws Exception {

        CompletableFuture future = new CompletableFuture<Boolean>();

        AtomicInteger completedPublicationTasks = new AtomicInteger();

        Map<String, UpdateStream<Binary>> streams = new HashMap<>();

        // Create update streams for each topic
        for (int i = 0; i < iterations; i++) {
            String topicPath = rootTopicPath + "/" + (i % numTopics);
            Binary dataValue = topicValues.get(i % topicValues.size());

            UpdateStream stream = streams.get(topicPath);
            if (stream == null) {
                stream = topicUpdate.createUpdateStream(topicPath, spec, Binary.class);
                streams.put(topicPath, stream);
            }

            stream.set(dataValue)
                    .whenComplete((ok, ex) -> {
                        if (ex != null) {
                            System.out.println("Exception publishing to " + topicPath + " :" + ex);
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
