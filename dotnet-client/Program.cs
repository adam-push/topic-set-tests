using PushTechnology.ClientInterface.Client.Factories;
using PushTechnology.ClientInterface.Client.Features;
using PushTechnology.ClientInterface.Client.Features.Control.Topics;
using PushTechnology.ClientInterface.Client.Session;
using PushTechnology.ClientInterface.Client.Topics;
using PushTechnology.ClientInterface.Client.Topics.Details;

using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace dotnet_test
{
    class Program
    {
        const int dflt_iterations = 1_000_000;
        const int dflt_numTopics = 100;
        const string filler = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            + "ABCDE";
        
        static async Task Main(string[] args)
        {
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
                    .Open("ws://localhost:8090");
                Console.WriteLine(session.SessionId);

                var spec = Diffusion.NewSpecification(TopicType.BINARY)
                    .WithProperty(TopicSpecificationProperty.PublishValuesOnly, "true");
                
                String topicPath = "test/set/dotnet"; // + "/" + GetTimeMillis();
                // await AddAllTopics(session, spec, topicPath, dflt_numTopics);
                await AddOnlyOneTopic(session, spec, "dummy");
                
                long startTime = GetTimeMillis();

                // await SetAllTopicsThrottled(session, topicPath, dflt_numTopics, dflt_iterations, 50);
                await SetAllTopics(session, topicPath, dflt_numTopics, dflt_iterations);
                
                long endTime = GetTimeMillis();
                Console.WriteLine("Test took " + (endTime - startTime) + " ms");
                Console.WriteLine("= " + ((double)dflt_iterations / (endTime - startTime) * 1000) + " msgs/sec");                
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
        
        private static Task AddAllTopics(ISession session, ITopicSpecification spec, String topicPath, int numTopics)
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


        private static async Task SetAllTopicsThrottled(ISession session, String topicPath, int numTopics, int iterations, int outstandingUpdates)
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
    }
}
