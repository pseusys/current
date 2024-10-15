package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeFloat extends Type {
	@Override
	public String toString() {
		return "float";
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}


}
