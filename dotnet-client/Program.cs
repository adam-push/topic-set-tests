﻿using PushTechnology.ClientInterface.Client.Factories;
using PushTechnology.ClientInterface.Client.Session;
using PushTechnology.ClientInterface.Client.Topics;
using PushTechnology.ClientInterface.Client.Topics.Details;
using PushTechnology.ClientInterface.Client.Callbacks;
using PushTechnology.ClientInterface.Client.Features.Control.Clients;
using PushTechnology.ClientInterface.Data.Binary;

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace dotnet_test
{
    class Program
    {
        const string filler = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDE";

        static string url;
        static int iterations;
        static int numTopics = 100;
        static int test_number;

        static async Task Main(string[] args)
        {
            if(args.Length < 3)
            {
                Console.WriteLine("Arguments: <url> <iterations> <test_number>");
                Environment.Exit(1);
            }

            url = args[0];
            iterations = Int32.Parse(args[1]);
            test_number = Int32.Parse(args[2]);

            await Run();
        }

        static async Task Run()
        {
            ISession session = null;
            ISession session2 = null;

            try
            {
                session = Diffusion.Sessions
                    .SessionStateChangedHandler((sender, args) => {
                        Console.WriteLine("Session state changed by " + sender + " to " + args.NewState);
                        })
                    .MaximumQueueSize(10_000)
                    .Principal("admin")
                    .Password("password")
                    .Open(url);
                Console.WriteLine(session.SessionId);

                List<string> requiredProperties = new List<string> { SessionProperty.ALL_FIXED_PROPERTIES, SessionProperty.ALL_USER_PROPERTIES };
                session.ClientControl.SetSessionPropertiesListener(new SessionPropertiesListener() ,requiredProperties.ToArray());

                session2 = Diffusion.Sessions
                    .Principal("admin")
                    .Password("password")
                    .Open(url);

                var spec = Diffusion.NewSpecification(TopicType.BINARY)
                    .WithProperty(TopicSpecificationProperty.PublishValuesOnly, "true");

                const String topicPath = "test/set/dotnet";

                long startTime = GetTimeMillis();

                switch(test_number)
                {
                    case 1:
                        Console.WriteLine("Running test: Add all topics then set them");
                        await AddAllTopics(session, spec, topicPath);

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopics(session, topicPath, numTopics, iterations);

                        break;
                    case 2:
                        Console.WriteLine("Running test: addAndSet()");
                        await AddAndSetAllTopics(session, spec, topicPath);
                        break;
                    case 3:
                        Console.WriteLine("Not implemented: Use UpdateStreams to update the topics");
                        break;
                    case 4:
                        Console.WriteLine("Running test: Update existing topics");
                        await SetAllTopics(session, topicPath, numTopics, iterations);
                        break;
                    case 5:
                        Console.WriteLine("Running test: Add a single topic, then set all the others");
                        await AddOnlyOneTopic(session, spec, "dummy");

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopics(session, topicPath, numTopics, iterations);
                        break;
                    case 6:
                        Console.WriteLine("Running test: Add a single topic in one session, then set the other topics in a different session");
                        await AddOnlyOneTopic(session2, spec, "dummy");

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopics(session, topicPath, numTopics, iterations);
                        break;
                    case 7:
                        Console.WriteLine("Running test: Create two sessions, and set all topics in only one of them");
                        await SetAllTopics(session, topicPath, numTopics, iterations);
                        break;
                    case 8:
                        Console.WriteLine("Running test: Add all topics, then set them with unordered client-side throttling");
                        await AddAllTopics(session, spec, topicPath);

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopicsThrottled(session, topicPath, numTopics, iterations, 10);
                        break;
                    case 9:
                        Console.WriteLine("Running test: Add all topics, then set them with ordered client-side throttling");
                        await AddAllTopics(session, spec, topicPath);

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopicsThrottledOrdered(session, topicPath, numTopics, iterations, 10);
                        break;
                    case 10:
                        Console.WriteLine("Running test: Add all topics, then set them with a steady update rate.");
                        await AddAllTopics(session, spec, topicPath);
                        await SetAllTopicsSteadyRate(session, topicPath, numTopics, iterations);
                        break;
                    default:
                        Console.WriteLine("Invalid test number: " + test_number);
                        break;
                }

                long endTime = GetTimeMillis();
                Console.WriteLine("Test took " + (endTime - startTime) + " ms");
                Console.WriteLine("= " + ((double)iterations / (endTime - startTime) * 1000) + " msgs/sec");

                session?.Close();
                session2?.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
            }
            finally
            {
                session?.Close();
            }
        }

        private static long GetTimeMillis()
        {
            DateTime zero = new DateTime(1970, 1, 1);
            TimeSpan span = DateTime.Now.Subtract(zero);
            return (long)span.TotalMilliseconds;
        }

        private static Task AddOnlyOneTopic(ISession session, ITopicSpecification spec, String topicPath)
        {
            return session.TopicControl.AddTopicAsync(topicPath, spec);
        }

        private static Task RemoveTopics(ISession session, String selector)
        {
            return session.TopicControl.RemoveTopicsAsync(selector);
        }

        private static Task AddAllTopics(ISession session, ITopicSpecification spec, String topicPath)
        {
            TaskCompletionSource<bool> tcs = new TaskCompletionSource<bool>();
            int completedAddTasks = 0;

            for(var i = 0; i < numTopics; i++) {
                String topic = topicPath + "/" + i;
                session.TopicControl.AddTopicAsync(topic, spec).ContinueWith(
                    addTask => {
                        if(addTask.Exception != null)
                        {
                            Console.WriteLine($"Exception adding topic {topic}: {addTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completedAddTasks) == numTopics)
                        {
                            Console.WriteLine($"All topics created");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

           return tcs.Task;
        }

        private static Task SetAllTopics(ISession session, String topicPath, int numTopics, int iterations)
        {
            var tcs = new TaskCompletionSource<bool>();

            int completed = 0;
            for (var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % numTopics);
                // lock payload to a 250 bytes size random value
                var value = Diffusion.DataTypes.Binary.ReadValue( new byte [ 250 ] );

                session.TopicUpdate.SetAsync(topic, value).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

            Console.WriteLine("All topics Set()");
            return tcs.Task;
        }

        private static async Task SetAllTopicsSteadyRate(ISession session, String topicPath, int numTopics, int updateRate)
        {
            // constants
            const long test_duration = 1 * 60 * 1000; // 10 minutes in milliseconds
            const long frames_per_second = 10; // how many times per second do we batch the updates

            // Accumulators
            long total_updates = 0;
            long total_sleep_time = 0;
            long total_over_time = 0;

            long frame_interval = 1000 / frames_per_second; // time window for each batch update in milliseconds
            long updates_per_frame = updateRate / frames_per_second; // how many updates in each batch update

            Console.WriteLine($"Update has started at {updates_per_frame} updates every {frame_interval} ms for {test_duration} ms.");
            long start_time = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            int round_robin_index = 0;
            Random randomGenerator = new Random();
            while ( true ) {
                long current_time = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                long elapsed_time = current_time - start_time;

                if ( elapsed_time > test_duration ) {
                    Console.WriteLine($"");
                    Console.WriteLine($"");
                    Console.WriteLine($"Elapsed time: {elapsed_time} ms");
                    Console.WriteLine($"Total Sleep time during test: {total_sleep_time} ms");
                    Console.WriteLine($"Total over time: {total_over_time} ms");
                    Console.WriteLine($"Total updates: {total_updates}");
                    float update_rate = total_updates / ((float) elapsed_time / 1000);
                    Console.WriteLine($"Update rate: {update_rate} updates per second");
                    Console.WriteLine($"");
                    Console.WriteLine($"");
                    break;
                }

                // build the task for the update frame
                var update_frame_tcs = new TaskCompletionSource<bool>();
                long completed = 0;

                for (long i = 0; i < updates_per_frame; i++) {
                    // Increment the round robin index within the total number of topics
                    round_robin_index = (round_robin_index + 1) % numTopics;

                    // Topic path determined by the round robin index
                    String topic = topicPath + "/" + round_robin_index;

                    // Payload of 250 random bytes.
                    byte[] randomBytes = new byte[250];
                    randomGenerator.NextBytes(randomBytes);
                    var value = Diffusion.DataTypes.Binary.ReadValue( randomBytes );

                    session.TopicUpdate.SetAsync(topic, value).ContinueWith(
                        setTask => {
                            if(setTask.Exception != null)
                            {
                                Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                                update_frame_tcs.SetResult(false);
                                return;
                            }
                            if(Interlocked.Increment(ref completed) == updates_per_frame)
                            {
                                update_frame_tcs.SetResult(true);
                            }
                        }, TaskContinuationOptions.ExecuteSynchronously);
                }
                bool update_frame_result = await update_frame_tcs.Task;

                // check available time
                long time_after_update = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                long elapsed_frame_time = time_after_update - current_time;
                long sleep_time = frame_interval - elapsed_frame_time;

                if ( !update_frame_result ) {
                    return;
                }
                total_updates += updates_per_frame;

                // sleep if some time left
                if ( sleep_time > 0 ) {
                    total_sleep_time += sleep_time;
                    Thread.Sleep((int)sleep_time);
                }
                // or log how long the update frame took
                else {
                    //Console.WriteLine($"Topic updates took longer than allocated time: {elapsed_frame_time} ms.");
                    total_over_time += - sleep_time;
                }
            }
        }

        private static Task SetAllTopicsThrottled(ISession session, String topicPath, int numTopics, int iterations, int outstandingUpdates)
        {
            var tcs = new TaskCompletionSource<bool>();

            var sem = new SemaphoreSlim(outstandingUpdates);

            int completed = 0;

            for(var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % numTopics);
                String data = "" + GetTimeMillis() + filler;
                var value = Diffusion.DataTypes.Binary.ReadValue(Encoding.UTF8.GetBytes(data));

                // This doesn't work
                // Main thread being locked waiting for a semaphore to be released
                sem.Wait();

                session.TopicUpdate.SetAsync(topic, value).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                        sem.Release();
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

            return tcs.Task;
        }

        private static Task SetAllTopicsThrottledOrdered(ISession session, String topicPath, int numTopics, int iterations, int outstandingUpdates)
        {
            var tcs = new TaskCompletionSource<bool>();

            var sem = new SemaphoreQueue(outstandingUpdates);

            int completed = 0;

            for(var i = 0; i < iterations; i++) {
                string topic = topicPath + "/" + (i % numTopics);
                string data = "" + GetTimeMillis() + filler;

                var value = Diffusion.DataTypes.Binary.ReadValue(Encoding.UTF8.GetBytes(data));

                sem.Wait();

                session.TopicUpdate.SetAsync(topic, value).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                        sem.Release();
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

            return tcs.Task;
        }

        private static Task AddAndSetAllTopics(ISession session, ITopicSpecification spec, String topicPath)
        {
            var tcs = new TaskCompletionSource<bool>();

            int completed = 0;
            for (var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % numTopics);
                String data = "" + GetTimeMillis() + filler;
                var value = Diffusion.DataTypes.Binary.ReadValue(Encoding.UTF8.GetBytes(data));

                session.TopicUpdate.AddAndSetAsync(topic, spec, value).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

            return tcs.Task;
        }

        private class SessionPropertiesListener : ISessionPropertiesListener
        {
            public void OnClose()
            {
            }

            public void OnError(ErrorReason errorReason)
            {
                Console.WriteLine($"An error has occured : {errorReason}.");
            }

            public void OnRegistered(IRegistration registration)
            {
            }

            public void OnSessionClose(ISessionId sessionId, IDictionary<string, string> properties, CloseReason closeReason)
            {
                Console.WriteLine("Closed: " + closeReason);
            }

            public void OnSessionEvent(ISessionId sessionId, SessionPropertiesListenerEventType? eventType, IDictionary<string, string> properties, IDictionary<string, string> previousValues)
            {
               if (eventType.HasValue)
                {
                    Console.WriteLine($"Session with id '{sessionId}' was {eventType.Value}.");
                }
            }

            public void OnSessionOpen(ISessionId sessionId, IDictionary<string, string> properties)
            {
                Console.WriteLine($"Session with id '{sessionId}' has been opened.");
            }

        }
    }
}
