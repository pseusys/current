package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// MEM LPAREN IDENT PLUS ident_or_imm RPAREN ASSIGN IDENT
public class ASMLMemoryAssignment implements ASMLExpression {
	public ASMLIdentifier base, assign;
	public ASMLIdentifierOrImmediate offset;

	private static int GLOBAL_STATE = -1;

	public ASMLMemoryAssignment(ASMLIdentifier base, ASMLIdentifierOrImmediate offset, ASMLIdentifier assign) {
		this.base = base;
		this.offset = offset;
		this.assign = assign;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	public static String gen() {
		GLOBAL_STATE++;
		return "tu" + GLOBAL_STATE;
	}
}
