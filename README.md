### Project Description

A lightweight multithreaded TCP server based on the reactor pattern. Currently this project is still in progress.


### Requirements
* GCC 7.5
* cmake
* WebBench (for server stress test)

### Features
* handles requests using the master-slave reactor multithreading model.
* uses a thread pool for better concurrency performance.
* priority queue-based timer (in progress).
* http request parseing & response (partially implemented)
* support asynchronous logging.


### References

* https://github.com/sylar-yin/sylar
* https://github.com/forthespada/MyPoorWebServer
* https://github.com/qinguoyi/TinyWebServer
* https://github.com/yuesong-feng/30dayMakeCppServer
