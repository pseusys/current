package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// NEW ident_or_imm
public class ASMLNewPlacement implements ASMLExpression {
	public ASMLIdentifierOrImmediate operator;

	public ASMLNewPlacement(ASMLIdentifierOrImmediate operator) {
		this.operator = operator;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
