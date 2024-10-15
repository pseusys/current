package team.lmao.asml.expressions;

import team.lmao.asml.visitors.Visitor;
import team.lmao.utils.Pair;

public class ASMLAssignment extends Pair<ASMLIdentifier, ASMLExpression> implements ASMLExpression {
	public ASMLAssignment(ASMLIdentifier left, ASMLExpression right) {
		super(left, right);
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
