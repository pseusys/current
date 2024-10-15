package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// FLOAT literal
public class ASMLFloatLiteral implements ASMLExpression {
	public float literal;
	private static int GLOBAL_STATE = -1;

	public ASMLFloatLiteral(float literal) {
		this.literal = literal;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
