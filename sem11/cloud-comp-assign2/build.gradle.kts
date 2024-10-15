plugins {
    `java-library`
    `maven-publish`
}

repositories {
    mavenLocal()
    maven {
        url = uri("https://repository.cloudera.com/artifactory/cloudera-repos/")
    }

    maven {
        url = uri("https://repo.maven.apache.org/maven2/")
    }
}

dependencies {
    api(libs.org.apache.hadoop.hadoop.client)
    api(libs.org.apache.mrunit.mrunit) { artifact { classifier = "hadoop2" } }
    api(libs.org.apache.hadoop.hadoop.minicluster)
    api(libs.commons.cli.commons.cli)
    testImplementation(libs.junit.junit)
}



group = "hk.ust.comp4651"
version = "1.0-SNAPSHOT"
description = "assignment-2"
java.sourceCompatibility = JavaVersion.VERSION_1_8

publishing {
    publications.create<MavenPublication>("maven") {
        from(components["java"])
    }
}

tasks.withType<JavaCompile>() {
    options.encoding = "UTF-8"
}

tasks.withType<Javadoc>() {
    options.encoding = "UTF-8"
}



tasks.create("warmup1", RunHadoopTask::class) {
    dependsOn("jar")
    main = "WordCount"
    reducers = 2
}

tasks.create("warmup1-analyze", AnalyzeHadoopTask::class) {
    dependsOn("jar", "warmup1")
    main = "AnalyzeWordCount"
}

tasks.create("warmup2-pairs", RunHadoopTask::class) {
    dependsOn("jar")
    main = "BigramCountPairs"
    reducers = 2
}

tasks.create("warmup2-pairs-analyze", AnalyzeHadoopTask::class) {
    dependsOn("jar", "warmup2-pairs")
    main = "AnalyzeBigramCount"
}

tasks.create("warmup2-stripes", RunHadoopTask::class) {
    dependsOn("jar")
    main = "BigramCountStripes"
    reducers = 2
}

tasks.create("warmup2-stripes-analyze", AnalyzeHadoopTask::class) {
    dependsOn("jar", "warmup2-stripes")
    main = "AnalyzeBigramCount"
}

tasks.create("task1-pairs", RunHadoopTask::class) {
    dependsOn("jar")
    main = "BigramFrequencyPairs"
    reducers = 2
}

tasks.create("task1-pairs-analyze", AnalyzeHadoopTask::class) {
    dependsOn("jar", "task1-pairs")
    main = "AnalyzeBigramFrequency"
    word = "the"
}

tasks.create("task1-stripes", RunHadoopTask::class) {
    dependsOn("jar")
    main = "BigramFrequencyStripes"
    reducers = 2
}

tasks.create("task1-stripes-analyze", AnalyzeHadoopTask::class) {
    dependsOn("jar", "task1-stripes")
    main = "AnalyzeBigramFrequency"
    word = "the"
}

tasks.create("task2-pairs", RunHadoopTask::class) {
    dependsOn("jar")
    main = "CORPairs"
    input = project.layout.projectDirectory.file("cor-demo-sample.txt")
    reducers = 1
}

tasks.create("task2-stripes", RunHadoopTask::class) {
    dependsOn("jar")
    main = "CORStripes"
    input = project.layout.projectDirectory.file("cor-demo-sample.txt")
    reducers = 1
}

tasks.create("task2-analyze", Exec::class) {
    commandLine("python3", "demo/cor-demo.py")
}
