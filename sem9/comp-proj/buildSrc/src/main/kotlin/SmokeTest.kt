import org.gradle.api.DefaultTask
import org.gradle.api.GradleException
import org.gradle.api.provider.Property
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.TaskAction
import java.io.ByteArrayOutputStream
import java.io.File

open class SmokeTest : DefaultTask() {
	@Input
	val mainClass: Property<String> = project.objects.property(String::class.java)

	private fun runSmokeTest(command: List<String?>, test: File, expectSuccess: Boolean): Boolean {
		var success = false
		val outputStream = ByteArrayOutputStream()
		try {
			val execution =
				project.exec {
					commandLine(command + listOf(test.path))
					standardOutput = outputStream
					errorOutput = outputStream
				}
			success = (execution.exitValue == 0) == expectSuccess
		} catch (_: Throwable) {
			success = !expectSuccess
		} finally {
			if (!success) {
				println("Execution failed for input file ${test.name}: ${outputStream.toString().trim()}")
			}
		}
		return success
	}

	@TaskAction
	fun smokeTest() {
		val classDir = project.layout.buildDirectory.dir("install/${project.name}/lib").get().asFile
		val classpath = classDir.listFiles()?.joinToString(":")
		val command = listOf("java", "-ea", "-cp", classpath, mainClass.get())
		val testDir = project.layout.projectDirectory.dir("buildSrc/src/main/resources")

		val success =
			testDir.dir("tests").asFile.listFiles()!!.fold(true) { res, set ->
				File("${set.path}/valid").listFiles()!!.fold(res) { vres, test ->
					vres and runSmokeTest(command, test, true)
				} and
					File("${set.path}/invalid").listFiles()!!.fold(res) { ires, test ->
						ires and runSmokeTest(command, test, false)
					}
			} and
				testDir.dir("examples").asFile.listFiles()!!.fold(true) { res, ex ->
					res and
						when (ex.extension) {
							"ml" -> runSmokeTest(command, ex, true)
							"asml" -> runSmokeTest(command + listOf("-asml"), ex, true)
							else -> {
								println("Unknown source file type: ${ex.extension}")
								false
							}
						}
				}
		if (!success) throw GradleException("Smoke test execution failed!")
	}
}
