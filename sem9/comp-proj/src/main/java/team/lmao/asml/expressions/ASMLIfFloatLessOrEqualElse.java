package team.lmao.asml.expressions;

import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.visitors.Visitor;

// IF IDENT FLE IDENT THEN block ELSE block
public class ASMLIfFloatLessOrEqualElse implements ASMLExpression {
	public ASMLIdentifier comp1, comp2;
	public ASMLBlock ifBlock, elseBlock;

	public ASMLIfFloatLessOrEqualElse(ASMLIdentifier comp1, ASMLIdentifier comp2, ASMLBlock ifBlock, ASMLBlock elseBlock) {
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
