using PushTechnology.ClientInterface.Client.Factories;
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
        const int DEFAULT_TOTAL_UNIQUE_TOPIC_VALUES = 1;
        const int DEFAULT_TOPIC_VALUE_SIZE = 250;
        const int DEFAULT_MESSAGE_QUEUE_SIZE = 10_000;
        const int DEFAULT_TOTAL_TOPICS = 100;

        // [0]: URL
        static string url;

        // [1]: Iterations
        // total topic value changes applied to the topics
        static int iterations;

        // [2]: Test number
        // Index for the various tests this program contains:
        // - 1: Add all topics then set them
        // - 2: Add and set all topics
        // - 3: Not Implemented
        // - 4: Update existing topics
        // - 5: Add a single topic, then set all the others
        // - 6: Add a single topic in one session, then set the other topics in a different session
        // - 7: Create two sessions, and set all topics in only one of them
        // - 8: Add all topics, then set them with unordered client-side throttling
        // - 9: Add all topics, then set them with ordered client-side throttling
        // - 10: Add all topics, then set them with a steady update rate
        static int test_number;

        // [3]: Total unique topic values
        // The test program will create these many unique topic values and iterate between them to set the topic values.
        // Default value: DEFAULT_TOTAL_UNIQUE_TOPIC_VALUES
        static int total_unique_topic_values;

        // [4]: Topic value size
        // The size of each random topic value
        // Default value: DEFAULT_TOPIC_VALUE_SIZE
        static int topic_value_size;

        // [5]: Session message queue
        // The size of the message queue of the publishing session
        // Default value: DEFAULT_MESSAGE_QUEUE_SIZE
        static int message_queue_size;

        // [6]: Total topics
        // The amount of topics created and updated in the test
        // Default value: 100
        static int total_topics;

        private static List<CancellationTokenSource> cancellationTokenSources;

        private static List<IBinary> uniqueValues;

        private static byte[] GetByteArray(long sizeInBytes)
        {
            Random rnd = new Random();
            byte[] b = new byte[sizeInBytes];
            rnd.NextBytes(b);
            return b;
          }

        static async Task Main(string[] args)
        {
            if(args.Length < 3)
            {
                Console.WriteLine("Arguments: <url> <iterations> <test_number> [<unique_topic_values> [<topic_value_size> [<message_queue_size> [<total_topics>]]]]");
                Environment.Exit(1);
            }

            url = args[0];
            iterations = Int32.Parse(args[1]);
            test_number = Int32.Parse(args[2]);
            total_unique_topic_values = ( args.Length >= 4 ) ? Int32.Parse( args[3] ) : DEFAULT_TOTAL_UNIQUE_TOPIC_VALUES;
            topic_value_size = ( args.Length >= 5 ) ? Int32.Parse( args[4] ) : DEFAULT_TOPIC_VALUE_SIZE;
            message_queue_size = ( args.Length >= 6 ) ? Int32.Parse( args[5] ) : DEFAULT_MESSAGE_QUEUE_SIZE;
            total_topics = ( args.Length >= 7 ) ? Int32.Parse( args[6] ) : DEFAULT_TOTAL_TOPICS;

            cancellationTokenSources = new List<CancellationTokenSource>();

            // Define the message content that will be sent during the test
            uniqueValues = new List<IBinary>();
            for ( int i = 0; i < total_unique_topic_values; i++ ) {
                byte[] messagePayload = Program.GetByteArray(topic_value_size);
                uniqueValues.Add(Diffusion.DataTypes.Binary.ReadValue(messagePayload));
            }
            Console.WriteLine($"Test using {total_unique_topic_values} unique topic values in round-robin.");

            await Run();
        }

        static async Task Run()
        {
            ISession session = null;
            ISession session2 = null;

			Console.WriteLine("List of loaded assemblies :");
			foreach (System.Reflection.Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                Console.WriteLine(assembly.ToString());
            }

            try
            {
                session = Diffusion.Sessions
                    .SessionStateChangedHandler((sender, args) => {
                        Console.WriteLine("Session state changed by " + sender + " to " + args.NewState);
                        })
                    .MaximumQueueSize(message_queue_size)
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
                bool result = false;

                switch(test_number)
                {
                    case 1:
                        Console.WriteLine("Running test: Add all topics then set them");
                        result = await AddAllTopics(session, spec, topicPath);

                        if (result) {
                            Console.WriteLine("Topics have been added.");

                            // Reset start time for this test, so we only time the SetAsync() calls
                            startTime = GetTimeMillis();
                            result = await SetAllTopics(session, topicPath, total_topics, iterations);
                        }
                        break;

                    case 2:
                        Console.WriteLine("Running test: Add and set all topics");
                        result = await AddAndSetAllTopics(session, spec, topicPath);
                        break;

                    case 3:
                        Console.WriteLine("Not implemented: Use UpdateStreams to update the topics");
                        break;

                    case 4:
                        Console.WriteLine("Running test: Update existing topics");
                        result = await SetAllTopics(session, topicPath, total_topics, iterations);
                        break;

                    case 5:
                        Console.WriteLine("Running test: Add a single topic, then set all the others");
                        await AddOnlyOneTopic(session, spec, "dummy");

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        result = await SetAllTopics(session, topicPath, total_topics, iterations);
                        break;

                    case 6:
                        Console.WriteLine("Running test: Add a single topic in one session, then set the other topics in a different session");
                        await AddOnlyOneTopic(session2, spec, "dummy");

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        result = await SetAllTopics(session, topicPath, total_topics, iterations);
                        break;

                    case 7:
                        Console.WriteLine("Running test: Create two sessions, and set all topics in only one of them");
                        result = await SetAllTopics(session, topicPath, total_topics, iterations);
                        break;

                    case 8:
                        Console.WriteLine("Running test: Add all topics, then set them with unordered client-side throttling");
                        await AddAllTopics(session, spec, topicPath);

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        result = await SetAllTopicsThrottled(session, topicPath, total_topics, iterations, 10);
                        break;

                    case 9:
                        Console.WriteLine("Running test: Add all topics, then set them with ordered client-side throttling");
                        await AddAllTopics(session, spec, topicPath);

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        result = await SetAllTopicsThrottledOrdered(session, topicPath, total_topics, iterations, 10);
                        break;

                    case 10:
                        Console.WriteLine("Running test: Add all topics, then set them with a steady update rate.");
                        await AddAllTopics(session, spec, topicPath);
                        result = await SetAllTopicsSteadyRate(session, topicPath, total_topics, iterations);
                        break;

                    default:
                        Console.WriteLine("Invalid test number: " + test_number);
                        break;
                }

                if (result == false) {
                    Console.WriteLine("Test Failed");
                }
                else {
                    long endTime = GetTimeMillis();
                    long elapsedTimeMs = endTime - startTime;
                    double totalPayloadMB = (double) topic_value_size * (iterations / (double) (1024 * 1024));
                    double averageUpdateRate = ((double)iterations / elapsedTimeMs) * 1000;
                    double averageThroughput = (totalPayloadMB / elapsedTimeMs) * 1000;
                    DateTimeOffset now = (DateTimeOffset)DateTime.UtcNow;

                    Console.WriteLine($"Test took {(endTime - startTime)} ms");
                    Console.WriteLine($"Total Payload = {Math.Round(totalPayloadMB)} MB");
                    Console.WriteLine($"Average Update Rate = {Math.Round(averageUpdateRate)} updates/s");
                    Console.WriteLine($"Average Throughput = {Math.Round(averageThroughput)} MB/s");
                    Console.WriteLine($"Test Timestamp = {now.ToUnixTimeSeconds()}");
                }

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

        private static Task<bool> AddAllTopics(ISession session, ITopicSpecification spec, String topicPath)
        {
            TaskCompletionSource<bool> tcs = new TaskCompletionSource<bool>();
            int completedAddTasks = 0;

            for(var i = 0; i < total_topics; i++) {
                String topic = topicPath + "/" + i;
                session.TopicControl.AddTopicAsync(topic, spec).ContinueWith(
                    addTask => {
                        if(addTask.Exception != null)
                        {
                            Console.WriteLine($"Exception adding topic {topic}: {addTask.Exception}");
                            // terminate loop if exception is caught
                            tcs.SetResult(false);
                        }
                        else if(Interlocked.Increment(ref completedAddTasks) == total_topics)
                        {
                            Console.WriteLine($"All topics created");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }
            return tcs.Task;
        }

        private static Task<bool> SetAllTopics(ISession session, String topicPath, int total_topics, int iterations)
        {
            var tcs = new TaskCompletionSource<bool>();

            CancellationTokenSource tokenSource = new CancellationTokenSource();
            cancellationTokenSources.Add(tokenSource);

            int completed = 0;
            for (var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % total_topics);
                var value = uniqueValues[i % total_unique_topic_values];

                session.TopicUpdate.SetAsync(topic, value, tokenSource.Token).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                            CancelAllTasks();
                            // terminate loop if exception is caught
                            tcs.SetResult(false);
                        }
                        else if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }
            return tcs.Task;
        }

        private static async Task<bool> SetAllTopicsSteadyRate(ISession session, String topicPath, int total_topics, int updateRate)
        {
            // constants
            const long test_duration = 10 * 60 * 1000; // test duration in milliseconds
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

                for (int i = 0; i < updates_per_frame; i++) {
                    // Increment the round robin index within the total number of topics
                    round_robin_index = (round_robin_index + 1) % total_topics;

                    // Topic path determined by the round robin index
                    String topic = topicPath + "/" + round_robin_index;
                    var value = uniqueValues[i % total_unique_topic_values];

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
                    return false;
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
            return true;
        }

        private static Task<bool> SetAllTopicsThrottled(ISession session, String topicPath, int total_topics, int iterations, int outstandingUpdates)
        {
            var tcs = new TaskCompletionSource<bool>();
            var sem = new SemaphoreSlim(outstandingUpdates);
            int completed = 0;

            // Define a message content that will be sent in all iterations
            byte[] messagePayload = Program.GetByteArray(topic_value_size);
            var value = Diffusion.DataTypes.Binary.ReadValue(messagePayload);

            for(var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % total_topics);

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

        private static Task<bool> SetAllTopicsThrottledOrdered(ISession session, String topicPath, int total_topics, int iterations, int outstandingUpdates)
        {
            var tcs = new TaskCompletionSource<bool>();
            var sem = new SemaphoreQueue(outstandingUpdates);
            int completed = 0;

            // Define a message content that will be sent in all iterations
            byte[] messagePayload = Program.GetByteArray(topic_value_size);
            var value = Diffusion.DataTypes.Binary.ReadValue(messagePayload);

            for(var i = 0; i < iterations; i++) {
                string topic = topicPath + "/" + (i % total_topics);
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

        private static Task<bool> AddAndSetAllTopics(ISession session, ITopicSpecification spec, String topicPath)
        {
            var tcs = new TaskCompletionSource<bool>();

            CancellationTokenSource tokenSource = new CancellationTokenSource();
            cancellationTokenSources.Add(tokenSource);

            int completed = 0;
            for (var i = 0; i < iterations; i++)
            {
                String topic = topicPath + "/" + (i % total_topics);
                var value = uniqueValues[i % total_unique_topic_values];

                // byte[] messagePayload = Program.GetByteArray(topic_value_size);
                // var discardedValue = Diffusion.DataTypes.Binary.ReadValue(messagePayload);
                // Console.WriteLine($"Generated random binary data that will not be used: {discardedValue}");

                session.TopicUpdate.AddAndSetAsync(topic, spec, value, tokenSource.Token).ContinueWith(
                    setTask => {
                        if(setTask.Exception != null)
                        {
                            Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                            CancelAllTasks();
                            // terminate loop if exception is caught
                            tcs.SetResult(false);
                        }
                        else if(Interlocked.Increment(ref completed) == iterations)
                        {
                            Console.WriteLine($"All updates done");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

            return tcs.Task;
        }

        private static void CancelAllTasks() {
            Console.WriteLine("Cancelling all tasks");
            foreach (CancellationTokenSource tokenSource in cancellationTokenSources) {
                tokenSource.Cancel();
            }
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
