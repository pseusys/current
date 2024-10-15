package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// NEG IDENT
public class ASMLNegativeIdentifier implements ASMLExpression {
	public ASMLIdentifier operator;

	public ASMLNegativeIdentifier(ASMLIdentifier operator) {
		this.operator = operator;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
