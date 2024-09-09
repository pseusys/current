# Distributed systems project

[![BUILD](https://github.com/pseusys/distributed_project/actions/workflows/simple.yml/badge.svg)](https://github.com/pseusys/distributed_project/actions/workflows/simple.yml)

## Requirements and building

1. Gradle 7.6  
   Can be installed with [this installation guide](https://gradle.org/install/#with-the-gradle-wrapper).  
   The command will be `./gradlew wrapper --gradle-version=7.6 --distribution-type=bin`.  
   Installation can be verified with `./gradlew --version`.
2. JDK 11+  
   Can be installed with [this installation guide](https://openjdk.org/install/).  
   Installation can be verified with `java --version`.

Build and install dependencies using:

```bash
./gradlew build clean
```

## Build and run

> NB! Every launcher needs `RabbitMQ 3.11` up and running on current host (localhost) on default ports (5672, 15672).  

To run `RabbitMQ`, the following command can be used:

```bash
docker run -it --rm --name Rabbitmq -p 5672:5672 -p 15672:15672 rabbitmq:3.11-management
```

In that case the following command can be used to check if `RabbitMQ` is available:

```bash
docker exec Rabbitmq rabbitmq-diagnostics -q check_running
```

> Configuration files:  
  Each launcher can accept two parameters or zero parameters (in that case the defaults will be used).  
  First parameter is a configuration file (without file extension) (physical first, virtual second).  
  These files should reside in `src/main/resources` directories.  
  Files should contain physical and virtual configuration matrixes in `.CSV` format, where connected nodes are represented by `1` and not connected by `-1`.

### Simple launcher

Simple launcher runs all the nodes in a single thread:

```bash
./gradlew -P MAIN_CLASS=ds.launchers.SimpleLauncher run [--args="PHYSICAL_CONFIG VIRTUAL_CONFIG"]
```

### Process launcher

Process launcher runs all the nodes in different native processes (fork / join):

```bash
./gradlew -P MAIN_CLASS=ds.launchers.ProcessLauncher run [--args="PHYSICAL_CONFIG VIRTUAL_CONFIG"]
```

### Individual launcher

Individual launcher runs a single node (gets its' id from `PHYSICAL_ID` environmental variable).  
Can be used to run system on separate processes / machines / hosts / containers:

```bash
./gradlew -P MAIN_CLASS=ds.launchers.IndividualLauncher run [--args="PHYSICAL_CONFIG VIRTUAL_CONFIG"]
```

Run whole system using default configurations in separate Docker containers:

```bash
bash docker/run_containers.sh
```
