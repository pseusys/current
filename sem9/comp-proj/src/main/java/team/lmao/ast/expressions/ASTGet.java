package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTGet implements ASTExpression {
	public final ASTExpression e1;
	public final ASTExpression e2;

	public ASTGet(ASTExpression e1, ASTExpression e2) {
		this.e1 = e1;
		this.e2 = e2;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
