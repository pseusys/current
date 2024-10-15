# LMAO Compiler

LMAO MinCaml compiler was created for compilers project assignment by LMAO team of MoSIG M1 students in 2023.
Its name is an acronym of the first letters of the team members names, that are:
- **L** for **Lili Berat**
- **M** for **Meriyam El Moutaraji**
- **A** for **Aleksandr Sergeev**
- **O** for **Olesia Shchukina**
- **Compiler** for **Sofia Duque Gomez** (who agreed to be referred to as "Compiler").

## Dependencies

Following tools are **required** for building and execution of the compiler.
Special requirements may apply to some of the [other options](#other-options).

1. Gradle version >=8.5  
   Gradle binary itself can be of (almost) any version (usually can be installed [from the default package manager](https://gradle.org/install/#with-a-package-manager)).  
   Required version installation can be done with `gradle wrapper --gradle-version=8.5 --distribution-type=bin`.  
   WARNING! In order for this command to succeed, JDK version **must** be compatible with Gradle **binary**.  
   Installation can be verified with `./gradlew --version`.
2. JDK version >=11, <21 [compatible with Gradle](https://docs.gradle.org/current/userguide/compatibility.html)  
   Can be installed with [this installation guide](https://openjdk.org/install/).  
   Installation can be verified with `java --version`.

Build and install dependencies using the following command:
```shell
./gradlew build
```

## Build and run

The execution `PARAMETERS`, mentioned below, are:
- `-o`: Set output file name.
- `-h`: Display help and exit.
- `-v`: Display version and exit.
- `-t`: Check types and exit.
- `-p`: Check syntax (parse) and exit.
- `-asml`: Output ASML source code.
- `-my-opt`: Custom options:
  - `verbose=[DEBUG|INFO|SUCCESS|WARN|ERROR]`: Enable logging below given label (to `stdout`).

The last (required) parameter is the source code file name.

The compiler returns:
- `0` On success.
- `1` On error, error message is written to `stderr`.

> Hint! If there is still any confusion about how do different arguments work with different running configurations,
> feel free to check out the [pipeline file](./.gitlab-ci.yml); it contains several valid compiler running commands.

### Build the compiler

Compiler "fat" (uber) JAR can be built using the following command:
```shell
./gradlew uber
```

The JAR will appear in `build/libs/LMAO_Compiler.jar`.
The JAR can be run with:
```shell
java -jar build/libs/LMAO_Compiler.jar [PARAMETERS]
```

### Link and compile the output

The code created by compiler is not freestanding.
In order to transform it to executable, it should be linked with [`stdlib`](./buildSrc/src/main/resources/stdlibs/arm_stdlib.s).
A special gradle command, that creates assembly code, links it with stdlib and runs it in emulator is provided:

```shell
./gradlew comprun --args="[PARAMETERS]"
```

This command accepts several additional arguments, including:

- `--compiler`: Allows to specify the desired assembly compiler executable, the default: `arm-none-eabi-as`.
- `--linker`:  Allows to specify the desired linker executable, the default: `arm-none-eabi-ld`.
- `--binary`:  Allows to specify the name for the produced binary, the default matches the source file name with `.run` extension.
- `--runner`:  Allows to specify the emulator to execute the produced binary, the default: `qemu-arm`.

### Run example program

The compiler can be run right away, without building any executables:
```shell
./gradlew exec --args="[PARAMETERS]"
```

> NB! Example program names can be found in and should be relative to `buildSrc/src/main/resources/examples`.
> That directory will be set as working directory of the compiler.

### Other options

In case installing Gradle of the required version is troublesome, the following prefix can be used for running Gradle commands in Docker instead.
For all the commands above, `./gradlew [TASK]` can be replaced with this:
```shell
docker run --rm -u gradle -v "$PWD":/home/gradle/project -w /home/gradle/project gradle:8.5.0-jdk11-alpine gradle [TASK]
```

In case installing both dependencies is troublesome, a Docker image can be used to build and run the compiler.
The following command builds local compiler image (including JRE and compiler JAR itself), it will be tagged `compiler:latest`:
```shell
docker build -t compiler:latest .
```

The following command can be used to execute compiler in Docker:
```shell
docker run --rm -u compiler -v "$PWD":/lmao_comp/project compiler:latest [PARAMETERS]
```

> NB! In this setup, the current work directory will correspond to `./project` directory in the Docker container.

## Tests and chores

### Run unit tests

TODO

### Run smoke tests

Smoke tests can be run with the following command:
```shell
./gradlew smoke
```
The tests also check all the examples (they are expected to be processed successfully).

> NB! Smoke tests can be found in `buildSrc/src/main/resources/tests`.

### Format your code

Honestly, we like long lines, and we also use Java, so we removed line length limit (it is set to 500).
Use the following command to check your Java and Kotlin code (using modified `eclipse` code style):
```shell
./gradlew spotlessCheck
```

Use the following command to automatically apply proposed changes (typically should be run at least once before merge of every PR):
```shell
./gradlew spotlessApply
```

> NB! Code will be automatically edited on-site!

## Design choices

Gradle is used as a more progressive build system than make.
In case Gradle is not available in current system (or JDK required by Gradle), corresponding Docker images can be used.

Compiler sources are placed into `src/main` directory.
Unit tests for the compiler are placed into `src/test` directory.

Build tools and scripts are placed into `buildSrc/src/main/kotlin` directory.
Kotlin is used as language for Gradle tasks, since it does not require any external dependencies, is simple and scrip-oriented.

Smoke test sources and MinCaml program examples are placed into `buildSrc/src/main/resources` directory.
If there are any test or example tasks to be created, they should use these sources as package sources.

## ASML features

Several features in our implementation of ASML are different from the proposed one:
1. Special keyword `FGE` (represented by `>=.` symbol) is added for consistency with `GE` keyword.
2. Object `asmt` can only be in brackets in `if` statement body, otherwise brackets are not generated (but still can be parsed).
3. Object `expr` is never generated in brackets (still brackets can be parsed).

## Other sections...
