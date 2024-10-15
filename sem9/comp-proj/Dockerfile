FROM gradle:8.5.0-jdk11-alpine as builder

WORKDIR /lmao_comp

COPY . ./
RUN gradle uber


FROM amazoncorretto:11-alpine as runner

WORKDIR /lmao_comp

VOLUME ./project
RUN set -o errexit -o nounset \
    && echo "Adding compiler user and group" \
    && addgroup --system --gid 1000 compiler \
    && adduser --system --ingroup compiler --uid 1000 --shell /bin/ash compiler

COPY --from=builder /lmao_comp/build/libs/LMAO_Compiler.jar ./
ENTRYPOINT ["java", "-jar", "LMAO_Compiler.jar"]
