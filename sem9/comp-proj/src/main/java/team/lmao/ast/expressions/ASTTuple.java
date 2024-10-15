package team.lmao.ast.expressions;

import java.util.List;

import team.lmao.ast.visitors.Visitor;

public class ASTTuple implements ASTExpression {
	public final List<ASTExpression> es;

	public ASTTuple(List<ASTExpression> es) {
		this.es = es;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
