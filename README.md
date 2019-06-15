# websocket_server
A websocket server implementation in C++ using poco libraries

# Extracting boost, poco and gtest
Go to external directory and run following

$ make all

# Building binaries
- Run the following command to build the binaries in bin/ directory

$ make all

- Exectute the following command to start the server on localhost:32000

$ LD_LIBRARY_PATH=../external/poco/lib ./server
  (To stop the server enter "exit" and Enter)

- Exectute the following command to start the client and connect to server on localhost:32000

$ LD_LIBRARY_PATH=../external/poco/lib IP_ADDR=127.0.0.1 PORT=32000 ./client
