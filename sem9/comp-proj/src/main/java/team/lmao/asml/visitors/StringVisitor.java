package team.lmao.asml.visitors;

import java.util.stream.Collectors;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;

public class StringVisitor implements Visitor<String> {
	@Override
	public String visit(ASMLAddFloat e) {
		return "fadd " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLAddIdentifierOrImmediate e) {
		return "add " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLAssignment e) {
		return "let " + e.left.accept(this) + " = " + e.right.accept(this) + " in";
	}

	@Override
	public String visit(ASMLCallClosureWithIdentifier e) {
		return "call_closure " + e.identifier.accept(this) + " " + e.arguments.stream().map(arg -> arg.accept(this)).collect(Collectors.joining(" "));
	}

	@Override
	public String visit(ASMLCallLabel e) {
		return "call " + e.label.accept(this) + " " + e.arguments.stream().map(arg -> arg.accept(this)).collect(Collectors.joining(" "));
	}

	@Override
	public String visit(ASMLDivideFloat e) {
		return "fdiv " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLFloatLiteral e) {
		return String.valueOf(e.literal);
	}

	@Override
	public String visit(ASMLIdentifier e) {
		return e.getName();
	}

	@Override
	public String visit(ASMLIfFloatEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " =. " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIfFloatGreaterOrEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " >=. " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIfFloatLessOrEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " =<. " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIfIdentifierOrImmediateEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " = " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIfIdentifierOrImmediateGreaterOrEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " >= " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIfIdentifierOrImmediateLessOrEqualElse e) {
		String ifBlock = "( \n\t" + String.join("\n\t", e.ifBlock.accept(this).split("\n")) + "\n)";
		String elseBlock = "( \n\t" + String.join("\n\t", e.elseBlock.accept(this).split("\n")) + "\n)";
		return "if " + e.comp1.accept(this) + " <= " + e.comp2.accept(this) + " then " + ifBlock + " else " + elseBlock;
	}

	@Override
	public String visit(ASMLIntegerLiteral e) {
		return String.valueOf(e.literal);
	}

	@Override
	public String visit(ASMLLabel e) {
		return e.getName();
	}

	@Override
	public String visit(ASMLMemoryAssignment e) {
		return "mem(" + e.base.accept(this) + " + " + e.offset.accept(this) + ") <- " + e.assign.accept(this);
	}

	@Override
	public String visit(ASMLMemoryRetrieval e) {
		return "mem(" + e.base.accept(this) + " + " + e.offset.accept(this) + ")";
	}

	@Override
	public String visit(ASMLMultiplyFloat e) {
		return "fmul " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLNegativeFloat e) {
		return "fneg " + e.operator.accept(this);
	}

	@Override
	public String visit(ASMLNegativeIdentifier e) {
		return "neg " + e.operator.accept(this);
	}

	@Override
	public String visit(ASMLNewPlacement e) {
		return "new " + e.operator.accept(this);
	}

	@Override
	public String visit(ASMLNoOp e) {
		return "nop";
	}

	@Override
	public String visit(ASMLRegister e) {
		return e.getName();
	}

	@Override
	public String visit(ASMLPushRegister e) {
		return "push " + e.register.accept(this);
	}

	@Override
	public String visit(ASMLSubtractFloat e) {
		return "fsub " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLSubtractIdentifierOrImmediate e) {
		return "sub " + e.op1.accept(this) + " " + e.op2.accept(this);
	}

	@Override
	public String visit(ASMLBlock e) {
		return e.assignments.stream().reduce("", (acc, ass) -> acc + ass.accept(this) + "\n", String::concat) + e.result.accept(this) + "\n";
	}

	@Override
	public String visit(ASMLProgram.ASMLFunction e) {
		return "let " + e.name.accept(this) + " " + e.arguments.stream().reduce("", (inn, id) -> inn + id.accept(this) + " ", String::concat) + "= ";
	}

	@Override
	public String visit(ASMLProgram e) {
		String floats = e.globals.stream().reduce("", (acc, glo) -> acc + "let " + glo.left.accept(this) + " = " + glo.right.accept(this) + "\n", String::concat);
		String funcs = e.functions.stream().reduce("", (acc, fun) -> acc + fun.left.accept(this) + "\n\t" + String.join("\n\t", fun.right.accept(this).split("\n")) + "\n", String::concat);
		return floats + (e.globals.isEmpty() ? "" : "\n") + funcs + "let _ = \n\t" + String.join("\n\t", e.main.accept(this).split("\n")) + "\n";
	}
}
