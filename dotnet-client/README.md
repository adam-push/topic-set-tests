# Setting topics with .NET

This test will send 10,000,000 x 200 byte updates across 100 topics. The topic type is "binary" and by default delta calculations are disabled.

We focus on the performance of the SetAsync() call here - the cost of adding topics is not counted.

To run, e.g.:
```
dotnet run <url> <number of update> <test number>
```

1. Add all topics then set them
2. Add and set all topics (AddAndSetAsync)
3. Not yet implements
4. Set topics only (topics should already exist))
5. Add a single topic, then set all the others
6. Add a single topic in one sessin, then set the other topics in a different session
7. Create two sessions; set the topics in one of them
8. Add all topics, then use (unordered) client-side throttling with SetAsync()
9. Add all topics, then use unordered client-side throttling with SetAsync()

You will also have to edit the code to specify how to connect to your particular Diffusion server (Principal, Password and URL).
