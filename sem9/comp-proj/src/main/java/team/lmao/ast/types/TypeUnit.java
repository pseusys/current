package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeUnit extends Type {
	@Override
	public String toString() {
		return "unit";
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}

}
