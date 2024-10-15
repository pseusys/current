package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// MEM LPAREN IDENT PLUS ident_or_imm RPAREN
public class ASMLMemoryRetrieval implements ASMLExpression {
	public ASMLIdentifier base;
	public ASMLIdentifierOrImmediate offset;

	public ASMLMemoryRetrieval(ASMLIdentifier base, ASMLIdentifierOrImmediate offset) {
		this.base = base;
		this.offset = offset;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
