package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTIntegerNegative implements ASTExpression {
	public final ASTExpression e;

	public ASTIntegerNegative(ASTExpression e) {
		this.e = e;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
