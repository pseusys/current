package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTIf implements ASTExpression {
	public final ASTExpression e1;
	public final ASTExpression e2;
	public final ASTExpression e3;

	public ASTIf(ASTExpression e1, ASTExpression e2, ASTExpression e3) {
		this.e1 = e1;
		this.e2 = e2;
		this.e3 = e3;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
