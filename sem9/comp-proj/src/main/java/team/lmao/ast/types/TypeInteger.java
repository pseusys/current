package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeInteger extends Type {
	@Override
	public String toString() {
		return "integer";
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}


}
