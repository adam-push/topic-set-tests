using PushTechnology.ClientInterface.Client.Factories;
using PushTechnology.ClientInterface.Client.Session;
using PushTechnology.ClientInterface.Client.Topics;
using PushTechnology.ClientInterface.Client.Topics.Details;
using PushTechnology.ClientInterface.Client.Callbacks;
using PushTechnology.ClientInterface.Client.Features.Control.Clients;
using PushTechnology.ClientInterface.Data.Binary;

using System;
using System.IO;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace dotnet_test
{
    class PerformanceLogEntry
    {
        private string label;
        private int testNumber;
        private int totalTopics;
        private long timestamp;
        private int targetUpdateRate;
        private float updateRate;

      public PerformanceLogEntry(string label, int testNumber, int totalTopics, long timestamp, int targetUpdateRate, float updateRate) {
          this.label = label;
          this.testNumber = testNumber;
          this.totalTopics = totalTopics;
          this.timestamp = timestamp;
          this.targetUpdateRate = targetUpdateRate;
          this.updateRate = updateRate;
      }

      public static string GetHeader() {
          return "Label,Test Number,Total Topics,Timestamp,Target Update Rate,Update Rate";
      }

      public string GetValue() {
          return string.Format("{0},{1},{2},{3},{4},{5}", label, testNumber, totalTopics, timestamp, targetUpdateRate, updateRate);
      }
    }

    class Program
    {
        static string url;
        static int totalTopics;
        static int targetUpdateRate;
        static int testDurationMinutes;
        static int testNumber;
        static string label;
        static List<PerformanceLogEntry> performanceLogEntries;

        static async Task Main(string[] args)
        {
            if(args.Length < 6)
            {
                Console.WriteLine("Arguments: <url> <total_topics> <target_update_rate> <test_duration_minutes> <test_number> <label>");
                Environment.Exit(1);
            }

            url = args[0];
            totalTopics = Int32.Parse(args[1]);
            targetUpdateRate = Int32.Parse(args[2]);
            testDurationMinutes = Int32.Parse(args[3]);
            testNumber = Int32.Parse(args[4]);
            label = args[5];
            performanceLogEntries = new List<PerformanceLogEntry>();

            await Run();

            WritePerformanceLog();
        }

        static async Task Run()
        {
            ISession session = null;

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

                var spec = Diffusion.NewSpecification(TopicType.BINARY)
                    .WithProperty(TopicSpecificationProperty.PublishValuesOnly, "true");

                const String topicPath = "test/set/dotnet";

                await AddAllTopics(session, spec, topicPath, totalTopics);
                await SetAllTopicsSteadyRate(session, topicPath, totalTopics, targetUpdateRate, testDurationMinutes);

                session?.Close();
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

        private static Task AddAllTopics(ISession session, ITopicSpecification spec, String topicPath, int totalTopics)
        {
            TaskCompletionSource<bool> tcs = new TaskCompletionSource<bool>();
            int completedAddTasks = 0;

            for(var i = 0; i < totalTopics; i++) {
                String topic = topicPath + "/" + i;
                session.TopicControl.AddTopicAsync(topic, spec).ContinueWith(
                    addTask => {
                        if(addTask.Exception != null)
                        {
                            Console.WriteLine($"Exception adding topic {topic}: {addTask.Exception}");
                        }
                        if(Interlocked.Increment(ref completedAddTasks) == totalTopics)
                        {
                            Console.WriteLine($"Topics created: {totalTopics}");
                            tcs.SetResult(true);
                        }
                    }, TaskContinuationOptions.ExecuteSynchronously);
            }

           return tcs.Task;
        }

        private static async Task SetAllTopicsSteadyRate(ISession session, String topicPath, int totalTopics, int updateRate, int durationMinutes)
        {
            // constants
            long testDurationMillis = durationMinutes * 60 * 1000; // test duration in milliseconds
            long framesPerSecond = 10; // how many times per second do we batch the updates

            // Accumulators
            long totalUpdates = 0;
            long totalSleepTime = 0;
            long totalOverTime = 0;

            long frameInterval = 1000 / framesPerSecond; // time window for each batch update in milliseconds
            long updatesPerFrame = updateRate / framesPerSecond; // how many updates in each batch update

            Console.WriteLine($"Update has started at {updatesPerFrame} updates every {frameInterval} ms for {testDurationMillis} ms.");
            long startTime = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
            int roundRobinIndex = 0;
            Random randomGenerator = new Random();
            while ( true ) {
                long currentTime = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                long elapsedTime = currentTime - startTime;

                if ( elapsedTime > testDurationMillis ) {
                    Console.WriteLine($"");
                    Console.WriteLine($"Elapsed time: {elapsedTime} ms");
                    Console.WriteLine($"Total Sleep time during test: {totalSleepTime} ms");
                    Console.WriteLine($"Total over time: {totalOverTime} ms");
                    Console.WriteLine($"Total updates: {totalUpdates}");
                    float averageUpdateRate = totalUpdates / ((float) elapsedTime / 1000);
                    Console.WriteLine($"Average Update rate: {averageUpdateRate} updates per second");
                    Console.WriteLine($"");
                    break;
                }

                // build the task for the update frame
                var updateFrameTcs = new TaskCompletionSource<bool>();
                long completed = 0;

                for (long i = 0; i < updatesPerFrame; i++) {
                    // Increment the round robin index within the total number of topics
                    roundRobinIndex = (roundRobinIndex + 1) % totalTopics;

                    // Topic path determined by the round robin index
                    String topic = topicPath + "/" + roundRobinIndex;

                    // Payload of 250 random bytes.
                    byte[] randomBytes = new byte[250];
                    randomGenerator.NextBytes(randomBytes);
                    var value = Diffusion.DataTypes.Binary.ReadValue( randomBytes );

                    session.TopicUpdate.SetAsync(topic, value).ContinueWith(
                        setTask => {
                            if(setTask.Exception != null)
                            {
                                Console.WriteLine($"Exception publishing to {topic}: {setTask.Exception}");
                                updateFrameTcs.SetResult(false);
                                return;
                            }
                            if(Interlocked.Increment(ref completed) == updatesPerFrame)
                            {
                                updateFrameTcs.SetResult(true);
                            }
                        }, TaskContinuationOptions.ExecuteSynchronously);
                }
                bool updateFrameResult = await updateFrameTcs.Task;

                // check available time
                long timeAfterUpdate = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                long elapsedFrameTime = timeAfterUpdate - currentTime;
                long sleepTime = frameInterval - elapsedFrameTime;

                if ( !updateFrameResult ) {
                    return;
                }
                totalUpdates += updatesPerFrame;

                float updateRateForFrame = (float) Math.Round(1000 * (updatesPerFrame / (float) elapsedFrameTime), 1);
                AddToLog((timeAfterUpdate - startTime), updateRateForFrame);

                // sleep if some time left
                if ( sleepTime > 0 ) {
                    totalSleepTime += sleepTime;
                    Thread.Sleep((int)sleepTime);
                }
                // or log how long the update frame took
                else {
                    totalOverTime += - sleepTime;
                }
            }
        }

        private static void AddToLog(long timestamp, float updateRate) {
            performanceLogEntries.Add(new PerformanceLogEntry(label, testNumber, totalTopics, timestamp, targetUpdateRate, updateRate));
        }

        private static void WritePerformanceLog() {
            string filePath = "results.csv";
            FileInfo fi = new FileInfo(filePath);

            StreamWriter stream;

            if (fi.Exists) {
                stream = fi.AppendText();
            } else {
                stream = fi.CreateText();
                stream.WriteLine(PerformanceLogEntry.GetHeader());
            }

            foreach (PerformanceLogEntry entry in performanceLogEntries) {
                stream.WriteLine(entry.GetValue());
            }

            stream.Close();
        }
    }
}
