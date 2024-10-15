package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;


public class ASTUnit implements ASTExpression {
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

}
