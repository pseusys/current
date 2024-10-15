package team.lmao.ast.types;

import java.util.List;

import team.lmao.ast.visitors.EquationVisitor;


public class TypeTuple extends Type {
	public List<Type> elementTypes;

	public TypeTuple(List<Type> elementTypes) {
		this.elementTypes = elementTypes;
	}

	@Override
	public String toString() {
		StringBuilder stringB = new StringBuilder("(");
		for (int i = 0; i < elementTypes.size(); i++) {
			if (i > 0) {
				stringB.append(", ");
			}
		}
		stringB.append(")");
		return stringB.toString();
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}

}
