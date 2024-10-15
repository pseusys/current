package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// FDIV IDENT IDENT
public class ASMLDivideFloat implements ASMLExpression {
	public ASMLIdentifier op1, op2;

	public ASMLDivideFloat(ASMLIdentifier op1, ASMLIdentifier op2) {
		this.op1 = op1;
		this.op2 = op2;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
