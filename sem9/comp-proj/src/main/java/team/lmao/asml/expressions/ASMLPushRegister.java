package team.lmao.asml.expressions;

import team.lmao.asml.visitors.Visitor;

public class ASMLPushRegister implements ASMLExpression {
	public ASMLRegister register;

	public ASMLPushRegister(ASMLRegister register) {
		this.register = register;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
