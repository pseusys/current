import org.gradle.api.DefaultTask
import org.gradle.api.tasks.Optional
import org.gradle.api.tasks.InputFile
import org.gradle.api.tasks.Input
import org.gradle.api.file.RegularFileProperty
import org.gradle.api.file.RegularFile
import org.gradle.api.provider.Property
import org.gradle.api.provider.Provider
import org.gradle.jvm.tasks.Jar

import java.io.File
import java.io.ByteArrayOutputStream
import kotlin.collections.listOf

abstract class HadoopTask : DefaultTask() {
	protected companion object {
        const val HADOOP_VOLUME = "/opt/hadoop/app"
        const val HADOOP_IMAGE = "apache/hadoop:3.4.0"
		const val BASE_OUTPUT = "output"
	}

	@Optional
	@get:InputFile
	val archive: RegularFileProperty = project.objects.fileProperty()

	@get:Input
	abstract val main: Property<String>

	protected fun runCommand(input: File, executable: File, main: String, output: File?, reducers: Int?, word: String?): String {
		val inputFile = input.relativeTo(project.projectDir).path
		val executableFile = executable.relativeTo(project.projectDir).path
		val outputFile = output?.relativeTo(project.projectDir)?.path

		var subcommand = listOf("hadoop", "jar", "$HADOOP_VOLUME/$executableFile", "hk.ust.comp4651.$main", "-input", "$HADOOP_VOLUME/$inputFile")
		if (outputFile != null) subcommand += listOf("-output", "$HADOOP_VOLUME/$outputFile")
		if (reducers != null) subcommand += listOf("-numReducers", "$reducers")
		if (word != null) subcommand += listOf("-word", word)
        val command = listOf("docker", "run", "--rm", "-v", "${project.projectDir}:$HADOOP_VOLUME", HADOOP_IMAGE) + subcommand

		var success = false
		val outputStream = ByteArrayOutputStream()
		try {
			val execution = project.exec {
				commandLine = command
		    	standardOutput = outputStream
				errorOutput = outputStream
			}
			success = execution.exitValue == 0
		} catch (_: Throwable) {
			success = false
		} finally {
			if (!success) {
				throw RuntimeException("Running command \"${command.joinToString(" ")}\" failed: ${outputStream.toString().trim()}")
			}
		}
		return outputStream.toString().trim()
	}

	protected fun resolveArchive(): RegularFile {
		if (archive.isPresent) return archive.get()
		val buildTask = getDependsOn().map { project.tasks.getByName(it.toString()) }
			.filterIsInstance(Jar::class.java).takeIf { it.isNotEmpty() }
			?.first() ?: throw RuntimeException("HadoopTask should depend on 'jar' task if 'archive' property is not specified!")
		return buildTask.archiveFile.get()
	}
}
