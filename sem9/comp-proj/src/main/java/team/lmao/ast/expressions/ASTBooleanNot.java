package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;


public class ASTBooleanNot implements ASTExpression {
	public final ASTExpression e;

	public ASTBooleanNot(ASTExpression e) {
		this.e = e;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

}
