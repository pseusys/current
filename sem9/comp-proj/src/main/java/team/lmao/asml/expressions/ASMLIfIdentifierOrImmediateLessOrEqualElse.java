package team.lmao.asml.expressions;

import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.visitors.Visitor;

// IF IDENT LE ident_or_imm THEN block ELSE block
public class ASMLIfIdentifierOrImmediateLessOrEqualElse implements ASMLExpression {
	public ASMLIdentifier comp1;
	public ASMLIdentifierOrImmediate comp2;
	public ASMLBlock ifBlock, elseBlock;

	public ASMLIfIdentifierOrImmediateLessOrEqualElse(ASMLIdentifier comp1, ASMLIdentifierOrImmediate comp2, ASMLBlock ifBlock, ASMLBlock elseBlock) {
		this.comp1 = comp1;
		this.comp2 = comp2;
		this.ifBlock = ifBlock;
		this.elseBlock = elseBlock;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
