package team.lmao.asml.expressions;

import java.util.List;

import team.lmao.asml.visitors.Visitor;


// CALL LABEL formal_args
public class ASMLCallLabel implements ASMLExpression {
	public ASMLLabel label;
	public List<ASMLIdentifier> arguments;

	public ASMLCallLabel(ASMLLabel label, List<ASMLIdentifier> arguments) {
		this.label = label;
		this.arguments = arguments;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
