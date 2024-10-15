plugins {
	id("java")
	id("application")
	id("cup.gradle.cup-gradle-plugin") version "2.0"
	id("org.xbib.gradle.plugin.jflex") version "3.0.2"
	id("com.diffplug.spotless") version "6.23.3"
}

group = "team.lmao"
version = "1.0-SNAPSHOT"


java {
	sourceCompatibility = JavaVersion.VERSION_11
	targetCompatibility = JavaVersion.VERSION_11
}

repositories {
	mavenCentral()
}

cup {
	generateDir = "build/generated/sources/main"
	setArgs("-nonterms", "-expect", "150", "-interface", "-locations")
}

application {
	mainClass = "team.lmao.Main"
}

dependencies {
	implementation("commons-cli:commons-cli:1.5.0")
	implementation("com.github.vbmacher:java-cup-runtime:11b")

	testImplementation(platform("org.junit:junit-bom:5.9.1"))
	testImplementation("org.junit.jupiter:junit-jupiter:5.9.2")
}


tasks.test {
	useJUnitPlatform()
}

tasks.create("exec", JavaExec::class) {
	mainClass = application.mainClass
	classpath = sourceSets["main"].runtimeClasspath
	workingDir = file("buildSrc/src/main/resources/examples")
}

tasks.create("uber", Jar::class) {
	archiveVersion = ""
	manifest {
		attributes["Implementation-Version"] = version
		attributes["Implementation-Title"] = project.name
		attributes["Main-Class"] = application.mainClass
	}
	from(configurations.runtimeClasspath.get().map { if (it.isDirectory) it else zipTree(it) })
	with(tasks.jar.get() as CopySpec)
	duplicatesStrategy = DuplicatesStrategy.EXCLUDE
}

tasks.create("comprun", CompileRun::class) {
	mainClass = application.mainClass
	classpath = sourceSets["main"].runtimeClasspath
	workingDir = file("buildSrc/src/main/resources/examples")
}

tasks.create("smoke", SmokeTest::class) {
	dependsOn("installDist")
	mainClass = application.mainClass
}


spotless {
	encoding("UTF-8")
	java {
		eclipse().configFile("codestyle.xml")
		target("src/main/java/**/*.java")
		trimTrailingWhitespace()
		indentWithTabs(2)
		endWithNewline()
		importOrder("java", "org.apache", "lmao.team")
		removeUnusedImports()
	}
	kotlin {
		ktlint().editorConfigOverride(mapOf("ktlint_standard_comment-wrapping" to "disabled", "max_line_length" to 500))
		target("buildSrc/src/main/kotlin/**/*.kt")
		trimTrailingWhitespace()
		indentWithTabs()
		endWithNewline()
	}
	kotlinGradle {
		ktlint().editorConfigOverride(mapOf("ktlint_standard_no-consecutive-blank-lines" to "disabled"))
		target("**/*.gradle.kts")
		trimTrailingWhitespace()
		indentWithTabs()
		endWithNewline()
	}
}
