using PushTechnology.ClientInterface.Client.Factories;
using PushTechnology.ClientInterface.Client.Session;
using PushTechnology.ClientInterface.Client.Topics;
using PushTechnology.ClientInterface.Client.Topics.Details;

using System;
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
                    .Principal("admin")
                    .Password("password")
                    .Open(url);
                Console.WriteLine(session.SessionId);

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
                        await AddOnlyOneTopic(session, spec, "dummy");

                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopics(session2, topicPath, numTopics, iterations);
                        break;
                    case 7:
                        Console.WriteLine("Running test: Create two sessions, and set all topics in only one of them");
                        // Reset start time for this test, so we only time the SetAsync() calls
                        startTime = GetTimeMillis();
                        await SetAllTopics(session, topicPath, numTopics, iterations);
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
                String data = "" + GetTimeMillis() + filler;
                var value = Diffusion.DataTypes.Binary.ReadValue(Encoding.UTF8.GetBytes(data));

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

        private static async Task SetAllTopicsThrottled(ISession session, String topicPath, int outstandingUpdates)
        {
            var sem = new SemaphoreSlim(outstandingUpdates);

            var tasks = new List<Task>();

            for(var i = 0; i < iterations; i++) {
                String topic = topicPath + "/" + (i % numTopics);
                String data = "" + GetTimeMillis() + filler;
                var value = Diffusion.DataTypes.Binary.ReadValue(Encoding.UTF8.GetBytes(data));

                await sem.WaitAsync();

                tasks.Add(Task.Run(async () =>
                {
                    await session.TopicUpdate.SetAsync(topic, value);
                    sem.Release();
                }));
            }
            await Task.WhenAll(tasks.ToArray());
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
    }
}
