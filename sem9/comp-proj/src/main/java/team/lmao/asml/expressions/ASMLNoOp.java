package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// NOOP
public class ASMLNoOp implements ASMLExpression {
	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
