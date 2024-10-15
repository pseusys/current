package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeBoolean extends Type {
	@Override
	public String toString() {
		return "boolean";
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}

}
