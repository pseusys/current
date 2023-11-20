FROM alpine:latest

RUN apk add --no-cache bash gcc musl-dev make gcc

WORKDIR /app/viscomp-lab5

COPY ./viscomp-lab5 ./
COPY ./viscomp-lab1 ../viscomp-lab1

ENTRYPOINT ["make"]
