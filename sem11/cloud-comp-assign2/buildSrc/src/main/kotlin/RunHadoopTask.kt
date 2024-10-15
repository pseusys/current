import org.gradle.api.tasks.InputFile
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.OutputDirectory
import org.gradle.api.tasks.TaskAction
import org.gradle.api.tasks.Optional
import org.gradle.api.file.RegularFileProperty
import org.gradle.api.file.DirectoryProperty
import org.gradle.api.file.Directory
import org.gradle.api.provider.Property

abstract class RunHadoopTask : HadoopTask() {
    private companion object {
        const val DEFAULT_INPUT_FILE = "1400-8.txt"
	}

    @get:InputFile
	abstract val input: RegularFileProperty

	@Optional
	@get:OutputDirectory
	val output: DirectoryProperty = project.objects.directoryProperty()

	@get:OutputDirectory
	val outputFile: Directory
		get() = if (output.isPresent) output.get() else project.layout.projectDirectory.dir("${HadoopTask.BASE_OUTPUT}/${main.get()}")

	@get:Input
	abstract val reducers: Property<Int?>

    init {
		input.convention(project.layout.projectDirectory.file(DEFAULT_INPUT_FILE))
		reducers.convention(null as Int?)
	}

    @TaskAction
	fun run() {
        runCommand(input.get().asFile, resolveArchive().asFile, main.get(), outputFile.asFile, reducers.get(), null)
	}
}
