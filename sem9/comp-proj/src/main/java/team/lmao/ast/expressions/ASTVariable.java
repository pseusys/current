package team.lmao.ast.expressions;

import java.util.ArrayList;
import java.util.List;

import team.lmao.ast.Id;
import team.lmao.ast.visitors.Visitor;

public class ASTVariable implements ASTExpression {
	public Id id;
	private static List<ASTVariable> vars = new ArrayList<>();

	public ASTVariable(Id id) {
		this.id = id;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	public static ASTVariable get(String lab) {
		for (ASTVariable v : vars) {
			if (v.id.toString().equals(lab)) {
				return v;
			}
		}
		ASTVariable newVar = new ASTVariable(new Id(lab));
		vars.add(newVar);
		return newVar;
	}
}
