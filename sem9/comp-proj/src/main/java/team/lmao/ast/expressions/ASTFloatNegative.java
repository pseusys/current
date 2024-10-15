package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTFloatNegative implements ASTExpression {
	public final ASTExpression e;

	public ASTFloatNegative(ASTExpression e) {
		this.e = e;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
