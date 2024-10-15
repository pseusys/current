package team.lmao.ast.expressions;

import java.util.List;

import team.lmao.ast.visitors.Visitor;

public class ASTApp implements ASTExpression {
	public final ASTExpression e;
	public final List<ASTExpression> es;

	public ASTApp(ASTExpression e, List<ASTExpression> es) {
		this.e = e;
		this.es = es;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
