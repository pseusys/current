FROM gradle:7.6-jdk11-alpine AS builder

ARG MAIN_CLASS
ARG BASE_NAME
ARG VERSION
ARG WORK_DIR
WORKDIR /$WORK_DIR

COPY build.gradle settings.gradle ./
COPY src ./src/
RUN gradle -P BASE_NAME=$BASE_NAME -P VERSION=$VERSION -P MAIN_CLASS=ds.launchers.IndividualLauncher clean build


FROM amazoncorretto:11.0.18-alpine AS runner

ARG BASE_NAME
ARG VERSION
ARG WORK_DIR
WORKDIR /$WORK_DIR
ENV ARTIFACT_NAME=$BASE_NAME-$VERSION.jar
COPY --from=builder /$WORK_DIR/build/libs/$ARTIFACT_NAME ./

EXPOSE 8080
ENTRYPOINT java -jar $ARTIFACT_NAME
