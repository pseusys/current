# Chat Application

## Build and run

### Server

Run server:
```shell
./gradlew -P mainClass=ds.ChatServer run [--args="SERVER_ARGS"]
```

Where `SERVER_ARGS` can be:
- `-h`, `--help`: Show help message with parameter description and return.
- `-l`, `--load`: File to load message history from.


### Client

Run client:
```shell
./gradlew -P mainClass=ds.ChatClient run [--args="CLIENT_ARGS"]
```

Where `CLIENT_ARGS` can be:
- `-h`, `--help`: Show help message with parameter description and return.
- `-n`, `--name`: Client name to display in chat (default: will be generated).
- `-a`, `--address`: Address of machine to run the client on (default: 'localhost').



## Runtime control

### Server

Control server:
```shell
/COMMAND ARGS
```

Where `COMMAND` can be:
- `help`: Show help message with command description.
- `save`: Save message history to file, specified by ARGS.
- `exit`: Finish server gracefully.  
  NB! `/exit` should be preferred over Ctrl+C in all cases.

### Client

Control client:
```shell
/COMMAND ARGS
```

Where `COMMAND` can be:
- `help`: Show help message with command description.
- `exit`: Finish server gracefully.  
  NB! `/exit` should be preferred over Ctrl+C in all cases.

Send a message:
```shell
MESSAGE
```

MESSAGE will be broadcasted to server.
