# Setting topics with .NET

This test will send 10,000,000 x 200 byte updates across 100 topics. The topic type is "binary" and by default delta calculations are disabled.

We focus on the performance of the SetAsync() call here - the cost of adding topics is not counted.

Edit `Program.cs` to chose one of two update techniques:

1. `SetAllTopics(...)` will update topics as fast as possible
2. `SetAllTopicsThrottled(..., outstandingUpdates)` will only allow `outstandingUpdates` to go unacknowledged before sending more updates. This appears to increase throughput, depending on the size of `outstandingUpdates`. Good values for this appear to be around 100, although your environment may be an influencing factor.

You will also have to edit the code to specify how to connect to your particular Diffusion server (Principal, Password and URL).
