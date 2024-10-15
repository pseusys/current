package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// ADD IDENT ident_or_imm
public class ASMLAddIdentifierOrImmediate implements ASMLExpression {
	public ASMLIdentifier op1;
	public ASMLIdentifierOrImmediate op2;

	public ASMLAddIdentifierOrImmediate(ASMLIdentifier op1, ASMLIdentifierOrImmediate op2) {
		this.op1 = op1;
		this.op2 = op2;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
