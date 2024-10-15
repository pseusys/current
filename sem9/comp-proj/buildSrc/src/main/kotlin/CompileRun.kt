import org.gradle.api.provider.Property
import org.gradle.api.tasks.Internal
import org.gradle.api.tasks.JavaExec
import org.gradle.api.tasks.options.Option
import java.io.ByteArrayOutputStream
import java.io.File

open class CompileRun : JavaExec() {
	private companion object {
		const val ARM_ASSEMBLY_FLAVOUR = "ARM"
		const val INTEL_ASSEMBLY_FLAVOUR = "INTEL"
		const val DEFAULT_ASSEMBLY_FLAVOUR = ARM_ASSEMBLY_FLAVOUR

		const val DEFAULT_ARM_COMPILER = "arm-none-eabi-as"
		const val DEFAULT_ARM_LINKER = "arm-none-eabi-ld"
		val ARM_ASSEMBLY_FLAGS = arrayOf("-mfpu=neon", "-mfloat-abi=soft")
		val ARM_LINKING_FLAGS = arrayOf<String>()

		const val DEFAULT_INTEL_COMPILER = "nasm"
		const val DEFAULT_INTEL_LINKER = "gcc"
		val INTEL_ASSEMBLY_FLAGS = arrayOf<String>()
		val INTEL_LINKING_FLAGS = arrayOf<String>()

		const val DEFAULT_ARM_RUNNER = "qemu-arm"
		const val DEFAULT_INTEL_RUNNER = "bash"
	}

	@Internal
	@Option(description = "Flavour of assembly to use")
	val flavour: Property<String> = project.objects.property(String::class.java)

	@Internal
	@Option(description = "Compiler that will be used for assembly code compilation name")
	val compiler: Property<String> = project.objects.property(String::class.java)

	@Internal
	@Option(description = "Linker that will be used for assembly code compilation name")
	val linker: Property<String> = project.objects.property(String::class.java)

	@Internal
	@Option(description = "Produced binary file name")
	val binary: Property<String> = project.objects.property(String::class.java)

	@Internal
	@Option(description = "Runner for executing generated executable (can be /bin/bash if native)")
	val runner: Property<String> = project.objects.property(String::class.java)

	private fun runCommand(command: List<String>): String {
		var success = false
		val outputStream = ByteArrayOutputStream()
		try {
			val execution =
				project.exec {
					commandLine(command)
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

	private fun execBuild(executable: String, flags: Array<String>, output: File, vararg target: File) = runCommand(listOf(executable, *flags, "-o", output.path, *target.map { it.path }.toTypedArray()))
	private fun execRun(executable: String, target: File) = runCommand(listOf(executable, target.path))

	override fun exec() {
		super.exec()

		val flavour = if (flavour.isPresent) flavour.get() else DEFAULT_ASSEMBLY_FLAVOUR
		if (flavour != ARM_ASSEMBLY_FLAVOUR && flavour != INTEL_ASSEMBLY_FLAVOUR) {
			throw RuntimeException("Unknown assembly target flavour: $flavour")
		}

		val compiler = if (compiler.isPresent) compiler.get() else if (flavour == ARM_ASSEMBLY_FLAVOUR) DEFAULT_ARM_COMPILER else DEFAULT_INTEL_COMPILER
		val linker = if (linker.isPresent) linker.get() else if (flavour == ARM_ASSEMBLY_FLAVOUR) DEFAULT_ARM_LINKER else DEFAULT_INTEL_LINKER
		val runner = if (runner.isPresent) runner.get() else if (flavour == ARM_ASSEMBLY_FLAVOUR) DEFAULT_ARM_RUNNER else DEFAULT_INTEL_RUNNER

		val assemblyFlags = if (flavour == ARM_ASSEMBLY_FLAVOUR) ARM_ASSEMBLY_FLAGS else INTEL_ASSEMBLY_FLAGS
		val linkingFlags = if (flavour == ARM_ASSEMBLY_FLAVOUR) ARM_LINKING_FLAGS else INTEL_LINKING_FLAGS

		val workDirPrefix = workingDir.path + "/"
		val stdlibAssembly = project.layout.projectDirectory.dir("buildSrc/src/main/resources/stdlibs").file("${flavour.lowercase()}_stdlib.s").asFile
		val outputAssembly = File(workDirPrefix + if (args.contains("-o")) args[args.indexOf("-o") + 1] else args.last().split(".")[0] + ".s")

		val binaryName = if (binary.isPresent) workDirPrefix + binary.get() else outputAssembly.path
		val binaryObject = File(binaryName.split(".")[0] + ".o")
		val binaryExecutable = File(binaryName.split(".")[0] + ".run")

		println("\nCompiling runnable '${outputAssembly.name}'...")
		execBuild(compiler, assemblyFlags, binaryObject, outputAssembly, stdlibAssembly)
		println("Linking runnable '${binaryObject.name}' with '${stdlibAssembly.name}'...")
		execBuild(linker, linkingFlags, binaryExecutable, binaryObject)
		println("Runnable '${binaryExecutable.name}' linked and compiled successfully!")

		binaryObject.delete()

		println("\nRunning '${binaryExecutable.name}'...")
		println("Runnable output:\n${execRun(runner, binaryExecutable)}")
		println("\nRunnable '${binaryExecutable.name}' finished successfully!")
	}
}
