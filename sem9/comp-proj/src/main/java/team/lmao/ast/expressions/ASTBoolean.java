package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;


public class ASTBoolean implements ASTExpression {
	public final boolean b;

	public ASTBoolean(boolean b) {
		this.b = b;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
