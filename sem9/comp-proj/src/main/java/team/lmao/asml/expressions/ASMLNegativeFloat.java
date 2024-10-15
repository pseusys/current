package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// FNEG IDENT
public class ASMLNegativeFloat implements ASMLExpression {
	public ASMLIdentifier operator;

	public ASMLNegativeFloat(ASMLIdentifier operator) {
		this.operator = operator;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
