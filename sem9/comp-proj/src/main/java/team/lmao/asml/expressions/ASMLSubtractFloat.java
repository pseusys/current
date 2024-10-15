package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// FSUB IDENT IDENT
public class ASMLSubtractFloat implements ASMLExpression {
	public ASMLIdentifier op1, op2;

	public ASMLSubtractFloat(ASMLIdentifier op1, ASMLIdentifier op2) {
		this.op1 = op1;
		this.op2 = op2;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
