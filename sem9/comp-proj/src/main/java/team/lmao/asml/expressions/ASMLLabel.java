package team.lmao.asml.expressions;


import team.lmao.asml.visitors.Visitor;

// LABEL
public class ASMLLabel extends ASMLIdentifier {
	public static final String SPECIAL = "%self";
	public static final ASMLLabel MAIN_NAME = new ASMLLabel("_start");

	public ASMLLabel(String identifier) {
		super(identifier);
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	@Override
	protected String validateName(String name) {
		return (name.charAt(0) == '_' || name.equals(SPECIAL)) ? name : ("_" + name);
	}

	public boolean isSpecial() {
		return this.getName().equals(SPECIAL);
	}

}
