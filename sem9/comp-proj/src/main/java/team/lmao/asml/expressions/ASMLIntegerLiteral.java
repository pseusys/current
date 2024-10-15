package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// INT literal
public class ASMLIntegerLiteral implements ASMLExpression, ASMLIdentifierOrImmediate {
	public int literal;

	public ASMLIntegerLiteral(int literal) {
		this.literal = literal;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
