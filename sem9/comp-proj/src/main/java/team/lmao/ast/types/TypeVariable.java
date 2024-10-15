package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeVariable extends Type {
	public String v;

	public TypeVariable(String v) {
		this.v = v;
	}

	@Override
	public String toString() {
		return v;
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}

}
