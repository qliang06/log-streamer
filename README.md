# logstreamer

Design log stream processor to monitor logs format as:

> "remotehost", "rfc931", "authuser", "date", "request", "status", "bytes"  
> "10.0.0.1",   "-", "apache", "1549574332", "GET /api/user HTTP/1.0", 200, 1234

- Date is unix time format.
- Try to make one pass to go through log file.  

# Requirements

**1**. *For everything 10 secs of log lines, display stats about the traffic during those 10s the sections lf the website with the most hist, as well as statistics that might be useful for debugging. A section is defined as being what's before the second '/' in the resource section of the log line. For example, the section for "/api/user" is "/api" and the section for "/report" is "/report"*  
**2**. *Whenever total trafffic for the past 2 mins exceeds a certain number on average, print a message to the console saying that "high traffic generated on alert - hits = {value}, triggered at {time} . The default threshold should be 10 request per second but should be configurable.*   
**3**. *Whenever the total traffic drops again below that value on average for the past 2 mins, print another message detailing when the alert recovered, +/- a second.*  
**4**. *Consider the efficiency of your solution and how it would scale to process high volumes of log lines, dont assume you can load file into memory.*  
**5**. *Write your solution as you would any piece of code that others might need to modify and maintain, in terms of structure and style.*  
**6**. *Write test for alerting logic.*  

# How to build

> **System environment requirement:** gcc 7.0+, cmake 2.6+  
> **Google Test, Google Mock:** libgtest-dev, google-mock  
> Install gtest/gmock on ubuntu/debian:   
>
> - sudo apt-get install libgtest-dev google-mock  
> - cd /usr/src/gtest  
> - sudo make && sudo cp \*.a /usr/lib  
> - cd /usr/src/gmock  
> - sudo make && sudo cp \*.a /usr/lib   

Under root directory:  

1. cmake CmakeLists.txt  
2. make  

**Run main program**  
./log_analysis \<filename\>  
./log_analysis \<filename\> \<traffic threshold\>  
**Run tests**  
./log_analysis_test
 
