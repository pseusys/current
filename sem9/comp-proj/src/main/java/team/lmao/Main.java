package team.lmao;

import java.io.FileReader;
import java.io.FileWriter;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

import org.apache.commons.cli.*;

import team.lmao.asml.structures.ASMLProgram;
import team.lmao.ast.AST;
import team.lmao.ast.expressions.ASTFunctionDefine;
import team.lmao.ast.typechecking.EquationGen;
import team.lmao.ast.visitors.ASMLgeneration;
import team.lmao.ast.visitors.AlphaConversionVisitor;
import team.lmao.ast.visitors.ClosureVisitor;
import team.lmao.exceptions.FileException;
import team.lmao.exceptions.MisConfigurationException;
import team.lmao.utils.ConsoleApp;
import team.lmao.utils.FileUtils;
import team.lmao.utils.Logger;

public class Main extends ConsoleApp {
	public static void main(String[] argv) {
		String name = "Welcome to LMAO MinCaml compiler!";
		addOption(Option.builder("o").desc("Output file name (without an extension), will be generated if options '-v', '-h', '-t' and '-p' are not present.").hasArg().build());
		addOption(Option.builder("t").desc("Type check (MinCAML source only) - returns 1 if not well-typed, 0 if well-typed.").build());
		addOption(Option.builder("p").desc("Syntax check - returns 1 if syntax incorrect, 0 if syntax is correct.").build());
		addOption(Option.builder("asml").desc("Generate ASML source file instead of ARM.").build());
		String argsHeader = "FILE: Source code file name (will be parsed as MinCAML or ASML according to extension).\nOPTIONS:";
		System.exit(initApp(name, argv, "[OPTIONS] <FILE>", argsHeader, Main.class));
	}

	private static void typechecking(AST tree, Logger logger) {
		logger.infoln("MinCaml file type checking...");
		EquationGen.start(tree.root);

		if (!EquationGen.getListEq().isEmpty()) {
			logger.errorln("MinCaml file is not well-typed!");
			System.exit(1); // Exit with status 1 if not well-typed
		}

		logger.successln("MinCaml file well-typed!");
	}

	private static ASMLProgram frontend(AST tree, Logger logger) {
		logger.infoln("Running compiler frontend...");
		logger.infoln("Cloning AST...");
		AST tree_cpy = tree.copy();
		logger.debugln("AST representation: " + tree_cpy.serialize());

		logger.infoln("K-Normalizing AST...");
		AST kNorm = tree_cpy.k_normalize();
		logger.debugln("AST representation: " + kNorm.serialize());

		logger.infoln("Test Alpha-conversion...");
		kNorm.accept(new AlphaConversionVisitor(logger));
		logger.debugln("AST representation: " + kNorm.serialize());

		logger.infoln("Let Reducing...");
		AST flattenedAst = kNorm.let_reduce();
		logger.debugln("AST representation: " + flattenedAst.serialize());

		logger.infoln("Closure Converting...");
		AST clos = flattenedAst.closure_convert();
		logger.debugln("AST representation: " + clos.serialize());
		List<ASTFunctionDefine> list = ClosureVisitor.returnListFunctions();

		logger.infoln("ASML generation...");
		clos.accept(new ASMLgeneration(logger, list));
		ASMLProgram program = ASMLgeneration.constructProgram();
		logger.debugln("AST representation: " + program.serialize());

		logger.successln("Compiler frontend succeeded!");
		return program;
	}

	private static String backend(ASMLProgram program, String fileName, Logger logger) {
		logger.infoln("Running compiler backend...");
		ASMLProgram assignedRegisters = program.assignRegisters();
		logger.debug("ASML with assigned registers: \n" + assignedRegisters.serialize());
		String ARMOutput = assignedRegisters.generate(ASMLProgram.GeneratorType.ARM_GENERATOR, fileName);
		logger.successln("Compiler backend succeeded!");
		return ARMOutput;
	}

	@SuppressWarnings({"unused", "SameReturnValue"})
	public static int launch(CommandLine CLI, CommandLine myCLI, Logger logger) throws Exception {
		if (CLI.getArgList().isEmpty())
			throw new FileException("Input file name is not provided!");
		Path source = Paths.get(CLI.getArgList().get(0));
		if (!source.toFile().exists())
			throw new FileException("Input file doesn't exist!");
		else
			logger.infoln("Input file found: " + source);

		FileReader sourceReader = new FileReader(source.toFile());
		String lowerCaseName = source.toString().toLowerCase();

		ASMLProgram program;
		if (lowerCaseName.endsWith(".ml")) {
			AST tree = AST.parse(sourceReader, logger);
			if (CLI.hasOption("p"))
				return 0;
			typechecking(tree, logger);
			if (CLI.hasOption("t"))
				return 0;
			program = frontend(tree, logger);
		} else if (lowerCaseName.endsWith(".asml")) {
			if (CLI.hasOption("t"))
				throw new MisConfigurationException("Type checking is not supported for ASML sources!");
			program = ASMLProgram.parse(sourceReader, logger);
			if (CLI.hasOption("p"))
				return 0;
		} else
			throw new FileException("Input file extension can not be recognised!");

		Path build;
		if (CLI.hasOption("o"))
			build = Paths.get(CLI.getOptionValue("o"));
		else
			build = FileUtils.changeExtension(source, "s");
		logger.infoln("Output file will be used: " + build);

		FileWriter buildWriter = new FileWriter(build.toFile(), false);
		if (CLI.hasOption("asml"))
			buildWriter.write(program.serialize());
		else
			buildWriter.write(backend(program, source.toString(), logger));
		buildWriter.flush();
		buildWriter.close();

		if (!build.toFile().exists())
			throw new FileException("Output file doesn't exist!");
		else
			logger.infoln("Output file found: " + build);
		return 0;
	}
}
