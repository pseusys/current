package team.lmao.ast.visitors;

import java.util.stream.Collectors;

import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;

public class StringVisitor implements Visitor<String> {
	public String visit(ASTUnit e) {
		return "()";
	}

	public String visit(ASTBoolean e) {
		return String.valueOf(e.b);
	}

	public String visit(ASTInteger e) {
		return String.valueOf(e.i);
	}

	public String visit(ASTFloat e) {
		return String.format("%.2f", e.f);
	}

	public String visit(ASTBooleanNot e) {
		return "(not " + e.e.accept(this) + ")";
	}

	public String visit(ASTIntegerNegative e) {
		return "(- " + e.e.accept(this) + ")";
	}

	public String visit(ASTIntegerAdd e) {
		return "(" + e.e1.accept(this) + " + " + e.e2.accept(this) + ")";
	}

	public String visit(ASTIntegerSubtract e) {
		return "(" + e.e1.accept(this) + " - " + e.e2.accept(this) + ")";
	}

	public String visit(ASTFloatNegative e) {
		return "(-. " + e.e.accept(this) + ")";
	}

	public String visit(ASTFloatAdd e) {
		return "(" + e.e1.accept(this) + " +. " + e.e2.accept(this) + ")";
	}

	public String visit(ASTFloatSubtract e) {
		return "(" + e.e1.accept(this) + " -. " + e.e2.accept(this) + ")";
	}

	public String visit(ASTFloatMultiply e) {
		return "(" + e.e1.accept(this) + " *. " + e.e2.accept(this) + ")";
	}

	public String visit(ASTFloatDivide e) {
		return "(" + e.e1.accept(this) + " /. " + e.e2.accept(this) + ")";
	}

	@Override
	public String visit(ASTFunctionDefine e) {
		String args = e.args.stream().map(Id::toString).collect(Collectors.joining(" "));
		if (e.free_var.equals("None")) {
			return "apply_direct(" + e.label + ", " + args + ")";
		}
		return "apply_closure(" + e.label + ", " + args + ")";
	}

	public String visit(ASTIntegerEqual e) {
		return "(" + e.e1.accept(this) + " = " + e.e2.accept(this) + ")";
	}

	public String visit(ASTIntegerLessOrEqual e) {
		return "(" + e.e1.accept(this) + " <= " + e.e2.accept(this) + ")";
	}

	public String visit(ASTIf e) {
		return "(if " + e.e1.accept(this) + " then " + e.e2.accept(this) + " else " + e.e3.accept(this) + ")";
	}

	public String visit(ASTLet e) {
		return "\n(let " + e.id + " = " + e.e1.accept(this) + " in " + e.e2.accept(this) + ")";
	}

	public String visit(ASTVariable e) {
		return e.id.toString();
	}

	public String visit(ASTLetRec e) {
		String args = e.fd.args.stream().map(Id::toString).collect(Collectors.joining(" "));
		return "\n(let rec " + e.fd.id + " " + args + " = " + e.fd.e.accept(this) + " in " + e.e.accept(this) + ")";
	}

	public String visit(ASTApp e) {
		String args = e.es.stream().map(ex -> ex.accept(this)).collect(Collectors.joining(","));
		return "(" + e.e.accept(this) + " " + args + ")";
	}

	public String visit(ASTTuple e) {
		String args = e.es.stream().map(ex -> ex.accept(this)).collect(Collectors.joining(", "));
		return "(" + args + ")";
	}

	public String visit(ASTLetTuple e) {
		String ids = e.ids.stream().map(Id::toString).collect(Collectors.joining(", "));
		return "(let (" + ids + ") = " + e.e1.accept(this) + " in " + e.e2.accept(this) + ")";
	}

	public String visit(ASTArray e) {
		return "(Array.create " + e.e1.accept(this) + " " + e.e2.accept(this) + ")";
	}

	public String visit(ASTGet e) {
		return e.e1.accept(this) + ".(" + e.e2.accept(this) + ")";
	}

	public String visit(ASTPut e) {
		return "(" + e.e1.accept(this) + ".(" + e.e2.accept(this) + ") <- " + e.e3.accept(this) + ")";
	}

	public String visit(ASTClosure e) {
		String args = e.args.stream().map(Id::toString).collect(Collectors.joining(" "));
		return "make_closure(" + e.label + ", " + args + ")";
	}

	public String visit(ASTCallClosure e) {
		String args = e.args.stream().map(Id::toString).collect(Collectors.joining(" "));
		return "apply_closure(" + e.id.toString() + ", " + args + ")";
	}

}
