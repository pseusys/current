import org.gradle.api.tasks.InputDirectory
import org.gradle.api.tasks.TaskAction
import org.gradle.api.tasks.Optional
import org.gradle.api.tasks.Input
import org.gradle.api.file.DirectoryProperty
import org.gradle.api.file.Directory
import org.gradle.api.provider.Provider
import org.gradle.api.provider.Property

abstract class AnalyzeHadoopTask : HadoopTask() {
    @Optional
    @get:InputDirectory
	val input: DirectoryProperty = project.objects.directoryProperty()

	@Optional
    @get:Input
	val word: Property<String> = project.objects.property(String::class.java)

	protected fun resolveInput(): Directory {
		if (input.isPresent) return input.get()
		val runTask = getDependsOn().map { project.tasks.getByName(it.toString()) }
			.filterIsInstance(RunHadoopTask::class.java).takeIf { it.isNotEmpty() }
			?.first() ?: throw RuntimeException("AnalyzeHadoopTask should depend on a RunHadoopTask task if 'input' property is not specified!")
        return runTask.outputFile
	}

    @TaskAction
	fun run() {
		val setWord = if (word.isPresent) word.get() else null
        println(runCommand(resolveInput().asFile, resolveArchive().asFile, main.get(), null, null, setWord))
	}
}
