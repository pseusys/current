package team.lmao.asml.structures;

import java.io.Reader;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

import team.lmao.asml.ASMLParser;
import team.lmao.asml.expressions.ASMLFloatLiteral;
import team.lmao.asml.expressions.ASMLIdentifier;
import team.lmao.asml.expressions.ASMLLabel;
import team.lmao.asml.generators.arm.ARMGenerator;
import team.lmao.asml.visitors.RegisterAllocationVisitor;
import team.lmao.asml.visitors.StringVisitor;
import team.lmao.asml.visitors.Visitor;
import team.lmao.exceptions.NotImplementedException;
import team.lmao.exceptions.SourceParseException;
import team.lmao.utils.Logger;
import team.lmao.utils.Pair;

public class ASMLProgram implements ASMLBase {
	public enum GeneratorType {
		ARM_GENERATOR, INTEL_GENERATOR
	}

	public static class ASMLFunction implements ASMLBase {
		public ASMLLabel name;
		public List<ASMLIdentifier> arguments;

		public ASMLFunction(ASMLLabel name, List<ASMLIdentifier> arguments) {
			this.name = name;
			this.arguments = arguments;
		}

		@Override
		public <E> E accept(Visitor<E> v) {
			return v.visit(this);
		}
	}

	public List<Pair<ASMLLabel, ASMLFloatLiteral>> globals;
	public List<Pair<ASMLFunction, ASMLBlock>> functions;
	public ASMLBlock main;

	public ASMLProgram(List<Pair<ASMLLabel, ASMLFloatLiteral>> globals, List<Pair<ASMLFunction, ASMLBlock>> functions, ASMLBlock main) {
		this.globals = globals;
		this.functions = functions;
		this.main = main;
	}

	public ASMLProgram(ASMLBlock main) {
		this(new LinkedList<>(), new LinkedList<>(), main);
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	public static ASMLProgram parse(Reader reader, Logger logger) {
		try {
			logger.infoln("Source ASML file parsing...");
			ASMLParser p = new ASMLParser(reader);
			ASMLProgram program = (ASMLProgram) p.parse().value;
			logger.successln("ASML source code parsed!");
			logger.debugln("ASML representation: \n" + program.serialize());
			return program;
		} catch (Exception e) {
			throw new SourceParseException("ASML parsing exception", e);
		}
	}

	public String serialize() {
		return accept(new StringVisitor());
	}

	public ASMLProgram assignRegisters() {
		return (ASMLProgram) accept(new RegisterAllocationVisitor()).get(0);
	}

	public String generate(GeneratorType type, String fileName) {
		List<String> assemblyCommands;
		switch (type) {
			case ARM_GENERATOR:
				assemblyCommands = accept(new ARMGenerator(fileName));
				break;
			default:
				throw new NotImplementedException("Assembly generator " + type.name() + " is not implemented");
		}
		return assemblyCommands.stream().map(comm -> comm.endsWith(":") || comm.isEmpty() ? comm : "\t" + comm).collect(Collectors.joining("\n"));
	}
}
