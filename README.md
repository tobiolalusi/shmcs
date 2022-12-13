# SHMCS (SHared Memory Client-Server)
This software holds a server and client program that communicate via shared-memory (POSIX shm). The server hosts a
chained hashtable, to which the clients may send operation requests like insert, read and delete to execute on the
shared memory server.

## Concept
It is to be assumed that the clients here are trusted and like ideally, multiple clients may attempt to send request to
the server and each one of them expect their <u>own</u> responses. Also, the communication between the client and
server is over a <u>single</u> shared memory.

Once the server starts, it creates a shared memory, to which any client may connect to in order to communicate with
the server. Additionally, the server also creates (named) semaphores, to synchronize read and writes to the shared
memory. The server keeps waiting for message inside the shared memory, before it handles them.

Once the client starts, it connects to this shared memory, as well as the (named) semaphores that have been created
by the server. A client is immediately allowed to send a message when the server is idle or isn't handling any request.

After the client sends a message, the synchronization mechanisms block every other incoming requests in order to let
the server handle the received message. After the message has been handled, the server writes back to the shared
memory, from which the client reads the response of the server from. Once the client takes out the message, the shared
memory is again available for queued client requests.

## Creating build
Messages are serialized before being sent and deserialized after received. This software uses `protobuf` to easily
perform serialization and deserialization of the messages. The following libraries are thus required to be installed,
in order to build the programs. These may however already exist in some linux distributions.
```
libprotobuf-dev libfmt-dev
```
[1] Build CMake project:
```shell
cmake -B build
```
[2] Build programs:
```shell
cmake --build build --target all
```

## Program execution
After the programs have been built, they can be executed as follows:

### Server:
```shell
./build/server [SHM_NAME] [BUCKETS]
```
- `[SHM_NAME]` represents the name of the shared memory to be created by the server. The clients would also use this name
to connect to the respective server.
- `[BUCKETS]` holds the number of buckets the server would have for the hashtable.

### Client:
```shell
./build/client [SHM_NAME] [OPERATION] [ARGS...]
```
- `[SHM_NAME]` like mentioned in previous section is the target server the client should connect to.
- `[OPERATION] [ARGS...]` represents the operation to be preformed on the server with the stated args.
The following operations are possible client operations:
  - `insert [ARGS...]`: inserts the keys into the hashtable
  - `read [ARG]`: reads the keys inside the bucket in arg. The `[ARG]` should be index of the bucket to be read from.
  - `delete [ARGS...]`: deletes the keys from the hashtable.

## Challenge

### Handling multiple clients
When compared to sockets, we could let the server handle multiple requests at the same time via multithreading, but
this is however more challenging with shared memory, since we have a single resource that is to be read and written to.
Due to the fact that a client would want its very own response after making a request (and not that of another client),
multithreading could not be used. In a multithreaded environment, it is expected that after a thread already completes
its job, it should send the response to the shared memory. This can be done, but the client that may read next
from the shared memory might not be the target client for that response. The implementation here allows for the server
to handle one request at a time, while the clients wait for access to the shared memory via semaphores.
