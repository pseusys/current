package team.lmao.ast.types;


import team.lmao.ast.visitors.EquationVisitor;

public class TypeArray extends Type {
	public Type t;

	public TypeArray(Type t) {
		this.t = t;
	}

	@Override
	public String toString() {
		return "[" + t.toString() + "]";
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}


}
