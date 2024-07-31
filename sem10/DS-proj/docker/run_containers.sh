#!/bin/bash

IMAGE_NAME_TAG=ds-proj

BASE_NAME=distributed_project
VERSION=0.1
WORK_DIR=ds-project

docker build --build-arg BASE_NAME=${BASE_NAME} --build-arg VERSION=${VERSION} --build-arg WORK_DIR=${WORK_DIR} -f docker/Dockerfile --rm -t ${IMAGE_NAME_TAG} .

for i in {0..4}; do
   docker run -td --rm --network host -e PHYSICAL_ID=${i} --log-driver=journald --log-opt tag=${IMAGE_NAME_TAG} ${IMAGE_NAME_TAG}
done

journalctl CONTAINER_TAG=${IMAGE_NAME_TAG} -f -n 0 -o cat
