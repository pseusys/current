# AWS instance quick-remember guide

Hi Alex!
Seems like you've forgotten how to run these weird AWS labs again...
No problems, here's a list:

1. Navigate [here](https://awsacademy.instructure.com/courses/92653/modules/items/8528473).
2. Click `Start Lab` and then click `AWS`.
3. Click `EC2` in the new tab.

## Run CPU, memory and file I\O benchmarking

Set following environment variables:

- `KEY_FILE`: benchmarking instance SSH key.
- `ADDRESS`: benchmarking instance address.

Run:

```shell
make -f Makefile-benchmark benchmark
```

## Run network throughput and RTT benchmarking

Set following environment variables:

- `KEY_FILE`: benchmarking instances SSH key.
- `SERVER_ADDRESS`: server benchmarking instance address.
- `CLIENT_ADDRESS`: client benchmarking instance address.

Run (in background or separate terminal):

```shell
make -f Makefile-network server
```

Run:

```shell
make -f Makefile-network client
```
