package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTArray implements ASTExpression {
	public final ASTExpression e1;
	public final ASTExpression e2;

	public ASTArray(ASTExpression e1, ASTExpression e2) {
		this.e1 = e1;
		this.e2 = e2;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
