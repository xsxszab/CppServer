## CppServer
as long as I don't know how to properly name this project, let's call it cppserver for now.

### Project Description

A lightweight multithreaded TCP server based on the reactor pattern. 
This project is still in progress.

### Requirements
* GCC 7.5
* cmake
* WebBench (for server stress test)

### Features
* handles requests using the master-slave reactor multithreading model.
* uses a thread pool for better concurrency performance.
* priority queue-based timer (in progress).
* http request parsing & response (partially implemented)
* support asynchronous logging.

### Usage
1. build the server shared lib:
```
mkdir build
cd build
cmake ..
make -j4
```

2. build test cases
```
make echo_server
make echo_single_client
make echo_multi_clients
make http_server
```


### References

* https://github.com/sylar-yin/sylar
* https://github.com/forthespada/MyPoorWebServer
* https://github.com/qinguoyi/TinyWebServer
* https://github.com/yuesong-feng/30dayMakeCppServer
