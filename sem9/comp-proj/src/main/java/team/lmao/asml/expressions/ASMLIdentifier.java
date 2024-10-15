package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// IDENT: variable or label
public class ASMLIdentifier implements ASMLExpression, ASMLIdentifierOrImmediate {
	private String name;

	public ASMLIdentifier(String identifier) {
		this.name = validateName(identifier);
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	protected String validateName(String name) {
		return Character.toLowerCase(name.charAt(0)) + name.substring(1);
	}

	public void setName(String name) {
		this.name = validateName(name);
	}

	public String getName() {
		return name;
	}
}
