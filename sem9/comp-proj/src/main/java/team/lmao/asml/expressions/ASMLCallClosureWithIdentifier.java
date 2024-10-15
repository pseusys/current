package team.lmao.asml.expressions;

import java.util.List;

import team.lmao.asml.visitors.Visitor;


// APPCLO IDENT formal_args
public class ASMLCallClosureWithIdentifier implements ASMLExpression {
	public ASMLIdentifier identifier;
	public List<ASMLIdentifier> arguments;

	public ASMLCallClosureWithIdentifier(ASMLIdentifier identifier, List<ASMLIdentifier> arguments) {
		this.identifier = identifier;
		this.arguments = arguments;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
