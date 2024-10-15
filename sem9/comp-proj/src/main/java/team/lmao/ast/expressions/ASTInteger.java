package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;


public class ASTInteger implements ASTExpression {
	public final int i;

	public ASTInteger(int i) {
		this.i = i;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

}
