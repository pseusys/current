### Server

Run server:
```shell
./gradlew -P mainClass=ds.ChatServer run [--args="SERVER_ARGS"]
```

Where `SERVER_ARGS` can be:
- `-h`, `--help`: Show help message with parameter description and return.



### Client

Run client:
```shell
./gradlew -P mainClass=ds.ChatClient run [--args="CLIENT_ARGS"]
```

Where `CLIENT_ARGS` can be:
- `-h`, `--help`: Show help message with parameter description and return.
- `-n`, `--name`: Client name to display in chat (default: will be generated).
- `-a`, `--address`: Address of machine to run the client on (default: 'localhost').