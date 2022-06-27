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
            try
            {
                session = Diffusion.Sessions
                    .Principal("admin")
                    .Password("password")
                    .Open(url);
                Console.WriteLine(session.SessionId);

                var spec = Diffusion.NewSpecification(TopicType.BINARY)
                    .WithProperty(TopicSpecificationProperty.PublishValuesOnly, "true");

                String topicPath = "test/set/dotnet"; // + "/" + GetTimeMillis();
                // await AddAllTopics(session, spec, topicPath);
                await AddOnlyOneTopic(session, spec, "dummy");

                long startTime = GetTimeMillis();

                // await SetAllTopicsThrottled(session, topicPath, 50);
                // await SetAllTopics(session, topicPath);
                await AddAndSetAllTopics(session, spec, topicPath);

                long endTime = GetTimeMillis();
                Console.WriteLine("Test took " + (endTime - startTime) + " ms");
                Console.WriteLine("= " + ((double)iterations / (endTime - startTime) * 1000) + " msgs/sec");
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
